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

#ifndef INSTALLERENGINE_H
#define INSTALLERENGINE_H

#include <QString>
#include <QList>

#include "settings.h"
#include "globalconfig.h"
#include "packagecategorycache.h"

class PackageList;
class Downloader;
class Installer;
class GlobalConfig;
class Settings;
class Package;
class InstallWizard;
class QTreeWidget;
class QTreeWidgetItem;
class Database;

class InstallerEngine : public QObject
{
    Q_OBJECT
public:
    InstallerEngine(QObject *parent);
    virtual ~InstallerEngine();

    // read in global config 
    void initGlobalConfig();

    // add packages from global config or sites (requires proxy settings)
    bool initPackages();

    // check installer version 
    bool isInstallerVersionOutdated();

    void stop();

    Package *getPackageByName(const QString &name,const QString &version=QString());

    Installer *installer()
    {
        return m_installer;
    }

    GlobalConfig *globalConfig() { return m_globalConfig; }
    /// download url 
    static QString defaultConfigURL;
    static QString fallBackURL;

Q_SIGNALS:
    void error ( const QString &error );

protected slots:
    //@ TODO using this slots make thing much complicater as necessary 
    void installDirChanged(const QString &newdir);
    void mirrorChanged(const QString &mirror);

protected:
    PackageList*         m_packageResources;
    Installer           *m_installer;    // currently used installer
    GlobalConfig        *m_globalConfig;
    Database            *m_database;
    bool                m_initFinished;
    bool                m_globalConfigReaded;
    bool                m_addedPackages;
    bool                m_canceled;
    QUrl                m_usedDownloadSource;
    /// init all package definitions
    virtual bool init();
    /// reload all package definition
    virtual void reload();

    /**
        fetch configuration from local (file procotol), http or ftp mirrors 
        The precedence is 
    */
    bool readGlobalConfig();

    /// adds packages defined directly in GlobalConfig
    bool addPackagesFromGlobalConfig();

    /// adds package collected from site definition
    bool addPackagesFromSites();

    friend QDebug &operator<<(QDebug &, const InstallerEngine &);
};

#endif
