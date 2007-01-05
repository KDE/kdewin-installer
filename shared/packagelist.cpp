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

#include <QBuffer>
#include <QDebug>
#include <QFile>
#include <QFileInfo>
#include <QStandardItemModel>
#include <QUrl>
#include <QTreeWidget>

#include "packagelist.h"
#include "downloader.h"
#include "installer.h"

#ifdef Q_CC_MSVC
# define __PRETTY_FUNCTION__ __FUNCTION__
#endif

QStringList filterPackageFiles(const QStringList &list,const QString &mode)
{
    QStringList result; 
    for (int j = 0; j < list.size(); ++j) {
        QUrl url(list.at(j));
        QFileInfo fileInfo(url.path());
        QString fileName = fileInfo.fileName();

        // only download package not already downloaded and only bin and lib packages
	        if (mode == "URL" && QFile::exists(fileName))
            qDebug() << fileName << " - already downloaded";
        //		else if(fileName.contains("src") ) 
        //	    qDebug() << fileName << " - ignored";
        else {
        if (mode == "URL")
            qDebug() << fileName << " - downloading";
            else
            qDebug() << fileName << " - installing";
        if (mode == "URL")
            result << list.at(j);
        else
            result << fileName;
        }
    }
    return result;
}

PackageList::PackageList(Downloader *_downloader)
	: QObject()
{
#ifdef DEBUG
	qDebug() << __PRETTY_FUNCTION__;
#endif
    downloader = _downloader ? _downloader : new Downloader;
	packageList = new QList<Package>;
	root = ".";
	configFile = "/packages.txt";
	
}
		
PackageList::~PackageList()
{
#ifdef DEBUG
	qDebug() << __PRETTY_FUNCTION__;
#endif
	delete packageList;
}

bool PackageList::hasConfig()
{
	return QFile::exists(root + configFile);
}

