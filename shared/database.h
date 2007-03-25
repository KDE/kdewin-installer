/****************************************************************************
**
** Copyright (C) 2007 Ralf Habacker. All rights reserved.
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
** This file is proviwded AS IS with NO WARRANTY OF ANY KIND, INCLUDING THE
** WARRANTY OF DESIGN, MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE.
**
****************************************************************************/

#ifndef DATABASE_H
#define DATABASE_H

#include <QObject>
#include "package.h"
class PackageList; 
/** 
 holds local package database
 the local package database is build by package version files 
 located in <installation-root>/manifest directory 
 package version file names are build up by <package-name>-<version>-<packagetype>.ver
*/
class Database: public QObject
{
    Q_OBJECT

public:
    Database();
    virtual ~Database();
    bool readFromDirectory(const QString &_fileName=QString());
    void addPackage(const Package &package);
    void listPackages(const QString &title=QString());
    Package *getPackage(const QString &pkgName, const QByteArray &version=QByteArray());
	bool addUnhandledPackages(PackageList *packageList);

	// returns version file name of package item e.g. xyz-1.2.3-bin.ver
    QString versionFileName(const QString &pkgName, const QString &version, Package::Type type);

    // returns manifest file name of package item e.g. xyz-1.2.3-bin.mft
    QString manifestFileName(const QString &pkgName, const QString &version, Package::Type type);

    int size() const
    {
        return m_database.size();
    }
    const QList <Package*> &database() const
    {
        return m_database;
    }
    void dump(const QString &title=QString());

	static Database &getInstance();
 
signals:
    void configLoaded();

private:
    QList<Package*> m_database;
};

#endif
