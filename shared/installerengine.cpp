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
{
	m_progressBar = progressBar;
	m_instProgressBar = instProgressBar; 
	m_database = &Database::getInstance();
#ifdef PRINT_AVAILABLE_PACKAGES	
	m_availablePackages = new PackageList();
	m_availablePackages->setName("all packages");
	Installer *installer = new Installer(m_availablePackages,m_instProgressBar );
	installer->setRoot(root());
	m_installerList.append(installer);
#endif
}

void InstallerEngine::readGlobalConfig()
{
	m_downloader = new Downloader(/*blocking=*/ true,m_progressBar);
    m_globalConfig = new GlobalConfig(QString("http://82.149.170.66/kde-windows/installer/config.txt"),*m_downloader);
    createMainPackagelist();
}

void InstallerEngine::createMainPackagelist()
{
    // package list is build from packages defined in global configuration
	m_database->readFromDirectory(root()+"/manifest");
#ifdef DEBUG
	m_database->listPackages("Package");
#endif
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
			/**
			  @TODO: m_installer is used for installing, there should be only one 
			  instance for each installation root, which requires not to store a 
			  package list pointer in the installer
			*/ 
		    Installer *installer = new Installer(packageList,m_instProgressBar );
			installer->setRoot(root());
		    m_installerList.append(installer);
			m_installer = installer;
		}
		packageList->addPackage(*pkg);
#ifdef PRINT_AVAILABLE_PACKAGES			
		m_availablePackages->addPackage(*pkg);
#endif
    }
	foreach(PackageList *pkgList, m_packageListList)
		pkgList->syncWithDatabase(*m_database);
}

/// download all packagelists, which are available on the configured sites
bool InstallerEngine::downloadPackageLists()
{
    QList<Site*>::iterator s;
    for (s = m_globalConfig->sites()->begin(); s != m_globalConfig->sites()->end(); s++)
    {
		QString category = (*s)->name();
#ifdef DEBUG
		qDebug() << "download package file list for site: " << category;
#endif
		PackageList *packageList = getPackageListByName(category);
		if (!packageList)
		{
		    packageList = new PackageList();
			packageList->setName(category);
			m_packageListList.append(packageList);
		    Installer *installer = new Installer(packageList,m_instProgressBar );
			installer->setRoot(root());
		    m_installerList.append(installer);
		}
		packageList->setNotes((*s)->notes());

		// packagelist needs to access Site::getDependencies() && Site::isExclude()
        packageList->setCurrentSite(*s);

/*
		// FIXME:: hardcoded name, better to use an option in the config file ?
        if ((*s)->name() == "gnuwin32")
        {
            installer->setType(Installer::GNUWIN32);
            // FIXME: add additional option in config.txt for mirrors
            packageList->setBaseURL("http://heanet.dl.sourceforge.net/sourceforge/gnuwin32/");
        }
        else
*/
        packageList->setBaseURL((*s)->url());

        qDebug() << (*s)->url();
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
	    Installer *installer = new Installer(packageList,m_instProgressBar );
		installer->setRoot(root());
	    m_installerList.append(installer);
	}
	m_database->addUnhandledPackages(packageList);

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

void InstallerEngine::setRoot(QString root)
{
    Settings::getInstance().setInstallDir(root);
}

const QString InstallerEngine::root()
{
    QString root = Settings::getInstance().installDir();
    if (root.isEmpty())
    {
        root = QDir::currentPath();
        Settings::getInstance().setInstallDir(root);
    }
    return QDir::convertSeparators(root);
}

void InstallerEngine::stop()
{
    m_downloader->cancel();
}