void PackageList::addPackage(Package const &package)
{
#ifdef DEBUG
	qDebug() << __PRETTY_FUNCTION__;
#endif
	packageList->append(package);
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

void PackageList::listPackages(const QString &title) 
{
#ifdef DEBUG
	qDebug() << __PRETTY_FUNCTION__;
#endif
	qDebug() << title;
	QList<Package>::iterator i;
	for (i = packageList->begin(); i != packageList->end(); ++i)
		qDebug(i->toString(true," - ").toLatin1());
}

bool PackageList::writeToFile(const QString &_fileName)
{
#ifdef DEBUG
	qDebug() << __PRETTY_FUNCTION__;
#endif
	if (packageList->count() == 0)
		return false;

	QString fileName = _fileName.isEmpty() ? root + configFile : _fileName;
	QFile file(fileName);
  if (!file.open(QIODevice::WriteOnly | QIODevice::Text))
      return false;

	QTextStream out(&file);
	out << "# package list" << "\n";
	QList<Package>::iterator i;
	for (i = packageList->begin(); i != packageList->end(); ++i)
		out << i->Name() << "\t" << i->Version() << "\n";
	return true;
}

bool PackageList::readFromFile(const QString &_fileName)
{
#ifdef DEBUG
	qDebug() << __PRETTY_FUNCTION__;
#endif
	QString fileName = _fileName.isEmpty() ? root + configFile : _fileName;
	QFile file(fileName);

  if (!file.open(QIODevice::ReadOnly| QIODevice::Text))
      return false;

  packageList->clear();

	Package pkg;
  while (!file.atEnd()) {
	  QByteArray line = file.readLine();
		if (line.startsWith("#"))
			continue;
    int i = line.lastIndexOf('\t');
    pkg.setName(line.mid(0,i));
    pkg.setVersion(line.mid(i+1,line.size()-i-2));
		addPackage(pkg);
	}
	emit loadedConfig();
	return true;
}

bool PackageList::readHTMLInternal(QIODevice *ioDev, SiteType type)
{
  packageList->clear();
	
	Package pkg; 

	switch (type) {
		case SourceForge: 
			while (!ioDev->atEnd()) {
				QByteArray line = ioDev->readLine();
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
			break; 

		case ApacheModIndex: 
			char *lineKey = "alt=\"[   ]\"> <a href=\"";
			char *fileKeyStart = "<a href=\"";
			char *fileKeyEnd = "\">";
			while (!ioDev->atEnd()) {
				QByteArray line = ioDev->readLine();
#ifdef DEBUG
				qDebug() << "2"  << line << " " << lineKey; 
#endif
				if (line.contains(lineKey)) {
					int a = line.indexOf(fileKeyStart) + strlen(fileKeyStart);
					int b = line.indexOf(fileKeyEnd,a);
					QByteArray name = line.mid(a,b-a);
#ifdef DEBUG
					qDebug() << "3"  << name;
#endif
					// desktop-translations-10.1-41.3.noarch.rpm
					// kde3-i18n-vi-3.5.5-67.9.noarch.rpm
					QList<QByteArray> parts = name.split('-');
#ifdef DEBUG
     				qDebug() << parts.size();
					for (int i = 0; i < parts.size(); ++i) 
     					qDebug() << parts.at(i);
#endif
					QList<QByteArray> patchlevel = parts.at(parts.size()-1).split('.');
					QByteArray version = parts.at(parts.size()-2) + "-" + patchlevel.at(0) + "." + patchlevel.at(1);
					pkg.setVersion(version);
					if (parts.size() == 4) 
						pkg.setName(parts.at(0) + '-' + parts.at(1));
					else if (parts.size() == 5) 
						pkg.setName(parts.at(0) + '-' + parts.at(1) + '-' + parts.at(2));
					addPackage(pkg);
				}
			}
			break; 
	}
	emit loadedConfig();
	return true;
}

bool PackageList::readHTMLFromByteArray(const QByteArray &_ba, SiteType type)
{
#ifdef DEBUG
	qDebug() << __PRETTY_FUNCTION__;
#endif

  QByteArray ba(_ba);
  QBuffer buf(&ba);

  if (!buf.open(QIODevice::ReadOnly| QIODevice::Text))
      return false;

  return readHTMLInternal(&buf, type);
}

bool PackageList::readHTMLFromFile(const QString &fileName, SiteType type )
{
#ifdef DEBUG
	qDebug() << __PRETTY_FUNCTION__;
#endif
	QFile pkglist(fileName);
	if (!pkglist.exists())
  	return false;

	pkglist.open(QIODevice::ReadOnly);

  return readHTMLInternal(&pkglist, type);
}

QStringList PackageList::getFilesForInstall(QString const &pkgName)
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
#ifdef INCLUDE_DOC_AND_SRC_PACKAGES
	result << pkg->getFileName(Package::DOC);
	result << pkg->getFileName(Package::SRC);
#else
	qDebug("downloading of DOC and SRC disabled for now");
#endif
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
#ifdef INCLUDE_DOC_AND_SRC_PACKAGES
	result << pkg->getURL(Package::DOC);
	result << pkg->getURL(Package::SRC);
#else
	qDebug("downloading of DOC and SRC disabled for now");
#endif
	return result;
}

bool PackageList::updatePackage(Package &apkg)
{
	Package *pkg = getPackage(apkg.Name());
	if (!pkg) {
		qDebug() << __FUNCTION__ << "package " << apkg.Name() << " not found";
		return false;
	}
	pkg->addInstalledTypes(apkg);
	return true;
}

bool PackageList::downloadPackage(const QString &pkgName)
{
	QStringList files = getFilesForDownload(pkgName);
	files = filterPackageFiles(files,"URL");
	bool ret = true;
	for (int j = 0; j < files.size(); ++j) {
		if (!downloader->start(files.at(j)))
			ret = false;
	}
	return true;
}

bool PackageList::installPackage(const QString &pkgName)
{
	QStringList files = getFilesForInstall(pkgName);
	files = filterPackageFiles(files,"PATH");
	bool ret = true;
	for (int j = 0; j < files.size(); ++j) {
		if (!installer->install(files.at(j)))
			ret = false;
	}
	return true;
}

int PackageList::size()
{
#ifdef DEBUG
	qDebug() << __PRETTY_FUNCTION__;
#endif
	return packageList->size();
}

void PackageList::setWidgetData(QTreeWidget *tree)
{
 	tree->setColumnCount(8);
 	QStringList labels;
 	labels 
 	<< "Package"
	<< "Version"
	<< "all"
	<< "bin"
	<< "lib"
	<< "src"
	<< "doc"
	<< "Notes";

 	tree->setHeaderLabels(labels);
 	QList<QTreeWidgetItem *> items;
	QList<Package>::iterator i;
	for (i = packageList->begin(); i != packageList->end(); ++i) {
		QStringList data; 
		data << i->Name()
			 << i->Version()
			 << ""
			 << (i->isInstalled(Package::BIN) ? "x" : "")
			 << (i->isInstalled(Package::LIB) ? "x" : "")
			 << (i->isInstalled(Package::SRC) ? "x" : "")
			 << (i->isInstalled(Package::DOC) ? "x" : "")
			;			 
		QTreeWidgetItem *a = new QTreeWidgetItem((QTreeWidget*)0, data);
/*		if (i->isInstalled(Package::BIN))
			a->setCheckState(2,Qt::Checked);
		if (i->isInstalled(Package::LIB))
			a->setCheckState(3,Qt::Checked);
		if (i->isInstalled(Package::SRC))
			a->setCheckState(4,Qt::Checked);
		if (i->isInstalled(Package::DOC))
			a->setCheckState(5,Qt::Checked);
*/
		items.append(a);
  }
 	tree->insertTopLevelItems(0, items);
}

void PackageList::itemClicked(QTreeWidgetItem *item, int column)
{
	if (column < 2)
		return;
	item->setCheckState(column,item->checkState(column) == Qt::Checked ? Qt::Unchecked : Qt::Checked);
	if (column == 2) {
		item->setCheckState(3,item->checkState(column));
		item->setCheckState(4,item->checkState(column));
		item->setCheckState(5,item->checkState(column));
		item->setCheckState(6,item->checkState(column));
	}
}   

void PackageList::installPackages(QTreeWidget *tree)
{
	for (int i = 0; i < tree->topLevelItemCount(); i++) {
		QTreeWidgetItem *item = tree->topLevelItem(i);
		qDebug("%s %s %d",item->text(0).toAscii().data(),item->text(1).toAscii().data(),item->checkState(2));
	}
}



#include "packagelist.moc"
