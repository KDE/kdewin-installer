/****************************************************************************
**
** Copyright (C) 2005-2010 Ralf Habacker. All rights reserved.
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

#include "config.h"
#include "installerengine.h"
#include "downloader.h"
#include "installer.h"
#include "installerprogress.h"
#include "package.h"
#include "packagelist.h"
#include "packageinfo.h"
#include "packagestates.h"
#include "globalconfig.h"
#include "database.h"

#include <QtDebug>
#include <QDir>
#include <QFlags>
#include <QRegExp>
#include <QCoreApplication>

QString InstallerEngine::defaultConfigURL;
QString InstallerEngine::fallBackURL = "http://downloads.sourceforge.net/kde-windows";
InstallerEngine::InstallMode InstallerEngine::m_installMode = InstallerEngine::onlineInstall;

/// holds the package selection and icon states
PackageStates packageStates;

/// holds the package dependency state
PackageStates dependencyStates;

InstallerEngine::InstallerEngine(QObject *parent)
    : QObject(parent),
      m_initFinished(false),
      m_globalConfigReaded(false),
      m_addedPackages(false),
      m_installedPackages(0),
      m_downloadedPackages(0),
      m_removedPackages(0),
      m_canceled(false),
      m_errorAction(InstallerEngine::ignore),
      m_currentCompiler(CompilerTypes::Unspecified)
{
    m_database = new Database(this);
    m_installer = new Installer(this);
    m_installer->setDatabase(m_database);
    m_globalConfig = new GlobalConfig();
    m_packageResources = new PackageList(this);

    //connect(&Settings::instance(),SIGNAL(installDirChanged(const QString&)),this,SLOT(installDirChanged(const QString&)));
    connect(m_installer,SIGNAL(error(const QString &)),this,SLOT(slotError(const QString &)));

}

InstallerEngine::~InstallerEngine()
{
    delete m_database;
    delete m_installer;
    delete m_globalConfig;
    delete m_packageResources;
}

void InstallerEngine::setRoot(const QString &root)
{
    m_root = root;
    m_installer->setRoot(root);
    m_database->setRoot(root);
}

bool InstallerEngine::initGlobalConfig()
{
    if (m_globalConfigReaded)
        m_globalConfig->clear();
    if (!readGlobalConfig())
        return false;
    QHash<QString, QString>::const_iterator i = m_globalConfig->categoryNotes().constBegin();
    for (; i != m_globalConfig->categoryNotes().constEnd(); i++)
        categoryCache.setNote(i.key(),i.value());

    m_globalConfigReaded = true;
    return true;
}

bool InstallerEngine::initPackages()
{
    if (m_addedPackages)
        m_packageResources->clear();
    if (!addPackagesFromGlobalConfig())
        return false;
    if (!addPackagesFromSites())
        return false;

    addInstalledPackages();
    
    addMetaPackages();

    // add site independend package category relations
    QHash<QString, QStringList>::const_iterator i = m_globalConfig->categoryPackages().constBegin();
    for (; i != m_globalConfig->categoryPackages().constEnd(); i++)
    {
        Q_FOREACH (const QString &name,i.value())
        {
            categoryCache.addPackage(i.key(), name);
            // is this really required ? 
            Package *pkg = m_packageResources->find(name);
            if (pkg)
                pkg->addCategories(i.key());
        }
    }
    
    Q_FOREACH (const QString &metaPackage, m_globalConfig->metaPackages().keys())
    {
        QString category = m_globalConfig->packageCategory(metaPackage + "-%1");
        categoryCache.addPackage(category, metaPackage);
    }
    m_addedPackages = true;

    // retrieve start menu path of recent kde installation
    getStartMenuRootPath();

    m_initFinished = true;
    return true;
}

bool InstallerEngine::init()
{
    if (m_root.isEmpty())
    {
        qCritical() << "no install root set";
        return false;
    }
    if (!initGlobalConfig())
        return false;
    return initPackages();
}

static int versionToInt(QByteArray version)
{
    const QList<QByteArray> v = version.replace('-','.').split('.');
    int n = 0;
    Q_FOREACH(const QByteArray &a,v)
    {
        bool ok;
        int b = a.toInt(&ok);
        if (ok)
        {
            n += b;
            n *= 100;
        }
    }
    return n;
}

bool InstallerEngine::isInstallerVersionOutdated()
{
    int minVersion = versionToInt(m_globalConfig->minimalInstallerVersion());
    int currentVersion = versionToInt(VERSION);
    return minVersion != 0 && currentVersion < minVersion;
}

bool InstallerEngine::isAnyPackageInstalled()
{
    return m_database->isAnyPackageInstalled();
}


bool InstallerEngine::isAnyKDEProcessRunning()
{
    QString cmd = m_root +"/bin/kdeinit4.exe";
    QProcess p;
    QStringList args = QStringList() << "--list";
    p.start(cmd,args);
    if (!p.waitForStarted()) 
    {
        qCritical() << "could not start" << cmd << args;
        return false;
    }
    if (!p.waitForFinished())
    {
        qCritical() << "failed to run" << cmd << args;
        return false;
    }
    QByteArray _stderr = p.readAllStandardError();
    qDebug() << "run" << cmd << args << "without errors" << _stderr; 
    QStringList lines = QString(_stderr).split('\n');
    bool processRunning = false;
    foreach(const QString &line, lines)
    {
        qDebug() << "got line" << line;
        if (line.contains(QCoreApplication::applicationName()))
            continue;
        else if(line.contains("kdeinit4.exe"))
            continue;
        else if (line == "\n")
            continue;
        else
            processRunning = true;
    }
    qDebug() << "returning running processes" << processRunning;
    return processRunning;
}

bool InstallerEngine::killAllKDEApps()
{
    QString cmd = m_root +"/bin/kdeinit4.exe";
    QStringList args = QStringList() << "--help";
    QProcess p;
    p.start(cmd, args);
    if (!p.waitForStarted()) 
    {
        qCritical() << "could not start" << cmd << args;
        return false;
    }
    if (!p.waitForFinished())
    {
        qCritical() << "failed to run" << cmd << args;
        return false;
    }
    QByteArray _stdout = p.readAllStandardOutput();
    args = QStringList() << "--terminate";

    /// I got cases where files are not removed and resulted into "a could not remove file error on installing" 
    p.start(cmd,args);
    if (!p.waitForStarted()) 
    {
        qCritical() << "could not start" << cmd << args;
        return false;
    }
    if (!p.waitForFinished())
    {
        qCritical() << "failed to run" << cmd << args;
        return false;
    }
    qDebug() << "run" << cmd << args << "without errors"; 
    // give applications some time to really be terminated
    qsleep(1000);
    return true;
}

void InstallerEngine::setConfigURL(const QUrl &url)
{
    m_configURL  = url;
}
 
void InstallerEngine::reload()
{
    m_database->reload();
    categoryCache.clear();
    init();
}

bool InstallerEngine::readGlobalConfig()
{
    QString hostURL;
    QStringList configFiles;

    if (!defaultConfigURL.isEmpty()) // command line overrides
    {
        configFiles = m_globalConfig->fetch(defaultConfigURL);
        m_usedConfigURL = defaultConfigURL;
    }
    if (configFiles.isEmpty())
    {
        hostURL = m_configURL.toString();
        if (!hostURL.isEmpty())
        {
            configFiles = m_globalConfig->fetch(hostURL);
            m_usedConfigURL = hostURL;
        }
    }
    if (configFiles.isEmpty())
    {
        if (hostURL != fallBackURL) // fallBack URL is other url
        {
            configFiles = m_globalConfig->fetch(fallBackURL);
            m_usedConfigURL = fallBackURL;
        }
    }
    if (configFiles.isEmpty())
    {
        qCritical() << "no download url found, you have to run setConfigUrl() to specify one";
        return false;
    }

    if (!m_globalConfig->parse(configFiles))
        return false;
    qDebug() << "globalconfig has timestamp" << m_globalConfig->timeStamp().toString(Qt::SystemLocaleDate);
    return true;
}

bool InstallerEngine::addPackagesFromGlobalConfig()
{
    Q_FOREACH ( Package *pkg, *m_globalConfig->packages() )
    {
        pkg->addCategories("all");
        if (pkg->name().contains("mingw"))
            pkg->addCategories("mingw");
        if (pkg->name().contains("msvc"))
            pkg->addCategories("msvc");

        m_packageResources->append(*pkg);
        categoryCache.addPackage(pkg);
    }
    if (Settings::hasDebug("InstallerEngine"))
        qDebug() << __FUNCTION__ << m_packageResources;
    return true;
}

/// download all packagelists, which are available on the configured sites
bool InstallerEngine::addPackagesFromSites()
{
    Q_FOREACH ( Site *site, *m_globalConfig->sites() )
    {
        PackageList packageList(this);
        QString category = site->name();
        qDebug() << "download package file list for site: " << category << "from" << site->url();

        categoryCache.setNote(category,site->notes());
        packageList.setNotes(site->notes());

        // packagelist needs to access Site::getDependencies() && Site::isExclude()
        packageList.setCurrentSite(site);

        packageList.setBaseURL(site->url());

        const QUrl listURL = site->listURL().isEmpty() ? site->url() : site->listURL();

        QByteArray ba;
        qDebug() << listURL;
        if (listURL.scheme() == "file") 
        {
            if (!packageList.readFromDirectory(listURL.toLocalFile(),true))
            {
                emit error("error reading package list from directory");
                continue;
            }
        }
        else {
            if (!Downloader::instance()->fetch(listURL, ba))
            {
                emit error(tr("failed to download site list page from %1").arg(listURL.toString()));
                return false;
            }
            PackageList::Type type;

            if (listURL.scheme() == "ftp") {
                type = PackageList::Ftp; 
            } else { 
                switch(site->Type()) {
                case Site::SourceForge:    type = PackageList::SourceForge; break;
                case Site::SourceForgeMirror: type = PackageList::SourceForgeMirror; break;
                case Site::ApacheModIndex: type = PackageList::ApacheModIndex; break;
                case Site::Ftp: type = PackageList::Ftp; break;
                default:
                    emit error("unknown Site type "  + site->Type());
                    type = PackageList::ApacheModIndex;
                    qDebug() << "error parsing file list" << ba;
                    break;
                }
            }

            if (!packageList.readFromByteArray(ba, type, true ))
            {
                emit error("error reading package list from download html file");
                continue;
            }
        }

        if (Settings::hasDebug("InstallerEngine"))
            qDebug() << __FUNCTION__ << packageList;

        Hash::Type hashType;
        if (site->hashType().type() != Hash::None)
            hashType = site->hashType().type();
        else if (m_globalConfig->hashType().type() != Hash::None)
            hashType = m_globalConfig->hashType().type();
        else
            hashType = Hash::MD5;

        Q_FOREACH(Package *pkg, packageList.packages())
        {
            // add some generic categories
            pkg->addCategories("all");
            pkg->addCategories(category);

            // add categories defined in the config
            pkg->addCategories(site->packageCategories(pkg->name()));

            // if the package hash type is not already set because a hash file was found,
            //  apply site wite hash type settings to the package
            if (pkg->hashType().type() == Hash::None)
                pkg->hashType().setType(hashType);
            
            m_packageResources->append(*pkg);
            categoryCache.addPackage(pkg);
        }
    }
    return true;
}

/// download all packagelists, which are available on the configured sites
bool InstallerEngine::addInstalledPackages()
{
    foreach(Package *pkg, m_database->packages()) 
    {
        if (!m_packageResources->find(pkg->name(),pkg->version().toString().toLatin1()))
            m_packageResources->append(*pkg);
    }
    return true;
}

bool InstallerEngine::addMetaPackages()
{
    Q_FOREACH(const QString& metaPackage, m_globalConfig->metaPackages().keys())
    {
        QList<Package*> pkgList;
        Q_FOREACH(QString compiler, supportedCompilers.values())
        {
            Package *_p = m_packageResources->find(metaPackage + "-" + compiler);
            if(_p)
                pkgList.append(_p);
        }

        // only add metaPackage if there is no package existing
        if(pkgList.isEmpty()) {
            Package* _p = new Package;
            _p->setName(metaPackage);
            Q_FOREACH(Site *site, *m_globalConfig->sites())
            {
                if(site && !site->packageNote(metaPackage + "-%1").isEmpty()) {
                    _p->setNotes(site->packageNote(metaPackage + "-%1"));
                }
            }
            pkgList.append(_p);
        }

        // now go through all existing packages: these must be marked as metaPackage
        Q_FOREACH(Package* p, pkgList)
        {
            Package::PackageItem item("meta");
            p->add(item);
            p->addCategories("all");

            m_packageResources->append(*p);

            // add package to the category cache, if it existed before, then the cache must be updated
            categoryCache.addPackage(p);
        }
    }
    return true;
}

Package *InstallerEngine::getPackageByName(const QString &name,const QString &version)
{
    return m_packageResources->find(name,version.toAscii());
}

void InstallerEngine::stop()
{
    Downloader::instance()->cancel();
    m_canceled = true;
}

bool InstallerEngine::getStartMenuRootPath()
{
    QProcess myProcess;
    myProcess.start(m_root + "\\bin\\kwinstartmenu", QStringList() << "--query-path");
    myProcess.waitForFinished(3000);
    QByteArray data = myProcess.readAllStandardOutput();
    if (data.size() > 0) 
    {
        startMenuRootPath = data;
        return true;
    }
    return false;
}

int InstallerEngine::getAppVersion(const QString &appname, const QString &key)
{
    return toVersionInt(getAppVersionString(appname, key));
}

QString InstallerEngine::getAppVersionString(const QString &appname, const QString &key)
{
    QProcess myProcess;
    myProcess.start(m_root + "\\bin\\" + appname, QStringList() << "--version");
    myProcess.waitForFinished(3000);
    QByteArray data = myProcess.readAllStandardOutput();
    if (data.size() > 0) 
    {
        QRegExp rx(".*"+(!key.isEmpty() ? key : appname)+":.*([0-9.]+).*",Qt::CaseInsensitive);
        if (rx.indexIn(data) != -1) 
            return rx.cap(1);
    }
    return QString();
}

int InstallerEngine::getStartMenuGeneratorVersion()
{
    return getAppVersion("kwinstartmenu", "kwinstartmenu");
}

bool InstallerEngine::setDependencyState(Package *_package, QList<Package *> &dependencies)
{
    qDebug() << __FUNCTION__ << _package->name();

    Q_FOREACH(const QString &dep, _package->deps())
    {
        Package *package = m_packageResources->getPackage(dep);
        if (!package)
            continue;

        // check dependencies first
        bool ret = setDependencyState(package, dependencies);

        stateType state = packageStates.getState(package,FileTypes::BIN);
        stateType depState = dependencyStates.getState(package,FileTypes::BIN);

        Package *installedPackage = m_database->getPackage(dep);

        // if package is already installed, ignore it
        if (installedPackage && installedPackage->version() == package->version())
            continue;

        // set installed version for uninstaller - this should be set in a more central places
        if (installedPackage && package->installedVersion().isEmpty())
            package->setInstalledVersion(installedPackage->version());

        // the package is installed with a different version
        stateType newState = installedPackage ? _Update : _Install;

        // only add package if is neither selected in main or dependency states
        if ((state == _Nothing || state == _Remove) && (depState == _Nothing || depState == _Remove))
        {
            qDebug() << __FUNCTION__ << "selected package" << package->name() << "in previous state" << state << "for" << newState;
            if (!dependencies.contains(package))
                dependencies.append(package);

            dependencyStates.setState(package,FileTypes::BIN,newState);
#if 0
            // set additional package types for download/install/remove
            if (m_displayMode == Developer)
            {
                if (package->hasType(Package::LIB))
                    dependencyStates.setState(package,Package::LIB,newState);
                if (package->hasType(Package::DOC))
                    dependencyStates.setState(package,Package::DOC,newState);
            }
            else if (m_displayMode == BinaryOnly)
            {
                ;//if (package->hasType(Package::DOC))
                 //   dependenciesStates.setState(package,Package::DOC,_Install);
            }
#endif
        }
    }
    return true;
}

void InstallerEngine::checkUpdateDependencies(QList<Package*> &deps)
{
    dependencyStates.clear();
    if (deps.count() > 0)
        deps.clear();

    Q_FOREACH(Package *pkg, packageStates.packages(m_packageResources)) {
        if (!setDependencyState(pkg,deps))
            break;
    }
    qDebug() <<  packageStates;
    qDebug() << dependencyStates;
}

QDebug &operator<<(QDebug &out, const InstallerEngine &c)
{
    /// @TODO add more members
    out << "InstallerEngine ("
        << "m_packageResources:" << *c.m_packageResources
//        << "m_installer:" << *(c.m_installer)
        << "m_downloader:" << *(Downloader::instance())
//        << "m_instProgress:" << *c.m_instProgress
        << "m_globalConfig:" << *(c.m_globalConfig)
//        << "m_instProgressBar:" << *c.m_instProgressBar
        << "m_database:" << *(c.m_database)
        << ")";
    return out;
}

void InstallerEngine::slotError(const QString &msg)
{
    emit error(msg);
}

bool InstallerEngine::includePackage(CompilerTypes::Type compilerType, const QString &name, const QString &categoryName)
{
    // check correctness
    if ( ( categoryName == "mingw"  || compilerType == CompilerTypes::MinGW )
        &&  QRegExp(".*-(msvc|vc90|vc100|mingw4)$").exactMatch(name) )
        return false;
    else if ( ( categoryName == "mingw4"  || compilerType == CompilerTypes::MinGW4 )
            && QRegExp(".*-(mingw|x86-mingw4|msvc|vc90|vc100)$" ).exactMatch(name) )
        return false;
    else if ( ( categoryName == "mingw4"  || compilerType == CompilerTypes::MinGW4_W32 )
            && ( QRegExp(".*-(mingw|mingw4|msvc|vc90|vc100)$" ).exactMatch(name) && !QRegExp(".*-x86-mingw4$" ).exactMatch(name) ) )
        return false;
    else if ( ( categoryName == "mingw4"  || compilerType == CompilerTypes::MinGW4_W64 )
            && ( QRegExp(".*-(mingw|mingw4|msvc|vc90|vc100)$" ).exactMatch(name) && !QRegExp(".*-x64-mingw4$" ).exactMatch(name) ) )
        return false;
    else if ( ( categoryName == "msvc"  || compilerType == CompilerTypes::MSVC9 )
              && QRegExp(".*-(mingw|mingw4|vc100)$" ).exactMatch(name) )
        return false;
    else if ( ( categoryName == "msvc"  || compilerType == CompilerTypes::MSVC10 )
              && QRegExp(".*-(mingw|mingw4|msvc|vc90)$" ).exactMatch(name)  )
        return false;
    return true;
}

bool InstallerEngine::includeCategory(CompilerTypes::Type compilerType, const QString &categoryName)
{
    // not compiler specific
    if (!allCompilers.values().contains(categoryName))
        return true; 
    // compiler specific
    return supportedCompilers.toString(compilerType) == categoryName;
}
