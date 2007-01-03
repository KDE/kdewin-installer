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
	QByteArray file = fileName.toLatin1();
	QByteArray rootdir = root.toLatin1();
	
	int ret = unzip(file.data(),rootdir.data());
    return ret;
#endif
}



