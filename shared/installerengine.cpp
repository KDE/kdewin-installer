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

InstallerEngine::InstallerEngine(DownloaderProgress *progressBar,InstallerProgress *instProgressBar)
    : QObject(), m_instProgressBar(instProgressBar)
{
    m_database = new Database();
    m_downloader = new Downloader(/*blocking=*/ true,progressBar);
#ifdef PRINT_AVAILABLE_PACKAGES    
    m_availablePackages = new PackageList();
    m_availablePackages->setName("all packages");
#endif
    m_database->setRoot(Settings::getInstance().installDir());
    m_installer = new Installer(m_instProgressBar );
    m_installer->setRoot(Settings::getInstance().installDir());
    m_installer->setDatabase(m_database);
    m_globalConfig = new GlobalConfig();
    connect(&Settings::getInstance(),SIGNAL(installDirChanged(const QString&)),this,SLOT(installDirChanged(const QString&)));
    connect(&Settings::getInstance(),SIGNAL(mirrorChanged(const QString&)),this,SLOT(mirrorChanged(const QString&)));
}

InstallerEngine::~InstallerEngine()
{
    m_packageListList.clear();
    // FIXME: application crashes when calling qDeleteAll()
    //qDeleteAll(m_packageListList);
    delete m_database;
    delete m_installer;
    delete m_downloader;
    delete m_globalConfig;
}

bool InstallerEngine::readGlobalConfig()
{
    QString fallBackHostURL = "http://82.149.170.66/kde-windows";
    QString hostURL = Settings::getInstance().mirror();
    if (!hostURL.isEmpty())    
    {
        if (!m_globalConfig->download(hostURL,*m_downloader))
        {
            if (hostURL == fallBackHostURL) // no other host available
                return false;
            else if (!m_globalConfig->download(fallBackHostURL,*m_downloader))
               return false;
        }
    }   
    else if (!m_globalConfig->download(fallBackHostURL,*m_downloader))
        return false;

    createMainPackagelist();
    return true;
}

void InstallerEngine::createMainPackagelist()
{
	if (Settings::hasDebug("InstallerEngine"))
		m_database->listPackages("createMainPackageList - installed packages");

	QList<Package*>::iterator p;
    for (p = m_globalConfig->packages()->begin(); p != m_globalConfig->packages()->end(); p++)
    {
        Package *pkg = (*p);
        QString category = pkg->category();
        if (category.isEmpty())
            category = "main"; 
        PackageList *packageList = getPackageListByName(category);
        if (!packageList)
        {
            packageList = new PackageList();
            packageList->setName(category);
            m_packageListList.append(packageList);
        }
        packageList->addPackage(*pkg);
#ifdef PRINT_AVAILABLE_PACKAGES            
        m_availablePackages->addPackage(*pkg);
#endif
    }
    foreach(PackageList *pkgList, m_packageListList)
        pkgList->syncWithDatabase(*m_database);

	if (Settings::hasDebug("InstallerEngine"))
		dump("createMainPackageList");
}

/// download all packagelists, which are available on the configured sites
bool InstallerEngine::downloadPackageLists()
{
    QList<Site*>::iterator s;
    for (s = m_globalConfig->sites()->begin(); s != m_globalConfig->sites()->end(); s++)
    {
        QString category = (*s)->name();
        qDebug() << "download package file list for site: " << category << "from" << (*s)->url();
        PackageList *packageList = getPackageListByName(category);
        if (!packageList)
        {
            packageList = new PackageList();
            packageList->setName(category);
            m_packageListList.append(packageList);
        }
        packageList->setNotes((*s)->notes());

        // packagelist needs to access Site::getDependencies() && Site::isExclude()
        packageList->setCurrentSite(*s);

        packageList->setBaseURL((*s)->url());

#ifdef DEBUG
        QFileInfo tmpFile(installer->Root() + "/packages-"+(*s)->Name()+".html");
        if (!tmpFile.exists())
            m_downloader->start((*s)->URL(), ba));

        // load and parse
        if (!packageList->readHTMLFromFile(tmpFile.absoluteFilePath(),(*s)->Type() == Site::ApacheModIndex ? PackageList::ApacheModIndex : PackageList::SourceForge ))
#else            
        QByteArray ba;
        m_downloader->start((*s)->url(), ba);
        if (!packageList->readHTMLFromByteArray(ba,(*s)->Type() == Site::ApacheModIndex ? PackageList::ApacheModIndex : PackageList::SourceForge, true ))
#endif
        {
            qDebug() << "error reading package list from download html file";
            continue;
        }
#ifdef PRINT_AVAILABLE_PACKAGES            
        m_availablePackages->append(*packageList);
#endif
        packageList->syncWithDatabase(*m_database);
    }
    PackageList *packageList = getPackageListByName("outdated packages");
    if (!packageList)
    {
        packageList = new PackageList();
        packageList->setName("installed");
        packageList->setNotes("packages in this categories are installed, but are not listed in recent configuration because they are obsolate or outdated");
        m_packageListList.append(packageList);
    }
    m_database->addUnhandledPackages(packageList);
	if (Settings::hasDebug("InstallerEngine"))
		dump("downloadPackageLists");

#ifdef PRINT_AVAILABLE_PACKAGES            
    m_database->resetHandledState();
    m_availablePackages->syncWithDatabase(*m_database);
    m_database->addUnhandledPackages(m_availablePackages);
    m_packageListList.append(m_availablePackages);
#endif
    return true;
}

PackageList *InstallerEngine::getPackageListByName(const QString &name)
{
    QList <PackageList *>::iterator k;
    for (k = m_packageListList.begin(); k != m_packageListList.end(); ++k)
    {
        if ((*k)->Name() == name)
            return (*k);
    }
    return 0;
}

Package *InstallerEngine::getPackageByName(const QString &name,const QString &version)
{
      Package *pkg;
    QList <PackageList *>::iterator k;
    for (k = m_packageListList.begin(); k != m_packageListList.end(); ++k)
    {
       pkg = (*k)->getPackage(name,version.toAscii());
       if (pkg)
           return pkg;
    }
    return 0;
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
    m_globalConfig = new GlobalConfig();
    readGlobalConfig();
    downloadPackageLists();
}


void InstallerEngine::dump(const QString &title)
{
    QList <PackageList *>::ConstIterator k = m_packageListList.constBegin();
    for ( ; k != m_packageListList.constEnd(); ++k)
    {
        if ((*k)->packageList().size() == 0)
            continue;

		// adding sub items
        QList<Package*>::ConstIterator i = (*k)->packageList().constBegin();
        for ( ; i != (*k)->packageList().constEnd(); ++i)
        {
            Package *pkg = *i;
			qDebug() << pkg;
 			pkg->dump(__FUNCTION__);
		}
	}
}

#include "installerengine.moc"
