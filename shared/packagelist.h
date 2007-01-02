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

#include "package.h"

class PackageList : public QObject {
	Q_OBJECT
	
	public:
		PackageList();
		virtual ~PackageList();
		void addPackage(Package const &package); 
		void listPackages(const QString &title="");
		bool readFromFile(QString const &fileName);
		bool readFromHTMLFile(const QString &fileName);
		bool writeToFile(QString const &fileName);
//		static bool downloadPackage(QString const &pkgName);
		QStringList getPackageFiles(QString const &pkgName);
		Package *getPackage(QString const &pkgName);
		QStringList getFilesToInstall(QString const &pkgName);
		QStringList getFilesForDownload(QString const &pkgName);
		bool updatePackage(Package &pkg);

	signals:
		void loadedConfig();

	private: 
		QList<Package> *packageList;
		
};

#endif
