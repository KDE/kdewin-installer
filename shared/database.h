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
    Database ( QObject *parent = 0 );
    virtual ~Database();
    bool readFromDirectory ( const QString &dirPath=QString() );
    void addPackage ( const Package &package );

    void listPackageFiles ( const QString &pkgName, Package::Type pkgType=Package::BIN );
    void listPackages ( const QString &title=QString() );
    QStringList getPackageFiles ( const QString &pkgName, Package::Type pkgType=Package::BIN );
    bool verifyFiles( const QString &pkgName, Package::Type pkgType=Package::BIN  );
    Package *getPackage ( const QString &pkgName, const QByteArray &version=QByteArray() );
    void clear();

    int size() const {
        return m_database.size();
    }

    const QList <Package*> &packages() const {
        return m_database;
    }

    void dump ( const QString &title=QString() );

    void setRoot ( const QString &root ) {
        m_root = root;
        readFromDirectory();
    }

    QString root() const {
        return m_root;
    }

    static bool isAnyPackageInstalled ( const QString &installRoot );

Q_SIGNALS:
    void configLoaded();

protected:
    void addFromRegistry();

protected Q_SLOTS:
    void slotInstallDirChanged ( const QString & );

private:
    QMap<QString,QString> m_installKeys;
    QMap<QString,QString> m_versionKeys;
    QList<Package*> m_database;
    QString m_root;

    friend QDebug & operator<< ( QDebug &, const Database & );
};

QDebug & operator<< ( QDebug &, const Database & );
#endif
