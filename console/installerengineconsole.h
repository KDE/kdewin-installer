/****************************************************************************
**
** Copyright (C) 2005-2010 Ralf Habacker. All rights reserved.
**
** This file is part of the KDE installer for windows
**
** SPDX-License-Identifier: LGPL-2.0-only
**
****************************************************************************/

#ifndef INSTALLERENGINECONSOLE_H
#define INSTALLERENGINECONSOLE_H

#include  "installerengine.h"
#include "typehelper.h"

class QUrl;

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

    void queryPackageWhatRequiresAll(const QString &pkgName,QStringList &list);
    void queryPackageWhatRequiresAll(const QStringList &list);

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
    bool installPackages(const QUrl &file);
    bool removePackages(const QStringList &packages);

protected:
    bool init();
    void printPackage(Package *p);
    void printPackageURLs(Package *p);

    int m_installedPackages;
private:
    bool done;
};

#endif
