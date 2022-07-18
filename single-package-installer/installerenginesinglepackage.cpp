/****************************************************************************
**
** Copyright (C) 2010 Ralf Habacker ralf.habacker@freenet.de
**
** This file is part of the KDE installer for windows
**
** SPDX-License-Identifier: LGPL-2.0-only
**
****************************************************************************/

#include "database.h"
#include "downloader.h"
#include "globalconfig.h"
#include "installer.h"
#include "installerenginesinglepackage.h"
#include "package.h"
#include "packagelist.h"
#include "downloader.h"
#include "unpacker.h"
#include "typehelper.h"

#include <QApplication>
#include <QtDebug>

InstallerEngineSinglePackage::InstallerEngineSinglePackage()
: InstallerEngine(0), done(false), m_withDevelopmentPackages(false)
{
}

void setWithInstallPackages(bool state);

void InstallerEngineSinglePackage::setRoot(const QString &root)
{
    InstallerEngine::setRoot(root);
    // make sure path exists at least when the first packages is installed
    QDir d(root);
    if (!d.exists())
        d.mkpath(root);
}

bool InstallerEngineSinglePackage::init()
{
    if (m_initFinished)
        return true;
    if (!initGlobalConfig())
        return false;
    //if (isInstallerV ersionOutdated())
    //    qWarning() << "Installer version outdated";
    return initPackages();
}

bool InstallerEngineSinglePackage::downloadPackages(QList<Package*> &packagesToInstall)
{
    Q_FOREACH(Package *p, packagesToInstall)
    {
        if (!p)
            continue;
        if (p->hasType(FileTypes::BIN))
            if (!p->downloadItem(FileTypes::BIN))
                return false;

        if (m_withDevelopmentPackages)
        {
            if (!p->hasType(FileTypes::LIB))
            {
                qWarning() << "package do not have a development part, could not download";
                continue;
            }
            if (!p->downloadItem(FileTypes::LIB))
                return false;
        }
    }
    return true;
}

bool InstallerEngineSinglePackage::installPackages(QList<Package*> &packagesToInstall)
{
    Q_FOREACH(Package *p, packagesToInstall)
    {
        if (!p)
            continue;
        if (p->hasType(FileTypes::BIN))
            if (!p->installItem(m_installer,FileTypes::BIN))
                return false;

        if (m_withDevelopmentPackages)
        {
            if (!p->hasType(FileTypes::LIB))
            {
                qWarning() << "package do not have development part, install may not be complete";
                continue;
            }
            if (!p->installItem(m_installer,FileTypes::LIB))
                return false;
        }
    }
    return true;
}
