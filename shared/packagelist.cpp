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

#include <QtGui>
#include <QtNetwork>

#include "packagelist.h"
//#define DEBUG

PackageList::PackageList()
	: QObject()
{
#ifdef DEBUG
	qDebug() << __PRETTY_FUNCTION__;
#endif
	packageList = new QList<Package>;
}
		
PackageList::~PackageList()
{
#ifdef DEBUG
	qDebug() << __PRETTY_FUNCTION__;
#endif
	delete packageList;
}

void PackageList::addPackage(Package const &package)
{
#ifdef DEBUG
	qDebug() << __PRETTY_FUNCTION__;
#endif
	packageList->append(package);
}

void PackageList::listPackages(const QString &title) 
{
#ifdef DEBUG
	qDebug() << __PRETTY_FUNCTION__;
#endif
	qDebug() << title;
	QList<Package>::iterator i;
	for (i = packageList->begin(); i != packageList->end(); ++i)
		qWarning(i->toString().toLatin1());
}

bool PackageList::writeToFile(QString const &fileName)
{
#ifdef DEBUG
	qDebug() << __PRETTY_FUNCTION__;
#endif
	QFile file(fileName);
  if (!file.open(QIODevice::WriteOnly | QIODevice::Text))
      return false;

	QTextStream out(&file);
	out << "# package list" << "\n";
	QList<Package>::iterator i;
	for (i = packageList->begin(); i != packageList->end(); ++i)
		out << i->toString().toLatin1() << "\n";
	return true;
}

bool PackageList::readFromFile(QString const &fileName)
{
#ifdef DEBUG
	qDebug() << __PRETTY_FUNCTION__;
#endif
	QFile file(fileName);
  if (!file.open(QIODevice::ReadOnly| QIODevice::Text))
      return false;

	packageList->clear();

	Package pkg;
  while (!file.atEnd()) {
	  QByteArray line = file.readLine();
		if (line.startsWith("#"))
			continue;
    int i = line.indexOf("-");
    pkg.setName(line.left(i));
    pkg.setVersion(line.mid(i+1,line.size()-i-2));
		addPackage(pkg);
	}
	emit loadedConfig();
	return true;
}

bool PackageList::readFromHTMLFile(const QString &fileName)
{
#ifdef DEBUG
	qDebug() << __PRETTY_FUNCTION__;
#endif
	QFile pkglist(fileName);
	if (!pkglist.exists())
  	return false;

	packageList->clear();
	
	pkglist.open(QIODevice::ReadOnly);
	Package pkg; 

  while (!pkglist.atEnd()) {
	  QByteArray line = pkglist.readLine();
		if (line.contains("<td><a href=\"/project/showfiles.php?group_id=23617")) {
		int a = line.indexOf("\">") + 2;
		int b = line.indexOf("</a>");
		QByteArray value = line.mid(a,b-a);
		if (line.indexOf("release_id") > -1) {
			pkg.setVersion(value);
			addPackage(pkg);
		}
		else
			pkg.setName(value);
		}
	}
	emit loadedConfig();
	return true;
}

Package *PackageList::getPackage(QString const &pkgName)
{
#ifdef DEBUG
	qDebug() << __PRETTY_FUNCTION__;
#endif
	QList<Package>::iterator i;
	for (i = packageList->begin(); i != packageList->end(); ++i)
		if (i->Name() == pkgName) 
			return &*i;
	return 0;
}

QStringList PackageList::getFilesToInstall(QString const &pkgName)
{
#ifdef DEBUG
	qDebug() << __PRETTY_FUNCTION__;
#endif
	QStringList result;
	Package *pkg = getPackage(pkgName);
	if (!pkg) 
		return result;
	result << pkg->getFileName(Package::BIN);
	result << pkg->getFileName(Package::LIB);
	result << pkg->getFileName(Package::DOC);
	result << pkg->getFileName(Package::SRC);
	return result;
}

QStringList PackageList::getFilesForDownload(QString const &pkgName)
{
#ifdef DEBUG
	qDebug() << __PRETTY_FUNCTION__;
#endif
	QStringList result;
	Package *pkg = getPackage(pkgName);
	if (!pkg)
		return result;
	result << pkg->getURL(Package::BIN);
	result << pkg->getURL(Package::LIB);
	result << pkg->getURL(Package::DOC);
	result << pkg->getURL(Package::SRC);
	return result;
}

bool PackageList::updatePackage(Package &apkg)
{
	Package *pkg = getPackage(apkg.Name());
	if (!pkg) {
		qDebug() << __PRETTY_FUNCTION__ << "package " << apkg.Name() << " not found";
		return false;
	}
	pkg->addInstalledTypes(apkg);
	return true;
}
