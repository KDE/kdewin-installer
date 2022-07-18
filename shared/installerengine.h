/*
**
** Copyright (C) 2005-2010 Ralf Habacker. All rights reserved.
**
** This file is part of the KDE installer for windows
**
** SPDX-License-Identifier: LGPL-2.0-only
**
****************************************************************************/

#ifndef INSTALLERENGINE_H
#define INSTALLERENGINE_H

#include <QString>
#include <QProcess>

#include "settings.h"
#include "globalconfig.h"
#include "packagecategorycache.h"
#include "packagestates.h"
#include "database.h"
#include "typehelper.h"

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

    /**
      set installation root 
      Calling this method will reload the installed package into the internal database
      @param root installation root 
    */
    void setRoot(const QString &root);
    
    // read in global config 
    bool initGlobalConfig();

    // add packages from global config or sites (requires proxy settings)
    bool initPackages();

    // check installer version 
    bool isInstallerVersionOutdated();

    /// check if any package is installed in the recent installation root
    bool isAnyPackageInstalled();

    /// check if any kde process is running from the installation root
    bool isAnyKDEProcessRunning();

    /// kill all kde processes running from the installation root
    bool killAllKDEApps();


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

    void checkUpdateDependencies(QList<Package*> &deps);

    bool setDependencyState(Package *package, QList<Package*>&deps);

    /// check if a package should be included in the list of displayed packages
    bool includePackage(CompilerTypes::Type compilerType, const QString &name, const QString &categoryName=QString());

    /// check if a category should be included in the list of displayed categories
    bool includeCategory(CompilerTypes::Type compilerType, const QString &categoryName);

    /// return version of kde start menu generator in the form 0x00aabbcc as converted from 'aa.bb.cc'
    int getStartMenuGeneratorVersion();

    /// return version identifed by "key" from executable "appname", see \ref toVersionInt for the output format
    int getAppVersion(const QString &appname, const QString &key=QString());
    QString getAppVersionString(const QString &appname, const QString &key=QString());

    void setCurrentCompiler(CompilerTypes::Type type) { m_currentCompiler = type; }
    CompilerTypes::Type currentCompiler() { return m_currentCompiler; }

    /// return process environment for running tools
    QProcessEnvironment processEnvironment();

    /// run process detached
    bool runProcessDetached(const QString &executable, const QStringList &args = QStringList());

    /// run process
    bool runProcess(QProcess &p, const QString &cmd, const QStringList &args = QStringList(), bool waitFinished = false);

    // check if helper applications could be started (used for running installer on linux)
    bool canRunHelperApplications();

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
    CompilerTypes::Type m_currentCompiler; ///< current compiler
    
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
    
    /// add meta packages collected from site definition
    bool addMetaPackages();

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

/// holds the package selection and icon states
extern PackageStates packageStates;

/// holds the package dependency state
extern PackageStates dependencyStates;

#endif
