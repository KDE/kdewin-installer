/****************************************************************************
**
** Copyright (C) 2005-2006 Ralf Habacker. All rights reserved.
**
** This file is part of the KDE installer for windows
**
** This file may be used under the terms of the GNU General Public
** License version 2.0 as published by the Free Software Foundation
** and appearing in the file LICENSE.GPL included in the packaging of
** this file.  Please review the following information to ensure GNU
** General Public Licensing requirements will be met:
** http://www.trolltech.com/products/qt/opensource.html
**
** If you are unsure which license is appropriate for your use, please
** review the following information:
** http://www.trolltech.com/products/qt/licensing.html or contact the
** sales department at sales@trolltech.com.
**
** This file is provided AS IS with NO WARRANTY OF ANY KIND, INCLUDING THE
** WARRANTY OF DESIGN, MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE.
**
****************************************************************************/

#include <QtCore>

#include <quazip/quazip.h>
#include <quazip/quazipfile.h>

#include "installer.h"
#include "packagelist.h"
//#define DEBUG

InstallerBase::InstallerBase(PackageList *_packageList)
	: QObject()
{
	root = ".";
	packageList = _packageList;
	packageList->installer = this;
	packageList->root = root;

	connect (packageList,SIGNAL(loadedConfig()),this,SLOT(updatePackageList()));
}

InstallerBase::~InstallerBase()
{
}

bool InstallerBase::isEnabled() 
{
	return false;
}

void InstallerBase::setRoot(const QString &_root) 
{ 
	root = _root; 
	packageList->root = root;
	QDir dir;
	dir.mkdir(root);
}

bool InstallerBase::loadConfig()
{
	return false;
}

bool InstallerBase::install(const QString &fileName)
{
	return false;
}

void InstallerBase::updatePackageList()
{
#ifdef DEBUG
	qDebug() << __PRETTY_FUNCTION__;
#endif
	loadConfig();
}



InstallerGNUWin32::InstallerGNUWin32(PackageList *packageList) : InstallerBase(packageList)
{
}

InstallerGNUWin32::~InstallerGNUWin32()
{
}

bool InstallerGNUWin32::isEnabled() 
{
	return QFile::exists("bin/unzip.exe");
}

bool InstallerGNUWin32::loadConfig()
{
#ifdef DEBUG
	qDebug() << __PRETTY_FUNCTION__;
#endif
	// gnuwin32 related 
	QDir dir(root + "/manifest");
	dir.setFilter(QDir::Files);
	dir.setNameFilters(QStringList("*.ver"));
	dir.setSorting(QDir::Size | QDir::Reversed);
	
	QFileInfoList list = dir.entryInfoList();
	for (int i = 0; i < list.size(); ++i) {
	  QFileInfo fileInfo = list.at(i);
	  Package pkg;
	  pkg.setFromVersionFile(fileInfo.fileName());
	  packageList->updatePackage(pkg);
	}
	return true;
}

extern "C" int unzip(char *fileName, char *rootdir);

bool InstallerGNUWin32::install(const QString &fileName)
{
#ifdef USE_EXTERNAL_ZIP
	QString cmd = "bin\\unzip.exe -o";
	if (root != "")
		cmd += " -d " + root; 
	cmd += " " + fileName;
	qDebug() << cmd;
	
	QProcess unzip;
	unzip.setReadChannelMode(QProcess::MergedChannels);
	unzip.start(cmd);
	if (!unzip.waitForFinished()) {
		qDebug() << "unzip failed:" << unzip.errorString();
		return false;
	}
	qDebug() << "unzip output:" << unzip.readAll();
	return true;

#else
  QuaZip zip(fileName);
  if(!zip.open(QuaZip::mdUnzip)) {
    qWarning("zip.open(): %d", zip.getZipError());
    return false;
  }
  zip.setFileNameCodec("IBM866");
#ifdef DEBUG
  printf("%d entries\n", zip.getEntriesCount());
  printf("Global comment: %s\n", zip.getComment().toLocal8Bit().constData());
#endif
  QuaZipFileInfo info;

#ifdef DEBUG
  printf("name\tcver\tnver\tflags\tmethod\tctime\tCRC\tcsize\tusize\tdisknum\tIA\tEA\tcomment\textra\n");
#endif
  QuaZipFile file(&zip);
  QString name;
  char c;
  for(bool more=zip.goToFirstFile(); more; more=zip.goToNextFile()) {
    if(!zip.getCurrentFileInfo(&info)) {
      qWarning("install(): getCurrentFileInfo(): %d\n", zip.getZipError());
      return false;
    }
#ifdef _DEBUG
    printf("%s\t%hu\t%hu\t%hu\t%hu\t%s\t%u\t%u\t%u\t%hu\t%hu\t%u\t%s\t%s\n",
        info.name.toLocal8Bit().constData(),
        info.versionCreated, info.versionNeeded, info.flags, info.method,
        info.dateTime.toString(Qt::ISODate).toLocal8Bit().constData(),
        info.crc, info.compressedSize, info.uncompressedSize, info.diskNumberStart,
        info.internalAttr, info.externalAttr,
        info.comment.toLocal8Bit().constData(), info.extra.constData());
#endif
    if(info.name.endsWith("/")) {
    	// don't use directory items, some zip files does not have them included
    	continue;
    }
    if(!file.open(QIODevice::ReadOnly)) {
      qWarning("install(): file.open(): %d", file.getZipError());
      return false;
    }
    name=file.getActualFileName();
    if(file.getZipError()!=UNZ_OK) {
      qWarning("install(): file.getFileName(): %d", file.getZipError());
      return false;
    }
	QFileInfo fi(root+"/"+name);
	QDir a(fi.absolutePath());
	if (!a.mkpath(fi.absolutePath())) {
		qWarning(" can't create directory %d", fi.absolutePath().toAscii().data());
		return false;
	}

	if (verbose)
    	printf("%s %d -> %d\n",info.name.toLocal8Bit().constData(), info.compressedSize, info.uncompressedSize);

	QFile out(fi.absoluteFilePath());
    out.open(QIODevice::WriteOnly);
    QByteArray buf;
    buf.resize(info.uncompressedSize);
    file.read(buf.data(),buf.size());
    out.write(buf.data(),buf.size());
    out.close();
    if(file.getZipError()!=UNZ_OK) {
      qWarning("install(): file.getFileName(): %d", file.getZipError());
      return false;
    }
    if(!file.atEnd()) {
      qWarning("install(): read all but not EOF");
      return false;
    }
    file.close();
    if(file.getZipError()!=UNZ_OK) {
      qWarning("install(): file.close(): %d", file.getZipError());
      return false;
    }
  }
  return true;
  #endif
}



