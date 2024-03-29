/****************************************************************************
**
** Copyright (C) 2005-2011 Ralf Habacker. All rights reserved.
**
** This file is part of the KDE installer for windows
**
** SPDX-License-Identifier: LGPL-2.0-only
**
****************************************************************************/

#include "database.h"
#include "debug.h"
#include "downloader.h"
#include "globalconfig.h"
#include "installer.h"
#include "installerengineconsole.h"
#include "installerprogress.h"
#include "package.h"
#include "packageinfo.h"
#include "packagelist.h"
#include "qio.h"
#include "typehelper.h"

#include <QtDebug>
#include <QDir>
#include <QFlags>

/**
 package names could be in the form 
 <package>
 <package>-<compiler>
 <package>-<compiler>-<type>
*/

InstallerEngineConsole::InstallerEngineConsole()
  : InstallerEngine(0), done(false), m_installedPackages(0)
{
}

bool InstallerEngineConsole::init()
{
    if (m_root.isEmpty())
    {
        qCritical() << "no install root set";
        return false;
    }
    if (done)
        return true;
    initGlobalConfig();
    if (isInstallerVersionOutdated())   
        qWarning() << "Installer version outdated";
    done = true;
    return initPackages();
}


void InstallerEngineConsole::printPackage(Package *p)
{
    if (!p)
        return;
    if (p->isInstalled(FileTypes::BIN))
        qprintf("%s-bin-%s\n",qPrintable(p->name()), qPrintable(p->version().toString())); 
    if (p->isInstalled(FileTypes::LIB))
        qprintf("%s-lib-%s\n",qPrintable(p->name()), qPrintable(p->version().toString())); 
    if (p->isInstalled(FileTypes::DOC))
        qprintf("%s-doc-%s\n",qPrintable(p->name()), qPrintable(p->version().toString())); 
    if (p->isInstalled(FileTypes::SRC))
        qprintf("%s-src-%s\n",qPrintable(p->name()), qPrintable(p->version().toString()));   
    if (p->isInstalled(FileTypes::DBG))
        qprintf("%s-dbg-%s\n",qPrintable(p->name()), qPrintable(p->version().toString()));   
}

void InstallerEngineConsole::queryPackage()
{
    Q_FOREACH(Package *p,m_database->packages())
        printPackage(p);
}

void InstallerEngineConsole::queryPackage(const QString &pkgName)
{
    Package *p = m_database->getPackage(pkgName);
    if (p)
        return;
    
    printPackage(p);
}

void InstallerEngineConsole::queryPackage(const QStringList &list)
{
    Q_FOREACH(const QString &pkgName,list)
        queryPackage(pkgName);
}

void InstallerEngineConsole::queryPackageListFiles(const QString &pkgName)
{
    Package *p = m_database->getPackage(pkgName);
    if (!p)
        return;

    PackageInfo info = PackageInfo::fromString(pkgName);
    if ((info.type == FileTypes::NONE || info.type == FileTypes::BIN) && p->isInstalled(FileTypes::BIN))
    {
        Q_FOREACH(const QString &file, m_database->getPackageFiles(pkgName,FileTypes::BIN))
            qprintf("%s%s\n", info.type == FileTypes::NONE  ? "BIN: " : "", qPrintable(file));
    }
    if ((info.type == FileTypes::NONE || info.type == FileTypes::LIB) && p->isInstalled(FileTypes::LIB))
    {
        Q_FOREACH(const QString &file, m_database->getPackageFiles(pkgName,FileTypes::LIB))
            qprintf("%s%s\n", info.type == FileTypes::NONE  ? "LIB: " : "", qPrintable(file));
    }
    if ((info.type == FileTypes::NONE || info.type == FileTypes::DOC) && p->isInstalled(FileTypes::DOC))
    {
        Q_FOREACH(const QString &file, m_database->getPackageFiles(pkgName,FileTypes::DOC))
            qprintf("%s%s\n", info.type == FileTypes::NONE  ? "DOC: " : "", qPrintable(file));
    }
    if ((info.type == FileTypes::NONE || info.type == FileTypes::SRC) && p->isInstalled(FileTypes::SRC))
    {
        Q_FOREACH(const QString &file, m_database->getPackageFiles(pkgName,FileTypes::SRC))
            qprintf("%s%s\n", info.type == FileTypes::NONE  ? "SRC: " : "", qPrintable(file));
    }
    if ((info.type == FileTypes::NONE || info.type == FileTypes::DBG) && p->isInstalled(FileTypes::DBG))
    {
        Q_FOREACH(const QString &file, m_database->getPackageFiles(pkgName,FileTypes::DBG))
            qprintf("%s%s\n", info.type == FileTypes::NONE  ? "DBG: " : "", qPrintable(file));
    }
}

void InstallerEngineConsole::queryPackageListFiles(const QStringList &list)
{
    Q_FOREACH(const QString &pkgName, list)
        queryPackageListFiles(pkgName);
}

