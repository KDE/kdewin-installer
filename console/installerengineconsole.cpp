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

#include "installerengineconsole.h"

#include "downloader.h"
#include "installer.h"
#include "installerprogress.h"
#include "package.h"
#include "packagelist.h"
#include "globalconfig.h"
#include "database.h"

InstallerEngineConsole::InstallerEngineConsole()
: InstallerEngine(0), done(false)
{
}

void InstallerEngineConsole::initLocal()
{
    // required because this is initial set in InstallerEngine's constructor
    m_database->setRoot(Settings::instance().installDir());
}

bool InstallerEngineConsole::init()
{
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
    if (p->isInstalled(Package::BIN))
        printf("%s-bin-%s\n",qPrintable(p->name()), qPrintable(p->version().toString())); 
    if (p->isInstalled(Package::LIB))
        printf("%s-lib-%s\n",qPrintable(p->name()), qPrintable(p->version().toString())); 
    if (p->isInstalled(Package::DOC))
        printf("%s-doc-%s\n",qPrintable(p->name()), qPrintable(p->version().toString())); 
    if (p->isInstalled(Package::SRC))
        printf("%s-src-%s\n",qPrintable(p->name()), qPrintable(p->version().toString()));   
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
    if ((info.type == Package::NONE || info.type == Package::BIN) && p->isInstalled(Package::BIN))
    {
        Q_FOREACH(const QString &file, m_database->getPackageFiles(pkgName,Package::BIN))
            printf("%s%s\n", info.type == Package::NONE  ? "BIN: " : "", qPrintable(file));
    }
    if ((info.type == Package::NONE || info.type == Package::LIB) && p->isInstalled(Package::LIB))
    {
        Q_FOREACH(const QString &file, m_database->getPackageFiles(pkgName,Package::LIB))
            printf("%s%s\n", info.type == Package::NONE  ? "LIB: " : "", qPrintable(file));
    }
    if ((info.type == Package::NONE || info.type == Package::DOC) && p->isInstalled(Package::DOC))
    {
        Q_FOREACH(const QString &file, m_database->getPackageFiles(pkgName,Package::DOC))
            printf("%s%s\n", info.type == Package::NONE  ? "DOC: " : "", qPrintable(file));
    }
    if ((info.type == Package::NONE || info.type == Package::SRC) && p->isInstalled(Package::SRC))
    {
        Q_FOREACH(const QString &file, m_database->getPackageFiles(pkgName,Package::SRC))
            printf("%s%s\n", info.type == Package::NONE  ? "SRC: " : "", qPrintable(file));
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
    Package *p = m_packageResources->getPackage(pkgName);
    if (!p)
        return; 

    Q_FOREACH(const QString &dep, p->deps()) 
        printf("%s\n", qPrintable(dep));
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
    Package *p = m_packageResources->getPackage(pkgName);
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
        printf("%s\n", qPrintable(dep));    
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
    Package *p = m_packageResources->getPackage(pkgName);
    printPackage(p);
}

void InstallerEngineConsole::listPackage(const QStringList &list)
{
    init();
    Q_FOREACH(const QString &pkgName, list)
        listPackage(pkgName);
}

bool includePackage(Package *p)
{
    if (Settings::instance().compilerType() == Settings::MinGW && p->name().contains("-msvc")
        || Settings::instance().compilerType() == Settings::MSVC && p->name().contains("-mingw"))
        return false;
    else
        return true;
}

bool includePackage(const QString &p)
{
    if (Settings::instance().compilerType() == Settings::MinGW && p.contains("-msvc")
        || Settings::instance().compilerType() == Settings::MSVC && p.contains("-mingw"))
        return false;
    else
        return true;
}

void InstallerEngineConsole::printPackageURLs(Package *p)
{
    if (!p)
        return;
    QUrl url;
    url = p->getUrl(Package::BIN);
    if (!url.isEmpty())
        printf("%s\n",qPrintable(url.toString())); 
    
    if (!Settings::instance().isDeveloperInstallMode())
        return;

    url = p->getUrl(Package::LIB);
    if (!url.isEmpty())
        printf("%s\n",qPrintable(url.toString())); 
    url = p->getUrl(Package::DOC);
    if (!url.isEmpty())
        printf("%s\n",qPrintable(url.toString())); 
    url = p->getUrl(Package::SRC);
    if (!url.isEmpty())
        printf("%s\n",qPrintable(url.toString())); 
}

void InstallerEngineConsole::listPackageURLs()
{
    init();
    QList <Package*> list = m_packageResources->packages(); 
    Q_FOREACH(Package *p, list)
    {
        if (includePackage(p))
            printPackageURLs(p);
    }
}

void InstallerEngineConsole::listPackageURLs(const QString &pkgName)
{
    init();
    Package *p = m_packageResources->getPackage(pkgName); 
    if (includePackage(p))
        printPackageURLs(p);
}

void InstallerEngineConsole::listPackageURLs(const QStringList &list)
{
    init();
    Q_FOREACH(const QString &pkgName, list)
    {
        if (includePackage(pkgName))
            listPackageURLs(pkgName);
    }
}

void InstallerEngineConsole::listPackageDescription(const QString &pkgName)
{
    init();
    Package *p = m_packageResources->getPackage(pkgName); 
    if (!p)
        return;
    printf("%s\n",qPrintable(p->notes()));
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
    Package *p = m_packageResources->getPackage(pkgName); 
    if (!p)
        return;
    printf("%s\n",qPrintable(p->categories().join("\n")));
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
        Package *p = m_packageResources->getPackage(pkgName);
        if (!p)
            continue;
        if (p->hasType(Package::BIN))
            p->downloadItem(Package::BIN);
        if (p->hasType(Package::LIB))
            p->downloadItem(Package::LIB);
        if (p->hasType(Package::DOC))
            p->downloadItem(Package::DOC);
        if (p->hasType(Package::SRC))
            p->downloadItem(Package::SRC);
    }
    return true;
}

bool InstallerEngineConsole::installPackages(const QStringList &packages,const QString &category)
{
    init();
    Q_FOREACH(const QString &pkgName, packages)
    {
        Package *p = m_packageResources->getPackage(pkgName);
        if (!p)
            continue;
        if (p->hasType(Package::BIN))
            p->installItem(m_installer,Package::BIN);
        if (p->hasType(Package::BIN))
            p->installItem(m_installer,Package::LIB);
        if (p->hasType(Package::BIN))
            p->installItem(m_installer,Package::DOC);
        if (p->hasType(Package::BIN))
            p->installItem(m_installer,Package::SRC);
    }
    return true;
}

