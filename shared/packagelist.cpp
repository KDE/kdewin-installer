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

#include <QtGui>
#include <QtNetwork>

#include "packagelist.h"
#include "downloader.h"
#include "installer.h"
//#define DEBUG

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

PackageList::PackageList()
	: QObject()
{
#ifdef DEBUG
	qDebug() << __PRETTY_FUNCTION__;
#endif
	downloader = new Downloader;
	packageList = new QList<Package>;
	root = ".";
	configFile = "/packages.txt";
}


PackageList::PackageList(Downloader *_downloader)
	: QObject()
{
#ifdef DEBUG
	qDebug() << __PRETTY_FUNCTION__;
#endif
	downloader = _downloader;
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
		qDebug(i->toString(true).toLatin1());
}

bool PackageList::writeToFile(const QString &_fileName)
{
#ifdef DEBUG
	qDebug() << __PRETTY_FUNCTION__;
#endif
	if (packageList->count() == 0)
		return false;

	QString fileName = _fileName == "" ? root + configFile : _fileName;
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
	QString fileName = _fileName == "" ? root + configFile : _fileName;
	QFile file(fileName);

  if (!file.open(QIODevice::ReadOnly| QIODevice::Text))
      return false;

	packageList->clear();

	Package pkg;
  while (!file.atEnd()) {
	  QByteArray line = file.readLine();
		if (line.startsWith("#"))
			continue;
    int i = line.lastIndexOf("\t");
    pkg.setName(line.mid(0,i));
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


QStandardItemModel *PackageList::getModel()
{
/*
	// TODO: update model size 
	if (!model)
		model = new QStandardItemModel(size(), 6);
	return model;
*/
	return 0;
}

void PackageList::setModelData(QTreeView *tree)
{
	int size = packageList->size();
  QStandardItemModel *model = new QStandardItemModel(size, 8);
	tree->setModel(model);

	QList<Package>::iterator i;
	int row = 0;
	int col = 0;
	model->setHeaderData( col++,Qt::Horizontal,QVariant("Package") );
	model->setHeaderData( col++,Qt::Horizontal,QVariant("Version") );
	model->setHeaderData( col++,Qt::Horizontal,QVariant("all") );
	model->setHeaderData( col++,Qt::Horizontal,QVariant("bin") );
	model->setHeaderData( col++,Qt::Horizontal,QVariant("lib") );
	model->setHeaderData( col++,Qt::Horizontal,QVariant("doc") );
	model->setHeaderData( col++,Qt::Horizontal,QVariant("src") );
	model->setHeaderData( col++,Qt::Horizontal,QVariant("Notes") );
	for (i = packageList->begin(); i != packageList->end(); ++i) {
		int col = 0;
	  model->setData(model->index(row, col++), QVariant(i->Name()) );
	  model->setData(model->index(row, col++), QVariant(i->Version()) );
	  model->setData(model->index(row, col++), QVariant(" ") );
	  model->setData(model->index(row, col++), QVariant(i->isInstalled(Package::BIN) ? " X " : "---") );
	  model->setData(model->index(row, col++), QVariant(i->isInstalled(Package::LIB) ? " X " : "---") );
	  model->setData(model->index(row, col++), QVariant(i->isInstalled(Package::DOC) ? " X " : "---") );
	  model->setData(model->index(row, col++), QVariant(i->isInstalled(Package::SRC) ? " X " : "---") );
	  model->setData(model->index(row, col++), QVariant("") );
	  row++;
	}
}

void PackageList::updateModelData(const QModelIndex &index)
{
	if (index.column() < 2)
		return;
	QString old = index.data().toString();
	QString now;
	if (index.column() == 2) {
		if (old == "  ")
			now = " I ";
		else if (old == " I ")
			now = " R ";
		else if (old == " R ")
			now = "  ";
		model->setData(index, QVariant(now));
	}
/* not allowed at now 
	else {
		if (old == " X ")
			now = " R ";
		else if(old == "---")
			now = " I ";
		else if(old == " R ")
			now = " X ";
		else if(old == " I ")
			now = "---";
		model->setData(index, QVariant(now));
	}	
*/ 
	// TODO: set required action in packageList
}
QStringList PackageList::getPackagesToInstall(QStandardItemModel *model)
{
	int row = 0;
	QStringList packagesToInstall;
	QList<Package>::iterator i;

	for (i = packageList->begin(); i != packageList->end(); ++i) {
	  if (model->data(model->index(row, 2)).toString() == " I ")
	  	packagesToInstall << model->data(model->index(row, 0)).toString();
		row++;
	}
	return packagesToInstall;
}