void InstallerEngineConsole::queryPackageWhatRequires(const QString &pkgName)
{
    init();
    Package *p = m_packageResources->find(pkgName);
    if (!p)
        return; 

    Q_FOREACH(const QString &dep, p->deps()) 
        qprintf("%s\n", qPrintable(dep));
}

void InstallerEngineConsole::queryPackageWhatRequires(const QStringList &list)
{
    init();
    Q_FOREACH(const QString &pkgName, list) 
        queryPackageWhatRequires(pkgName);
}

void InstallerEngineConsole::queryPackageWhatRequiresAll(const QString &pkgName, QStringList &list)
{
    init();
    static int level=0; 
    Package *p = m_packageResources->find(pkgName);
    if (!p)
        return; 

    level++;
    Q_FOREACH(const QString &dep, p->deps()) 
    {
        queryPackageWhatRequiresAll(dep,list);
        if (!dep.isEmpty() && !list.contains(dep))
            list.append(dep);
        // debug print 
        //printf("%d %s\n", level, qPrintable(dep));
    }
    level--;
}
 
void InstallerEngineConsole::queryPackageWhatRequiresAll(const QStringList &list)
 {
    init();
    QStringList result;
    Q_FOREACH(const QString &pkgName, list) 
        queryPackageWhatRequiresAll(pkgName,result);
    result.sort();
    Q_FOREACH(const QString &dep, result) 
        qprintf("%s\n", qPrintable(dep));    
}


void InstallerEngineConsole::listPackage()
{
    init();
    Q_FOREACH(Package *p, m_packageResources->packages())
        printPackage(p);
}

void InstallerEngineConsole::listPackage(const QString &pkgName)
{
    init();
    Package *p = m_packageResources->find(PackageInfo::fromFileName(pkgName));
    printPackage(p);
}

void InstallerEngineConsole::listPackage(const QStringList &list)
{
    init();
    Q_FOREACH(const QString &pkgName, list)
        listPackage(pkgName);
}

void InstallerEngineConsole::printPackageURLs(Package *p)
{
    if (!p)
        return;
    QUrl url;
    url = p->getUrl(FileTypes::BIN);
    if (!url.isEmpty())
        qprintf("%s\n",qPrintable(url.toString())); 
    
    if (!Settings::instance().isDeveloperInstallMode())
        return;

    url = p->getUrl(FileTypes::LIB);
    if (!url.isEmpty())
        qprintf("%s\n",qPrintable(url.toString())); 
    url = p->getUrl(FileTypes::DOC);
    if (!url.isEmpty())
        qprintf("%s\n",qPrintable(url.toString())); 
    url = p->getUrl(FileTypes::SRC);
    if (!url.isEmpty())
        qprintf("%s\n",qPrintable(url.toString())); 
    url = p->getUrl(FileTypes::DBG);
    if (!url.isEmpty())
        qprintf("%s\n",qPrintable(url.toString())); 
}

void InstallerEngineConsole::listPackageURLs()
{
    init();
    QList <Package*> list = m_packageResources->packages(); 
    Q_FOREACH(Package *p, list)
    {
        if (includePackage(Settings::instance().compilerType(),p->name()))
            printPackageURLs(p);
    }
}

void InstallerEngineConsole::listPackageURLs(const QString &pkgName)
{
    init();
    Package *p = m_packageResources->find(pkgName);
    if (includePackage(Settings::instance().compilerType(),p->name()))
        printPackageURLs(p);
}

void InstallerEngineConsole::listPackageURLs(const QStringList &list)
{
    init();
    Q_FOREACH(const QString &pkgName, list)
    {
        if (includePackage(Settings::instance().compilerType(),pkgName))
            listPackageURLs(pkgName);
    }
}

void InstallerEngineConsole::listPackageDescription(const QString &pkgName)
{
    init();
    Package *p = m_packageResources->find(pkgName);
    if (!p)
        return;
    qprintf("%s\n",qPrintable(p->notes()));
}

void InstallerEngineConsole::listPackageDescription(const QStringList &list)
{
    init();
    Q_FOREACH(const QString &pkgName, list)
        listPackageDescription(pkgName);
}

void InstallerEngineConsole::listPackageCategories(const QString &pkgName)
{
    init();
    Package *p = m_packageResources->find(pkgName);
    if (!p)
        return;
    qprintf("%s\n",qPrintable(p->categories().join("\n")));
}

void InstallerEngineConsole::listPackageCategories(const QStringList &list)
{
    init();
    Q_FOREACH(const QString &pkgName, list)
        listPackageCategories(pkgName);
}

