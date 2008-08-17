/*
**
** Copyright (C) 2005-2008 Ralf Habacker. All rights reserved.
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

#include "settings.h"
#include "globalconfig.h"
#include "packagecategorycache.h"
#include "database.h"

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

    void setConfigURL(const QUrl &url);
    
    void setRoot(const QString &root);
    
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
    Database *database() { return m_database; }
    PackageList* packageResources() { return m_packageResources; }
    QUrl &usedDownloadSource() { return m_usedConfigURL; }

    /// number of installed packages
    int installedPackages() { return m_installedPackages; }
    
    /// number of installed packages
    int removedPackages() { return m_removedPackages; }
    /// number of really downloaded packages
    int downloadedPackages() { return m_downloadedPackages; }

    /// set local install mode which means using a file url 
    static void setLocalInstall(bool mode) { m_localInstall = mode; }

    /// return state of local install mode 
    static bool isLocalInstall() { return m_localInstall; }



Q_SIGNALS:
    void error ( const QString &error );

protected slots:
    //@ TODO using this slots make thing much complicater as necessary 
    //void installDirChanged(const QString &newdir);
    void slotError ( const QString &error );

protected:
    PackageList*         m_packageResources;
    Installer           *m_installer;    // currently used installer
    GlobalConfig        *m_globalConfig;
    Database            *m_database;
    bool                m_initFinished;
    bool                m_globalConfigReaded;
    bool                m_addedPackages;
    bool                m_canceled;
    QString             m_root;
    QUrl                m_configURL;
    QUrl                m_usedConfigURL;
    int                 m_installedPackages;
    int                 m_downloadedPackages;
    int                 m_removedPackages;
    static bool         m_localInstall;
    
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

    /// add packages collected from site definition
    bool addPackagesFromSites();
    
    /// add installed packages for which no package is available
    bool addInstalledPackages();

    friend QDebug &operator<<(QDebug &, const InstallerEngine &);
};

#endif
