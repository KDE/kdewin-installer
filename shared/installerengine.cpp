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

#include <QtDebug>
#include <QDir>
#include <QTreeWidget>
#include <QFlags>

#include "installerengine.h"

#include "downloader.h"
#include "installer.h"
#include "installerprogress.h"
#include "package.h"
#include "packagelist.h"
#include "globalconfig.h"
#include "database.h"

QString InstallerEngine::defaultConfigURL;
QString InstallerEngine::fallBackURL = "http://82.149.170.66/kde-windows";

InstallerEngine::InstallerEngine(QObject *parent)
    : QObject(parent),
      m_globalConfigReaded(false),
      m_initFinished(false),
      m_addedPackages(false),
      m_canceled(false)
{
    m_database = new Database();
    m_database->setRoot(Settings::getInstance().installDir());
    m_installer = new Installer( );
    m_installer->setRoot(Settings::getInstance().installDir());
    m_installer->setDatabase(m_database);
    m_globalConfig = new GlobalConfig();
    m_packageResources = new PackageList();

    connect(&Settings::getInstance(),SIGNAL(installDirChanged(const QString&)),this,SLOT(installDirChanged(const QString&)));
    connect(&Settings::getInstance(),SIGNAL(mirrorChanged(const QString&)),this,SLOT(mirrorChanged(const QString&)));
}

InstallerEngine::~InstallerEngine()
{
    delete m_database;
    delete m_installer;
    delete m_globalConfig;
    delete m_packageResources;
}

void InstallerEngine::initGlobalConfig()
{
    if (m_globalConfigReaded)
        m_globalConfig->clear();
    readGlobalConfig();
    QHash<QString, QString>::const_iterator i = m_globalConfig->categoryNotes().constBegin();
    for (; i != m_globalConfig->categoryNotes().constEnd(); i++) 
        categoryCache.setNote(i.key(),i.value());

    m_globalConfigReaded = true;
}

bool InstallerEngine::initPackages()
{
    if (m_addedPackages)
        m_packageResources->clear();
    if (!addPackagesFromGlobalConfig())
        return false;
    if (!addPackagesFromSites())
        return false;

    // add site independend package category relations
    QHash<QString, QStringList>::const_iterator i = m_globalConfig->categoryPackages().constBegin();
    for (; i != m_globalConfig->categoryPackages().constEnd(); i++) 
    {
        foreach (const QString &name,i.value())
        {
            Package *pkg = m_packageResources->getPackage(name);
            if (pkg)
            {
                pkg->addCategories(i.key());
                categoryCache.addPackage(pkg);
            }
        }
    }
    m_addedPackages = true;
    m_initFinished = true;
    return true;
}

bool InstallerEngine::init()
{
    initGlobalConfig();
    return initPackages();
}

void InstallerEngine::reload()
{
    m_database->clear();
    m_database->setRoot(Settings::getInstance().installDir());
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
        m_usedDownloadSource = defaultConfigURL;
    }
    if (configFiles.isEmpty())
    {
        hostURL = Settings::getInstance().mirror();
        if (!hostURL.isEmpty())
        {
            configFiles = m_globalConfig->fetch(hostURL);
            m_usedDownloadSource = hostURL;
        }
    }
    if (configFiles.isEmpty())
    {
        if (hostURL != fallBackURL) // fallBack URL is other url
        {
            configFiles = m_globalConfig->fetch(fallBackURL);
            m_usedDownloadSource = fallBackURL;
        }
    }
    if (configFiles.isEmpty())
        return false;

    if (!m_globalConfig->parse(configFiles))
        return false;
    qDebug() << "globalconfig has timestamp" << m_globalConfig->timeStamp().toString(Qt::SystemLocaleDate);
    return true;
}

bool InstallerEngine::addPackagesFromGlobalConfig()
{
    QList<Package*>::iterator p;
    for (p = m_globalConfig->packages()->begin(); p != m_globalConfig->packages()->end(); p++)
    {
        Package *pkg = *p;
        pkg->addCategories("all");
        if (pkg->name().contains("mingw"))
            pkg->addCategories("mingw");
        if (pkg->name().contains("msvc"))
            pkg->addCategories("msvc");

        m_packageResources->addPackage(*pkg);
        categoryCache.addPackage(pkg);
    }
    if (Settings::hasDebug("InstallerEngine"))
        qDebug() << __FUNCTION__ << m_packageResources;
    return true;
}

/// download all packagelists, which are available on the configured sites
bool InstallerEngine::addPackagesFromSites()
{
    QList<Site*>::iterator s;
    for (s = m_globalConfig->sites()->begin(); s != m_globalConfig->sites()->end(); s++)
    {
        Site *site = (*s);
        PackageList packageList;
        QString category = site->name();
        qDebug() << "download package file list for site: " << category << "from" << site->url();

        categoryCache.setNote(category,site->notes());
        packageList.setNotes(site->notes());

        // packagelist needs to access Site::getDependencies() && Site::isExclude()
        packageList.setCurrentSite(site);

        packageList.setBaseURL(site->url());

        QUrl listURL = site->listURL().isEmpty() ? site->url() : site->listURL();

        QByteArray ba;
        qDebug() << listURL;
        if (!Downloader::instance()->start(listURL, ba))
        {
            emit error(tr("failed to download site list page from %1").arg(listURL.toString()));
            return false;
        }
        PackageList::Type type;

        switch(site->Type()) {
        case Site::SourceForge:    type = PackageList::SourceForge; break;
        case Site::SourceForgeMirror: type = PackageList::SourceForgeMirror; break;
        case Site::ApacheModIndex: type = PackageList::ApacheModIndex; break;
        default:
            emit error("unknown Site type "  + site->Type());
            type = PackageList::ApacheModIndex;
            break;
        }

        if (!packageList.readHTMLFromByteArray(ba, type, true ))
        {
            emit error("error reading package list from download html file");
            continue;
        }
        if (Settings::hasDebug("InstallerEngine"))
            qDebug() << __FUNCTION__ << packageList;

        foreach(Package *pkg, packageList.packages())
        {
            // add some generic categories
            pkg->addCategories("all");
            pkg->addCategories(category);
            if (pkg->name().contains("mingw"))
                pkg->addCategories("mingw");
            if (pkg->name().contains("msvc"))
                pkg->addCategories("msvc");

            // add categories defined in the config 
            pkg->addCategories(site->packageCategories(pkg->name()));

            m_packageResources->addPackage(*pkg);
            categoryCache.addPackage(pkg);
        }
    }

    return true;
}

Package *InstallerEngine::getPackageByName(const QString &name,const QString &version)
{
    return m_packageResources->getPackage(name,version.toAscii());
}

void InstallerEngine::stop()
{
    Downloader::instance()->cancel();
    m_canceled = true;
}

void InstallerEngine::installDirChanged(const QString &newdir)
{
    m_installer->setRoot(newdir);
    // this will fetch installed packages
    m_database->setRoot(newdir);
}

void InstallerEngine::mirrorChanged(const QString &mirror)
{
    if (m_initFinished)
        reload();
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


#include "installerengine.moc"
