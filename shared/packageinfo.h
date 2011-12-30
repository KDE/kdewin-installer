/****************************************************************************
**
** Copyright (C) 2010-2011 Ralf Habacker. All rights reserved.
** Copyright (C) 2011 Patrick von Reth patrick.vonreth@gmail.com>
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
#ifndef PACKAGEINFO_H
#define PACKAGEINFO_H

#include "package.h"
#include "typehelper.h"

class PackageInfo
{
public:
    QString name;
    QString architecture;
    FileTypes::Type type;
    QString version;


    /// separate package name and version from a string
    static bool fromString(const QString &astring, QString &pkgName, QString &pkgVersion);

    /// create PackageInfo instance from string containing package name and version
    static PackageInfo fromString(const QString &name, const QString &version=QString());

    /// separate package name, version, type and file format from a filename
    static bool fromFileName(const QString &fileName, QString &pkgName, QString &pkgVersion, QString &pkgType, QString &pkgFormat);

    /// return package name from string without optional compiler
    static QString baseName(const QString &_name);

    // returns version file name of package item e.g. xyz-1.2.3-bin.ver
    static QString versionFileName(const QString &pkgName, const QString &pkgVersion, const FileTypes::Type type);

    // returns manifest file name of package item e.g. xyz-1.2.3-bin.mft
    static QString manifestFileName(const QString &pkgName, const QString &pkgVersion, const FileTypes::Type type);

    /// return the possible package endings
    static QStringList endings();
};

#endif
