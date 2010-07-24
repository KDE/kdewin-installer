/****************************************************************************
**
** Copyright (C) 2006-2010 Ralf Habacker. All rights reserved.
** Copyright (C) 2006-2007 Christian Ehrlicher <ch.ehrlicher@gmx.de>
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

#include "misc.h"
#include "installercallconfig.h"

#include <QFileInfo>
#include <QCoreApplication>
#include <QStringList>

InstallerCallConfig::InstallerCallConfig(const QString &fileName) : isLoaded(false)
{
    QFileInfo fi(!fileName.isEmpty() ? fileName : QCoreApplication::applicationFilePath());
    installerBaseName = fi.completeBaseName();
    QStringList a = installerBaseName.split("-");
    if (a.size() >= 5)
    {
        key = a[0];

        packageName = a[1].isEmpty() || a[1] == "*" ? "" : a[1];

        compilerType = toCompilerType(a[2]);
        if (compilerType == Unspecified)
            compilerType = MSVC9;

        version = a[3].isEmpty() || a[3] == QLatin1String("*") ? "latest" : a[3]; 

        releaseType = toReleaseType(a[4]);
        if (releaseType == Unspecified)
            releaseType = Stable;

        if (a.size() == 5 || a[5].isEmpty())
            mirror = "www.winkde.org";
        else if (a[5] != QLatin1String("*"))
            mirror = a[5];
        isLoaded = true;
    }
}

InstallerCallConfig &InstallerCallConfig::instance()
{
    static InstallerCallConfig instance;
    return instance;
}