bool InstallerEngineConsole::downloadPackages(const QStringList &packages, const QString &category)
{
    init();
    Q_FOREACH(const QString &pkgName, packages)
    {
        PackageInfo pi(pkgName);
        if (!pi.isValid())
        {
            qError() << "package \"" << pkgName << "\" is not a valid package name\n";
            continue;
        }
        bool hasType = pi.hasType();

        Package *p = m_packageResources->find(pkgName);
        if (!p)
        {
            qError() << "package \"" << pkgName << "\" not available for download\n";
            continue;
        }
        if (p->hasType(FileTypes::BIN) && (!hasType || pi.isType(FileTypes::BIN)) )
            p->downloadItem(FileTypes::BIN);
        if (p->hasType(FileTypes::LIB) && (!hasType || pi.isType(FileTypes::LIB)) )
            p->downloadItem(FileTypes::LIB);
        if (p->hasType(FileTypes::DOC) && (!hasType || pi.isType(FileTypes::DOC)) )
            p->downloadItem(FileTypes::DOC);
        if (p->hasType(FileTypes::SRC) && (!hasType || pi.isType(FileTypes::SRC)) )
            p->downloadItem(FileTypes::SRC);
        if (p->hasType(FileTypes::DBG) && (!hasType || pi.isType(FileTypes::DBG)) )
            p->downloadItem(FileTypes::DBG);
    }
    return true;
}

bool InstallerEngineConsole::installPackages(const QStringList &packages,const QString &category)
{
    init();
    Q_FOREACH(const QString &pkgName, packages)
    {
        PackageInfo pi(pkgName);
        if (!pi.isValid())
        {
            qError() << "package \"" << pkgName << "\" is not a valid package name\n";
            continue;
        }
        bool hasType = pi.hasType();

        Package *p = m_packageResources->find(pkgName);
        if (!p)
        {
            qError() << "package \"" << pkgName << "\" not available for installation\n";
            continue;
        }
        if (p->hasType(FileTypes::BIN) && (!hasType || pi.isType(FileTypes::BIN)) )
            p->installItem(m_installer,FileTypes::BIN);
        if (p->hasType(FileTypes::LIB) && (!hasType || pi.isType(FileTypes::LIB)) )
            p->installItem(m_installer,FileTypes::LIB);
        if (p->hasType(FileTypes::DOC) && (!hasType || pi.isType(FileTypes::DOC)) )
            p->installItem(m_installer,FileTypes::DOC);
        if (p->hasType(FileTypes::SRC) && (!hasType || pi.isType(FileTypes::SRC)) )
        {
            // src packages unpacks into the wrong location, do not enable yet
            // p->installItem(m_installer,FileTypes::SRC);
            qDebug() << "installing src package \"" << pkgName << "\" not possible due to package problems\n";
        }
        if (p->hasType(FileTypes::DBG) && (!hasType || pi.isType(FileTypes::DBG)) )
            p->installItem(m_installer,FileTypes::DBG);
        m_installedPackages++;
    }
    return true;
}

/**
 install package from a local file
 @param file url of the local file
 @return true installing finished without any error
*/
bool InstallerEngineConsole::installPackages(const QUrl &file)
{
    init();
    Package package;
    Package::PackageItem item(FileTypes::BIN);
    QFileInfo fi(file.toLocalFile());
    item.setFileName(fi.fileName());
    package.add(item);
    // save an restore original value
    Settings::instance().setDownloadDir(fi.absolutePath(),false);

    Package *p = &package;

    if (p->hasType(FileTypes::BIN))
        p->installItem(m_installer,FileTypes::BIN);
    if (p->hasType(FileTypes::LIB))
        p->installItem(m_installer,FileTypes::LIB);
    if (p->hasType(FileTypes::DOC))
        p->installItem(m_installer,FileTypes::DOC);
    if (p->hasType(FileTypes::SRC))
        p->installItem(m_installer,FileTypes::SRC);
    if (p->hasType(FileTypes::DBG))
        p->installItem(m_installer,FileTypes::DBG);
    m_installedPackages++;
    return true;
}


bool InstallerEngineConsole::removePackages(const QStringList &packages)
{
    init();
    Q_FOREACH(const QString &pkgName, packages)
    {
        PackageInfo pi(pkgName);
        if (!pi.isValid())
        {
            qError() << "package \"" << pkgName << "\" is not a valid package name\n";
            continue;
        }
        bool hasType = pi.hasType();

        Package *p = m_database->getPackage(pkgName);
        if (!p)
        {
            qError() << "package \"" << pkgName << "\" could not be removed, it is not installed\n";
            continue;
        }
        if (p->hasType(FileTypes::BIN) && (!hasType || pi.isType(FileTypes::BIN)) )
            p->removeItem(m_installer,FileTypes::BIN);
        if (p->hasType(FileTypes::LIB) && (!hasType || pi.isType(FileTypes::LIB)) )
            p->removeItem(m_installer,FileTypes::LIB);
        if (p->hasType(FileTypes::DOC) && (!hasType || pi.isType(FileTypes::DOC)) )
            p->removeItem(m_installer,FileTypes::DOC);
        if (p->hasType(FileTypes::SRC) && (!hasType || pi.isType(FileTypes::SRC)) )
            p->removeItem(m_installer,FileTypes::SRC);
        if (p->hasType(FileTypes::DBG) && (!hasType || pi.isType(FileTypes::DBG)) )
            p->removeItem(m_installer,FileTypes::DBG);
    }
    return true;
}
