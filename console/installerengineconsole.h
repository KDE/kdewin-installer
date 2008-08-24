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

#ifndef INSTALLERENGINECONSOLE_H
#define INSTALLERENGINECONSOLE_H

#include  "installerengine.h"

class InstallerEngineConsole : public InstallerEngine
{
public:
    InstallerEngineConsole();

    // installed packages
    void queryPackage();
    void queryPackage(const QString &pkgName);
    void queryPackage(const QStringList &pkglist);

    void queryPackageListFiles(const QString &pkgName);
    void queryPackageListFiles(const QStringList &list);

    void queryPackageWhatRequires(const QString &pkgName);
    void queryPackageWhatRequires(const QStringList &list);

    // available packages
    void listPackage();
    void listPackage(const QString &pkgName);
    void listPackage(const QStringList &list);

    void listPackageURLs();
    void listPackageURLs(const QString &pkgName);
    void listPackageURLs(const QStringList &list);

    void listPackageDescription(const QString &pkgName);
    void listPackageDescription(const QStringList &list);

    void listPackageCategories(const QString &pkgName);
    void listPackageCategories(const QStringList &list);

    bool downloadPackages(const QStringList &packages, const QString &category=QString());
    bool installPackages(const QStringList &packages, const QString &category=QString());

    void initLocal();
protected:
    bool init();
    void printPackage(Package *p);
    void printPackageURLs(Package *p);

private:
    bool done;
};

#endif
