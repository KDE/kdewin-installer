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

InstallerEngine::InstallerEngine(DownloaderProgress *progressBar,InstallerProgress *instProgressBar)
    : QObject(), m_instProgressBar(instProgressBar)
{
    m_database = new Database();
    m_downloader = new Downloader(/*blocking=*/ true,progressBar);
    m_database->setRoot(Settings::getInstance().installDir());
    m_installer = new Installer(m_instProgressBar );
    m_installer->setRoot(Settings::getInstance().installDir());
    m_installer->setDatabase(m_database);
    m_globalConfig = new GlobalConfig(m_downloader);
    connect(&Settings::getInstance(),SIGNAL(installDirChanged(const QString&)),this,SLOT(installDirChanged(const QString&)));
    connect(&Settings::getInstance(),SIGNAL(mirrorChanged(const QString&)),this,SLOT(mirrorChanged(const QString&)));
}

InstallerEngine::~InstallerEngine()
{
    delete m_database;
    delete m_installer;
    delete m_downloader;
    delete m_globalConfig;
}

bool InstallerEngine::readGlobalConfig()
{
    QString hostURL;
    QStringList configFiles;

    if (!defaultConfigURL.isEmpty()) // command line overrides
    {
        configFiles = m_globalConfig->fetch(defaultConfigURL);
    }
    if (configFiles.isEmpty())
    {
        hostURL = Settings::getInstance().mirror();
        if (!hostURL.isEmpty())
            configFiles = m_globalConfig->fetch(hostURL);
    }
    if (configFiles.isEmpty())
    {
        if (hostURL != fallBackURL) // fallBack URL is other url
            configFiles = m_globalConfig->fetch(fallBackURL);
    }
    if (configFiles.isEmpty())
        return false;

    if (!m_globalConfig->parse(configFiles))
        return false;

    createMainPackagelist();
    return true;
}

void InstallerEngine::createMainPackagelist()
{
    if (Settings::hasDebug("InstallerEngine"))
        m_database->listPackages("createMainPackageList - installed packages");

    m_packageResources = new PackageList();

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
        dump("createMainPackageList");
}

/// download all packagelists, which are available on the configured sites
bool InstallerEngine::downloadPackageLists()
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

        packageList.setBaseURL(site->url().toString());    // why not use QUrl here?

#ifdef DEBUG
        QFileInfo tmpFile(m_installer->root() + "/packages-"+site->name()+".html");
        if (!tmpFile.exists())
            m_downloader->start(site->url(), tmpFile.absoluteFilePath());

        // load and parse
        if (!packageList->readHTMLFromFile(tmpFile.absoluteFilePath(),site->Type() == Site::ApacheModIndex ? PackageList::ApacheModIndex : PackageList::SourceForge ))
#else
        QByteArray ba;
        m_downloader->start(site->url(), ba);
        if (!packageList.readHTMLFromByteArray(ba,site->Type() == Site::ApacheModIndex ? PackageList::ApacheModIndex : PackageList::SourceForge, true ))
#endif
        {
            qDebug() << "error reading package list from download html file";
            continue;
        }
        foreach(Package *pkg, packageList.packageList()) 
        {
            // merge with duplicated definition above
            pkg->addCategories("all");
            if (pkg->name().contains("mingw"))
                pkg->addCategories("mingw");
            if (pkg->name().contains("msvc"))
                pkg->addCategories("msvc");

            m_packageResources->addPackage(*pkg);
            categoryCache.addPackage(pkg);
        }
    }
    if (Settings::hasDebug("InstallerEngine"))
        dump("downloadPackageLists");

    return true;
}

Package *InstallerEngine::getPackageByName(const QString &name,const QString &version)
{
    return m_packageResources->getPackage(name,version.toAscii());
}

void InstallerEngine::stop()
{
    m_downloader->cancel();
}

void InstallerEngine::installDirChanged(const QString &newdir)
{
    m_installer->setRoot(newdir);
    m_database->setRoot(newdir);
}

void InstallerEngine::mirrorChanged(const QString &mirror)
{
    delete m_globalConfig;
    m_globalConfig = new GlobalConfig(m_downloader);
    readGlobalConfig();
    downloadPackageLists();
}

void InstallerEngine::dump(const QString &title)
{
    QList<Package*>::ConstIterator i = m_packageResources->packageList().constBegin();
    for ( ; i != m_packageResources->packageList().constEnd(); ++i)
    {
        Package *pkg = *i;
        qDebug() << pkg;
    }
}

#include "installerengine.moc"
