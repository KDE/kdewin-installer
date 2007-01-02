/****************************************************************************
**
** Copyright (C) 2005 Ralf Habacker. All rights reserved.
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

// @TODO load installer config from manifest dir


#include <QtCore>

#include "installer.h"
#include "packagelist.h"

Installer::Installer(PackageList *_packageList)
	: QObject()
{
	packageList = _packageList;
	connect (packageList,SIGNAL(loadedConfig()),this,SLOT(updatePackageList()));
	installedList = new QList<QString>();

//	if (QFile::exists("installed.txt"))
//		readFromFile("installed.txt");
//	else
//		loadConfig();
}

Installer::~Installer()
{
//	writeToFile("installed.txt");
	delete installedList;
}

/*
void Installer::printList(const QString &title) 
{
#ifdef DEBUG
	qDebug() << __PRETTY_FUNCTION__;
#endif
	qDebug() << title;
	QList<QString>::iterator i;
	for (i = installedList->begin(); i != installedList->end(); ++i)
		qWarning(i->toLatin1());
}
*/

/* not required at now
bool Installer::readFromFile(QString const &fileName)
{
#ifdef DEBUG
	qDebug() << __PRETTY_FUNCTION__;
#endif
	QFile file(fileName);
  if (!file.open(QIODevice::ReadOnly| QIODevice::Text))
      return false;

	installedList->clear();

  while (!file.atEnd()) {
	  QByteArray line = file.readLine();
		if (line.startsWith("#"))
			continue;
    installedList->append(line);
	}
	return true;
}

bool Installer::writeToFile(QString const &fileName)
{
#ifdef DEBUG
	qDebug() << __PRETTY_FUNCTION__;
#endif
	QFile file(fileName);
  if (!file.open(QIODevice::WriteOnly | QIODevice::Text))
      return false;

	QTextStream out(&file);
	out << "# installed package list" << "\n";
	QList<QString>::iterator i;
	for (i = installedList->begin(); i != installedList->end(); ++i)
		out << i->toLatin1() << "\n";
	return true;
}
*/

bool Installer::loadConfig(const QString &destdir)
{
#ifdef DEBUG
	qDebug() << __PRETTY_FUNCTION__;
#endif
	installedList->clear();

	QDir dir(destdir + "manifest");
	dir.setFilter(QDir::Files);
	dir.setNameFilters(QStringList("*.ver"));
	dir.setSorting(QDir::Size | QDir::Reversed);
	
	QFileInfoList list = dir.entryInfoList();
	for (int i = 0; i < list.size(); ++i) {
	  QFileInfo fileInfo = list.at(i);
	  Package pkg;
	  pkg.setFromVersionFile(fileInfo.fileName());
	  packageList->updatePackage(pkg);
//    installedList->append(fileInfo.fileName());
	}
//	printList("installed packages");
	return true;
}

void Installer::install(const QString &fileName, const QString &destdir)
{
	QString cmd = "bin\\unzip.exe -o";
	if (destdir != "")
	cmd += " -d " + destdir; 
	cmd += " " + fileName;
	qDebug() << cmd;
	
	QProcess unzip;
  unzip.setReadChannelMode(QProcess::MergedChannels);
	unzip.start(cmd);
	if (!unzip.waitForFinished())
		qDebug() << "unzip failed:" << unzip.errorString();
	else
		qDebug() << "unzip output:" << unzip.readAll();
}

void Installer::updatePackageList()
{
#ifdef DEBUG
	qDebug() << __PRETTY_FUNCTION__;
#endif
	loadConfig();
}



