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
: InstallerEngine(0)
{
}

bool InstallerEngineConsole::init()
{
    initGlobalConfig();
    if (isInstallerVersionOutdated())   
        qWarning() << "Installer version outdated";
    return initPackages();
}

/**
 query package from installed package database 
 @param pkgName 
*/ 
void InstallerEngineConsole::queryPackages(const QString &pkgName, bool listFiles)
{
    if (pkgName.isEmpty()) {
        m_database->listPackages();
        return;
    }
    if (listFiles) {
        Q_FOREACH(const QString &file, m_database->getPackageFiles(pkgName,Package::BIN))
            printf("BIN: %s\n", qPrintable(file));
        Q_FOREACH(const QString &file, m_database->getPackageFiles(pkgName,Package::LIB))
            printf("LIB: %s\n", qPrintable(file));
        Q_FOREACH(const QString &file, m_database->getPackageFiles(pkgName,Package::DOC))
            printf("DOC: %s\n", qPrintable(file));
        Q_FOREACH(const QString &file, m_database->getPackageFiles(pkgName,Package::SRC))
            printf("SRC: %s\n", qPrintable(file));
    }        
    else {
        Package *pkg = m_database->getPackage(pkgName);
        if (!pkg)
            return; 

        printf("%s\n", qPrintable(pkg->toString(true)));
    }
}

void InstallerEngineConsole::listPackages(const QString &title)
{
    QStringList list = m_packageResources->listPackages();
    Q_FOREACH(const QString &package, list)
        printf("%s\n",qPrintable(package)); 
}

bool InstallerEngineConsole::downloadPackages(const QStringList &packages, const QString &category)
{
   Q_FOREACH(const QString &pkgName, packages)
   {
        Package *pkg = m_packageResources->getPackage(pkgName);
        if (!pkg)
            continue;
        pkg->downloadItem(Package::BIN);
        pkg->downloadItem(Package::LIB);
        pkg->downloadItem(Package::DOC);
        pkg->downloadItem(Package::SRC);
   }
   return true;
}

bool InstallerEngineConsole::installPackages(const QStringList &packages,const QString &category)
{
   Q_FOREACH(const QString &pkgName, packages)
   {
        Package *pkg = m_packageResources->getPackage(pkgName);
        if (!pkg)
            continue;
        pkg->installItem(m_installer,Package::BIN);
        pkg->installItem(m_installer,Package::LIB);
        pkg->installItem(m_installer,Package::DOC);
        pkg->installItem(m_installer,Package::SRC);
   }
   return true;
}

