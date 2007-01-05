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

#include "installer.h"
#include "packagelist.h"
#include "installerprogress.h"
#include "quazip.h"
#include "quazipfile.h"

//#define DEBUG
#ifdef Q_CC_MSVC
# define __PRETTY_FUNCTION__ __FUNCTION__
#endif

InstallerBase::InstallerBase(PackageList *_packageList, InstallerProgress *_progress)
	: QObject(), m_progress(_progress)
{
	root = ".";
	packageList = _packageList;
  if(packageList) {
	  packageList->installer = this;
	  packageList->root = root;
  }
	connect (packageList,SIGNAL(loadedConfig()),this,SLOT(updatePackageList()));
}

InstallerBase::~InstallerBase()
{
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

void InstallerBase::updatePackageList()
{
#ifdef DEBUG
	qDebug() << __PRETTY_FUNCTION__;
#endif
	loadConfig();
}

#ifndef QUNZIP_BUFFER
# define QUNZIP_BUFFER (256 * 1024)
#endif
bool InstallerBase::unzipFile(const QString &destpath, const QString &zipFile)
{
  QDir path(destpath);
  QuaZip z(zipFile);

  if(!z.open(QuaZip::mdUnzip)) {
    setError("Can not open %s", zipFile.toAscii().data());
    return false;
  }

  if(!path.exists()) {
    setError("Internal Error - Path %s does not exist", path.absolutePath().toAscii().data());
    return false;
  }

  // z.setFileNameCodec("Windows-1252"); // important!

  QuaZipFile file(&z);
  QuaZipFileInfo info;
  if (m_progress) {
  	m_progress->setMaximum(z.getEntriesCount());
  	m_progress->show();
  }

  for(bool bOk = z.goToFirstFile(); bOk; bOk = z.goToNextFile()) {
    // get file informations
    if(!z.getCurrentFileInfo(&info)) {
      setError("Can not get file information from zip file ", zipFile.toAscii());
      return false;
    }
    QFileInfo fi(path.filePath(info.name));

    // is it's a subdir ?
	if(info.compressedSize == 0 && info.uncompressedSize == 0) {
      if(fi.exists()) {
        if(!fi.isDir()) {
          setError("Can not create directory %s", fi.absoluteFilePath().toAscii().data());
          return false;
        }
        continue;
      }
      if(!path.mkpath(fi.absoluteFilePath())) {
        setError("Can not create directory %s", fi.absolutePath().toAscii().data());
        return false;
      }
      continue;
    }
    // some archives does not have directory entries 
  else {
    if(!path.exists(fi.absolutePath())) {
      setError("create directory %s", fi.absolutePath().toAscii().data());
      if (!path.mkpath(fi.absolutePath())) {
        setError("Can not create directory %s", fi.absolutePath().toAscii().data());
        return false;
      }
    }
  }
    // open file
    if(!file.open(QIODevice::ReadOnly)) {
      setError("Can not open file %s from zip file %s", info.name.toAscii().data(), zipFile.toAscii().data());
      return false;
    }
    if(file.getZipError() != UNZ_OK) {
      setError("Error reading zip file %s", zipFile.toAscii().data());
      return false;
    }

    // create new file
    QFile newFile(fi.absoluteFilePath());
    if(!newFile.open(QIODevice::WriteOnly)) {
      setError("Can not creating file %s ", fi.absoluteFilePath().toAscii().data());
      return false;
    }

  	if (m_progress) {
		m_progress->setTitle(tr("Installing %1 ").arg(newFile.fileName().toAscii().data()));
	}
    // copy data
    // FIXME: check for not that huge filesize ?
    qint64 iBytesRead;
    QByteArray ba;
    ba.resize(QUNZIP_BUFFER);

    while((iBytesRead = file.read(ba.data(), QUNZIP_BUFFER)) > 0)
      newFile.write(ba.data(), iBytesRead);

    file.close();
    newFile.close();

    if(file.getZipError() != UNZ_OK) {
      setError("Error reading zip file %s", zipFile.toAscii().data());
      return false;
    }
  }
  z.close();
  if(z.getZipError() != UNZ_OK) {
    setError("Error reading zip file %s", zipFile.toAscii().data());
    return false;
  }
  return true;
}


void InstallerBase::setError(QByteArray format, QByteArray p1, QByteArray p2)
{
	qDebug(format.data(),p1.data(),p2.data());
}
 

// InstallerGNUWin32
InstallerGNUWin32::InstallerGNUWin32(PackageList *packageList) : InstallerBase(packageList)
{
}

InstallerGNUWin32::~InstallerGNUWin32()
{
}

bool InstallerGNUWin32::isEnabled() 
{
	return true;
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

bool InstallerGNUWin32::install(const QString &fileName)
{
	return unzipFile(root, fileName);
}

#include "installer.moc"
