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
class Database;

/** 
    The class InstallerEngine contains all relevant informations and actions to 
    download, remove, install or update packages. 
*/ 
class InstallerEngine : public QObject
{
    Q_OBJECT
public:
    InstallerEngine(QObject *parent=0);
    virtual ~InstallerEngine();

    void setConfigURL(const QUrl &url);
    
    QString root() { return m_root; }
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

    QString startMenuRootPath;

    typedef enum { cancel, retry, ignore } ErrorAction;
    void setErrorAction(ErrorAction action) { m_errorAction = action; }
    
    typedef enum { onlineInstall, localInstall, downloadOnly } InstallMode;
    static void setInstallMode(InstallMode mode) { m_installMode = mode; }

    static InstallMode installMode() { return m_installMode; }

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
    static InstallMode  m_installMode;
    ErrorAction         m_errorAction;  // action required after errors
    
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

    bool getStartMenuRootPath();
    
    /**
        in case that dependencies are updated, this method
        can be used to find which additional packages has to be installed
    */
    bool checkInstalledDependencies();

    friend QDebug &operator<<(QDebug &, const InstallerEngine &);
};

#endif
