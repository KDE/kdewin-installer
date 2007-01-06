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

#ifndef PACKAGELIST_H
#define PACKAGELIST_H

#include <QObject>
#include "package.h"

class Downloader;
class InstallerBase;
class QTreeWidget;
class QTreeWidgetItem;
class QIODevice;

QStringList filterPackageFiles(const QStringList &list,const QString &mode);

class PackageList : public QObject {
	Q_OBJECT
	
	public:
		enum SiteType {SourceForge, ApacheModIndex};
		
		PackageList(Downloader *downloader= NULL);
		virtual ~PackageList();
		void addPackage(Package const &package); 
		void listPackages(const QString &title=QString::null);
		bool readFromFile(const QString &_fileName=QString::null);
		bool readHTMLFromFile(const QString &fileName, SiteType type=SourceForge);
		bool readHTMLFromByteArray(const QByteArray &ba, SiteType type=SourceForge);
		bool writeToFile(QString const &fileName=QString::null);
//		static bool downloadPackage(QString const &pkgName);
		QStringList getPackageFiles(QString const &pkgName);
		Package *getPackage(QString const &pkgName);
		QStringList getFilesForInstall(QString const &pkgName);
		QStringList getFilesForDownload(QString const &pkgName);
		bool updatePackage(Package &pkg);
		int size();

		// 0.5.3
		bool hasConfig();
		bool downloadPackage(const QString &pkgName);
		bool installPackage(const QString &pkgName);

		// Selector Page related methods
		void setWidgetData(QTreeWidget *tree);
		void itemClicked(QTreeWidgetItem *item, int column);
		bool downloadPackages(QTreeWidget *tree, const QString &category="");
		bool installPackages(QTreeWidget *tree, const QString &category="");
		
	signals:
		void loadedConfig();
	
	private:
		bool readHTMLInternal(QIODevice *ioDev, SiteType type);
	private: 
		QList<Package> *packageList;
		QString root; 
		QString configFile;
		Downloader *downloader;
		InstallerBase *installer;

	friend class InstallerBase;		
};

#endif
