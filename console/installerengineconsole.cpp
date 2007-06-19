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

InstallerEngineConsole::InstallerEngineConsole(DownloaderProgress *progressBar,InstallerProgress *instProgressBar)
: InstallerEngine(progressBar,instProgressBar)
{
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
    if (listFiles)
        m_database->listPackageFiles(pkgName);
    else {
        Package *pkg = m_database->getPackage(pkgName);
        if (!pkg)
            return; 

        printf("%s\n", pkg->toString(true).toAscii().data());
    }
}

void InstallerEngineConsole::listPackages(const QString &title)
{
    QList <PackageList *>::iterator k;
    for (k = m_packageListList.begin(); k != m_packageListList.end(); ++k)
    {
       qDebug() << (*k)->Name();
       (*k)->listPackages(title);
   }
}

bool InstallerEngineConsole::downloadPackages(const QStringList &packages, const QString &category)
{
    QList<Site*>::ConstIterator s = m_globalConfig->sites()->constBegin();
    for ( ; s != m_globalConfig->sites()->constEnd(); s++)
    {
       qDebug() << (*s)->name();
       PackageList *packageList = getPackageListByName((*s)->name());
       if (!packageList)
       {
           qDebug() << __FUNCTION__ << " packagelist for " << (*s)->name() << " not found";
           continue;
       }
       foreach(QString pkgName, packages)
       {
            Package *pkg = packageList->getPackage(pkgName);
            if (!pkg)
                continue;
            pkg->downloadItem(m_downloader, Package::BIN);
            pkg->downloadItem(m_downloader, Package::LIB);
            pkg->downloadItem(m_downloader, Package::DOC);
            pkg->downloadItem(m_downloader, Package::SRC);
       }
   }
   return true;
}

bool InstallerEngineConsole::installPackages(const QStringList &packages,const QString &category)
{
    QList<Site*>::ConstIterator s = m_globalConfig->sites()->constBegin();
    for ( ; s != m_globalConfig->sites()->constEnd(); s++)
    {
       qDebug() << (*s)->name();
       PackageList *packageList = getPackageListByName((*s)->name());
       if (!packageList)
       {
           qDebug() << __FUNCTION__ << " packagelist for " << (*s)->name() << " not found";
           continue;
       }
       foreach(QString pkgName, packages)
       {
            Package *pkg = packageList->getPackage(pkgName);
            if (!pkg)
                continue;
            pkg->installItem(m_installer, Package::BIN);
            pkg->installItem(m_installer, Package::LIB);
            pkg->installItem(m_installer, Package::DOC);
            pkg->installItem(m_installer, Package::SRC);
       }
   }
   return true;
}

