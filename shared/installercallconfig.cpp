/****************************************************************************
**
** Copyright (C) 2006-2011 Ralf Habacker. All rights reserved.
** Copyright (C) 2006-2007 Christian Ehrlicher <ch.ehrlicher@gmx.de>
**
** This file is part of the KDE installer for windows
**
** SPDX-License-Identifier: LGPL-2.0-only
**
****************************************************************************/

#include "misc.h"
#include "installercallconfig.h"
#include "typehelper.h"

#include <QFileInfo>
#include <QCoreApplication>
#include <QStringList>

InstallerCallConfig::InstallerCallConfig(const QString &fileName)
    : hasReleaseType(false),
      hasVersion(false),
      hasMirror(false),
      hasSDK(false),
      isLoaded(false)
{
    QFileInfo fi(!fileName.isEmpty() ? fileName : QCoreApplication::applicationFilePath());
    installerBaseName = fi.completeBaseName();
    if (installerBaseName.contains("-sdk"))
    {
        installerBaseName.remove("-sdk");
        hasSDK = true;
    }
    QStringList a = installerBaseName.split("-");
    releaseType = Stable;
    compilerType = CompilerTypes::MSVC10;
    version = "latest";
    mirror = "download.kde.org";

    // setup-<packagename>.exe
    if (a.size() >= 2)
    {
        key = a[0];
        packageName = a[1].isEmpty() || a[1] == "*" ? "" : a[1];
        packageName.replace('_','-');
    }

    // setup-<packagename>-<compiler>.exe
    if (a.size() >= 3)
    {
        compilerType = supportedCompilers.fromString(a[2]);
        if (compilerType == CompilerTypes::Unspecified)
            compilerType = CompilerTypes::MSVC10;
    }

    // setup-<packagename>-<compiler>-<releasetype>.exe
    if (a.size() >= 4)
    {
        if (!a[3].isEmpty() && a[3] != QLatin1String("*"))
        {
            releaseType = toReleaseType(a[3]);
            if (releaseType == Undefined)
                releaseType = Stable;
            else
                hasReleaseType = true;
        }
        else
            releaseType = Stable;
    }

    // setup-<packagename>-<compiler>-<releasetype>-<version>.exe
    if (a.size() >= 5)
    {
        if (!a[4].isEmpty() && a[4] != QLatin1String("*"))
        {
            version = a[4];
            hasVersion = true;
        }
    }

    // setup-<packagename>-<compiler>-<releasetype>-<version>-<mirror>.exe
    if (a.size() >= 6)
    {
        if (!a[5].isEmpty() && a[5] != QLatin1String("*"))
        {
            mirror = a[5];
            hasMirror = true;
        }
    }
    isLoaded = true;
}

InstallerCallConfig &InstallerCallConfig::instance()
{
    static InstallerCallConfig instance;
    return instance;
}
