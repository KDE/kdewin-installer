/****************************************************************************
**
** Copyright (C) 2010-2011 Ralf Habacker <ralf.habacker@freenet.de>
** Copyright (C) 2011 Patrick von Reth <patrick.vonreth@gmail.com>
** All rights reserved.
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

#include "packageinfo.h"

bool PackageInfo::fromString(const QString &name, QString &pkgName, QString &pkgVersion)
{
    QString work(name);

    //something like "-(mingw|mingw4|msvc|vc90|vc100)-
    QRegExp compilersRx = CompilerTypes::regex();
    //alow only number and points, as patchlvl only numbers
    QRegExp versionRx("-(\\w|\\d|\\.|_|\\+)*(-\\d*){0,1}$");

    if (compilersRx.indexIn(work) != -1)
    {
        QString compiler = compilersRx.capturedTexts()[0];
        QStringList tmp =  work.split(compiler);
        pkgName = tmp[0] + compiler;
        pkgVersion = tmp[1].remove(0,1);
    }
    else
    {
        if (versionRx.indexIn(work) != -1)
        {
            QStringList tmp = versionRx.capturedTexts();
            pkgVersion = tmp[0].remove(0,1);
            pkgName = work.remove("-" + tmp[0]);
        }
        else
        {
            qWarning() << "filename without version found" << name;
            return false;
        }
    }
    return true;
}

PackageInfo PackageInfo::fromString(const QString &_name, const QString &version)
{
    PackageInfo result;
    if (_name.isEmpty())
        return result;

    QString name = _name;

    // check architecture
    QRegExp archRx = ArchitectureTypes::endswith();
    if ( archRx.indexIn(name) != -1)
    {
        result.architecture = archRx.capturedTexts()[0];
    }
    else
    {
        result.architecture = "x86";
    }

    // check type
    QRegExp typeRx = FileTypes::endswith();
    if (typeRx.indexIn(name) != -1)
    {
        result.type = FileTypes::fromString(typeRx.capturedTexts()[0]);
        name.remove("-" + result.type);
    }

    // version is given
    if (!version.isEmpty())
    {
        result.version = version;
        result.name = name;
        return result;
    }
    result.name = name;
    return result;
}

bool PackageInfo::fromFileName(const QString &fileName, QString &pkgName, QString &pkgVersion, QString &pkgType, QString &pkgFormat)
{
    QString baseName;

    // first remove ending
    int idx  = fileName.lastIndexOf('.');
    if (idx != -1)
    {
        pkgFormat = fileName.mid(idx + 1);
        baseName = fileName.left(idx).toLower();
        if (pkgFormat == "bz2")
        {
            int idx2 = fileName.lastIndexOf('.', idx - 1);
            pkgFormat = fileName.mid(idx2 + 1);
            idx = idx2;
        }
        baseName = fileName.left(idx);
    }
    else
    {
        pkgFormat = "unknown";
        baseName = fileName.toLower();
    }


    QRegExp typeRx = FileTypes::endswith();
    if (typeRx.indexIn(baseName) == -1)
    {
        qWarning() << "filename without type found" << baseName;
        return false;
    }

    pkgType = typeRx.capturedTexts()[0];
    baseName.remove("-" + pkgType);

    return fromString(baseName, pkgName, pkgVersion);
}


QString PackageInfo::versionFileName(const QString &pkgName, const QString &pkgVersion, const FileTypes::Type type)
{
    return pkgName + "-" + pkgVersion + "-" + FileTypes::toString(type) +".ver";
}

QString PackageInfo::manifestFileName(const QString &pkgName, const QString &pkgVersion, const FileTypes::Type type)
{
    return pkgName + "-" + pkgVersion + "-" + FileTypes::toString(type) +".mft";
}

QString PackageInfo::baseName(const QString &_name)
{
    QString name = _name;
    name.remove(CompilerTypes::endswith());
    name.remove(ArchitectureTypes::endswith());
    //remove last "-" at the end
    static QRegExp e("-*$");
    return name.remove(e);

}

QStringList PackageInfo::endings()
{
    QStringList list;
    list << CompilerTypes::values();
    if (isX64Windows())
        list << "x64";
    list << "x86";
    return list;
}

