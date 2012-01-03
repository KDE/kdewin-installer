/****************************************************************************
**
** Copyright (C) 2005-2007 Ralf Habacker. All rights reserved.
**
** This file is part of the KDE installer for windows
**
** This library is free software; you can redistribute it and/or
** modify it under the terms of the GNU Library General Public
** License version 2 as published by the Free Software Foundation.
**
** This library is distributed in the hope that it will be useful,
** but WITHOUT ANY WARRANTY; without even the implied warranty of
** MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU
** Library General Public License for more details.
**
** You should have received a copy of the GNU Library General Public License
** along with this library; see the file COPYING.LIB.  If not, write to
** the Free Software Foundation, Inc., 51 Franklin Street, Fifth Floor,
** Boston, MA 02110-1301, USA.
**
****************************************************************************/

#ifndef PACKAGELIST_H
#define PACKAGELIST_H

#include <QtCore/QObject>
#include <QtCore/QStringList>
#include "package.h"
#include "site.h"
class PackageInfo;

class QTreeWidget;
class QTreeWidgetItem;
class QIODevice;
class Site;
class Database;
class InstallerEngine;

QStringList filterPackageFiles(const QStringList &list,const QString &mode);

class PackageList : public QObject
{
    Q_OBJECT

public:
    enum Type {ApacheModIndex, Default, Ftp, SourceForge, SourceForgeMirror };

    PackageList(InstallerEngine *parent);
    virtual ~PackageList();

    QT_DEPRECATED bool addPackage(const Package &package) { return append(package); }
    /// append package to the end of the list
    bool append(const Package &package);
    /// append package to the end of the list
    bool append(Package &package);
    /// append given packages to the end of the list
    bool append(const PackageList &packageList);

    QT_DEPRECATED Package *getPackage(QString const &pkgName, const QByteArray &version = QByteArray()) { return find(pkgName, version); }
    Package *find(QString const &pkgName, const QByteArray &version = QByteArray());
    Package *find(const PackageInfo &info);

    QStringList listPackages();
    bool readFromFile(const QString &_fileName=QString());
    bool readFromFile(const QString &fileName, PackageList::Type type=PackageList::Default, bool append=false);
    bool readFromByteArray(const QByteArray &_ba, PackageList::Type type=PackageList::Default, bool append=false);
    bool readFromDirectory(const QString &dir, bool append=false);

    bool writeToFile(QString const &fileName=QString::null);
    bool syncWithFile(const QString &_fileName=QString());
    bool syncWithDatabase(Database &database);

    bool setInstalledPackage(const Package &pkg);

    int size() const
    {
        return m_packageList.size();
    }
    const QList <Package*> &packages() const
    {
        return m_packageList;
    }
    void setConfigFileName(const QString &file)
    {
        m_configFile = '/' + file;
    }
    QString name() const
    {
        return m_name;
    }
    void setName(const QString &name)
    {
        m_name = name;
    }
    QString notes() const
    {
        return m_notes;
    }
    void setNotes(const QString &notes)
    {
        m_notes = notes;
    }
    const QUrl &baseURL() const
    {
        return m_baseURL;
    }
    void setBaseURL(const QString &baseURL)
    {
        m_baseURL = baseURL;
    }
    void setBaseURL(const QUrl &baseURL)
    {
        m_baseURL = baseURL;
    }
    // sets the current Site to get access to dep & deny lists
    void setCurrentSite(Site *s)
    {
        m_curSite = s;
    }

    // 0.5.3
    bool hasConfig();

    void clear();

signals:
    void configLoaded();

protected:
    InstallerEngine *m_parent;
    bool addPackagesFromFileNames(const QStringList &files, bool ignoreConfigTxt=false);

    bool readInternal(QIODevice *ioDev, PackageList::Type type, bool append=false);
private:
    QList<Package*> m_packageList;
    QString m_root;
    QString m_configFile;
    QString m_name;
    QUrl m_baseURL;
    Site *m_curSite;
    QString m_notes;
    bool m_parserConfigFileFound; /// indicates the a config file was found for a parsed web site

    friend class Installer;
    friend QDebug & operator<<(QDebug &, PackageList &);
};

QDebug & operator<<(QDebug &, PackageList &);

#endif
