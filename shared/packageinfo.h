/****************************************************************************
**
** Copyright (C) 2010-2011 Ralf Habacker. All rights reserved.
** Copyright (C) 2011 Patrick von Reth <patrick.vonreth@gmail.com>
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
    QString name;         ///< package name
    QString compiler;     ///< compiler
    QString typeString;   ///< package type as string
    FileTypes::Type type; ///< package type
    QString version;      ///< package version
    QString format;       ///< package format eg tar.bz2, zip

    PackageInfo();

    /**
     check package type
     @param _type type to check
     @return true if object is _type
    */
    bool isType(FileTypes::Type _type)
    {
        return type == _type;
    }

    /**
     check package type
     @param _type type to check
     @return true if obkect is _type or type is empty
    */
    bool isTypeOrEmpty(FileTypes::Type _type)
    {
        return isType(_type) || type == FileTypes::NONE;
    }

    /**
     separate package name and version from a string
     @note This method returns the package name combined with the compiler
    */
    QT_DEPRECATED static bool fromString(const QString &astring, QString &pkgName, QString &pkgVersion);

    /// create PackageInfo instance from string containing package name and version
    static PackageInfo fromString(const QString &name, const QString &version=QString());

    /// @TODO make deprecated
    /// separate package name, version, type and file format from a filename
    static bool fromFileName(const QString &fileName, QString &pkgName, QString &pkgCompiler, QString &pkgVersion, QString &pkgType, QString &pkgFormat);

    /// separate package name, version, type and file format from a filename
    static PackageInfo fromFileName(const QString &fileName);

    /// return package name from string without optional compiler
    static QString baseName(const QString &_name);

    // returns version file name of package item e.g. xyz-1.2.3-bin.ver
    static QString versionFileName(const QString &pkgName, const QString &pkgVersion, const FileTypes::Type type);

    // returns manifest file name of package item e.g. xyz-1.2.3-bin.mft
    static QString manifestFileName(const QString &pkgName, const QString &pkgVersion, const FileTypes::Type type);

protected:
    /// extract file format from string
    static bool extractFormat(QString &s, QString &result);
    /// extract package type from string
    static bool extractType(QString &s, QString &result);
    /// extract compiler from string
    static bool extractCompiler(QString &s, QString &result);
    /// extract version from string
    static bool extractVersion(QString &s, QString &result);
    /// extract package name from string
    static bool extractName(QString &s, QString &result);

};

#endif
