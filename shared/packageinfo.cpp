/****************************************************************************
**
** Copyright (C) 2010 Ralf Habacker. All rights reserved.
** Copyright (C) 2010 Patrick von Reth patrick.vonreth@gmail.com> 
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
    const QStringList parts = name.split('-');
    // <name>-<version>
    if (parts.size() == 2 && parts[1][0].isNumber())
    {
        pkgName = parts[0];
        pkgVersion = parts[1];
        return true;
    }
    // <name>-<version>-<patchlevel>
    else if (parts.size() == 3 && parts[1][0].isNumber())
    {
        pkgName = parts[0];
        pkgVersion = parts[1] + '-' + parts[2];
        return true;
    }
    // <name1>-<name2>-<version>
    else if (parts.size() == 3 && parts[2][0].isNumber())
    {
        pkgName = parts[0] + '-' + parts[1];
        pkgVersion = parts[2];
        return true;
    }
    else
        return false;
}

PackageInfo PackageInfo::fromString(const QString &_name, const QString &version)
{
    PackageInfo result;
    QString name = _name;
    // check architecture
    if (name.contains("-x64"))
    {
        result.architecture = "x64";
    }
    else if (name.contains("-x86"))
    {
        result.architecture = "x86";
    }
    else
    {
        result.architecture = "x86";
    }

    // check type
    result.type = Package::NONE;
    if (name.contains("-bin"))
    {
        result.type = Package::BIN;
        name.replace("-bin","");
    }
    else if (name.contains("-lib"))
    {
        result.type = Package::LIB;
        name.replace("-lib","");
    }
    else if (name.contains("-doc"))
    {
        result.type = Package::DOC;
        name.replace("-doc","");
    }
    else if (name.contains("-src"))
    {
        result.type = Package::SRC;
        name.replace("-src","");
    }
    else if (name.contains("-dbg"))
    {
        result.type = Package::DBG;
        name.replace("-dbg","");
    }

    // version is given
    if (!version.isEmpty())
    {
        result.version = version;
        result.name = name;
        return result;
    }

    // version is empty
    const QStringList parts = name.split('-');
    // <name>-<version>
    if (parts.size() == 1)
    {
        result.name = name;
    }
    else if (parts.size() == 2 && parts[1][0].isNumber())
    {
        result.name = parts[0];
        result.version = parts[1];
    }
    // <name>-<version>-<patchlevel>
    else if (parts.size() == 3 && parts[1][0].isNumber())
    {
        result.name = parts[0];
        result.version = parts[1] + '-' + parts[2];
    }
    // <name1>-<name2>-<version>
    else if (parts.size() == 3 && parts[2][0].isNumber())
    {
        result.name = parts[0] + '-' + parts[1];
        result.version = parts[2];
    }
    else
        result.name = name;
    return result;
}

bool PackageInfo::fromFileName(const QString &fileName, QString &pkgName, QString &pkgVersion, QString &pkgType, QString &pkgFormat)
{
    QString baseName;

    // first remove ending
    int idx  = fileName.lastIndexOf('.');
    if(idx != -1)
    {
        pkgFormat = fileName.mid(idx + 1);
        baseName = fileName.left(idx).toLower();
        if(pkgFormat == "bz2") {
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

    QString work = baseName;
    pkgType = work.mid(work.lastIndexOf("-")+1);
    work.remove("-" + pkgType);

    
    QRegExp compilersRx("-(mingw|x86-mingw4|mingw4|msvc|vc90|vc100)-");
    QRegExp versionRx("-(\\d|\\w|\\.)*(-\\d*){0,1}$");

    QString compiler;
    int pos = compilersRx.indexIn(work);
    QStringList tmp;
    if(pos != -1){
        compiler = compilersRx.capturedTexts()[0];
        tmp =  work.split(compiler);
        compiler = compiler.remove(compiler.length() -1 ,1);
        pkgName = tmp[0] + compiler;
        pkgVersion = tmp[1];
    }else{
        pos = versionRx.indexIn(work);
        if(pos != -1){
            tmp = versionRx.capturedTexts();
        pkgVersion = tmp[0].remove(0,1);
        pkgName = work.remove("-" + tmp[0]);
        }else{
            qWarning() << "filename without version found" << baseName;
            return false;
        }
    }
    return true;
}


QString PackageInfo::versionFileName(const QString &pkgName, const QString &pkgVersion, const Package::Type type)
{
    return pkgName + "-" + pkgVersion + "-" + typeToString(type) +".ver";
}

QString PackageInfo::manifestFileName(const QString &pkgName, const QString &pkgVersion, const Package::Type type)
{
    return pkgName + "-" + pkgVersion + "-" + typeToString(type) +".mft";
}

QString PackageInfo::baseName(const QString &_name)
{
    QString name = _name;
    if (name.endsWith("-msvc"))
        name.remove("-msvc");
    if (name.endsWith("-vc90"))
        name.remove("-vc90");
    if (name.endsWith("-vc100"))
        name.remove("-vc100");
    if (name.endsWith("-mingw4"))
        name.remove("-mingw4");
    if (name.endsWith("-mingw"))
        name.remove("-mingw");
    if (isX64Windows())
        if (name.endsWith("-x64"))
            name.remove("-x64");
    if (name.endsWith("-x86"))
        name.remove("-x86");

    return name;
}

QStringList PackageInfo::endings()
{
    QStringList list;
    list << "msvc" << "vc90" << "vc100" << "mingw4" << "mingw";
    if(isX64Windows()) list << "x64";
    list << "x86";
    return list;
}
