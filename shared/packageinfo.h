/****************************************************************************
**
** Copyright (C) 2010-2011 Ralf Habacker. All rights reserved.
** Copyright (C) 2011 Patrick von Reth <patrick.vonreth@gmail.com>
**
** This file is part of the KDE installer for windows
**
** SPDX-License-Identifier: LGPL-2.0-only
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
    PackageInfo(const QString &fileName);

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
     check if package type is present
     @return true if type is present
    */
    bool hasType()
    {
        return type != FileTypes::NONE;
    }

    /**
     check if object has valid content
     @return true if content is valid
    */
    bool isValid() { return m_isValid; }

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

    /**
     fetch package name from string
     @param package name with optional compiler
     @return pur package name without compiler
    */
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

    bool m_isValid;
};

#endif
