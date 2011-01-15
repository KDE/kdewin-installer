/****************************************************************************
**
** Copyright (C) 2010 Ralf Habacker. All rights reserved.
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

#include "packageinfo.h"

bool PackageInfo::fromString(const QString &name, QString &pkgName, QString &pkgVersion)
{
    QString work(name);
    
    //something like "-(mingw|mingw4|msvc|vc90|vc100)-
    QRegExp compilersRx("-("+PackageInfo::compilers().join("|")+")-");
    //alow only number and points, as patchlvl only numbers
    QRegExp versionRx("-(\\w|\\d|\\.|_|\\+)*(-\\d*){0,1}$");


    if(compilersRx.indexIn(work) != -1){
        QString compiler = compilersRx.capturedTexts()[0];
        QStringList tmp =  work.split(compiler);
        compiler = compiler.remove(compiler.length() -1 ,1);
        pkgName = tmp[0] + compiler;
        pkgVersion = tmp[1];
    }else{
        if(versionRx.indexIn(work) != -1){
            QStringList tmp = versionRx.capturedTexts();
            pkgVersion = tmp[0].remove(0,1);
            pkgName = work.remove("-" + tmp[0]);
        }else{
            qWarning() << "filename without version found" << name;
            return false;
        }
    }
    return true;
}

PackageInfo PackageInfo::fromString(const QString &_name, const QString &version)
{
    PackageInfo result;
    if(_name.isEmpty())
        return result;

    QString name = _name;

    // check architecture
    QRegExp archRx("(x86|x64)");    
    if( archRx.indexIn(name) != -1){
        result.architecture = archRx.capturedTexts()[0];
    }else{
        result.architecture = "x86";
    }

    // check type
    QRegExp typeRx("-(" + PackageInfo::types().join("|") + ")$");
    if(typeRx.indexIn(name) != -1){
        result.type = stringToType(typeRx.capturedTexts()[0].remove(0,1));
        name.remove(typeRx.capturedTexts()[0]);
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
    QRegExp typeRx("-(" + PackageInfo::types().join("|") + ")$");
    if(typeRx.indexIn(work) == -1){
        qWarning() << "filename without type found" << baseName;
        return false;
    }

    pkgType = typeRx.capturedTexts()[0];
    work.remove(pkgType);
    pkgType.remove(0,1);

    return fromString(work, pkgName, pkgVersion);
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
    name.remove(QRegExp("-("+PackageInfo::compilers().join("|")+")$"));
    return name.remove(QRegExp("-(x64|x86)$"));
}

QStringList PackageInfo::endings()
{
    QStringList list;
    list << PackageInfo::compilers();
    if(isX64Windows()) list << "x64";
    list << "x86";
    return list;
}

QStringList PackageInfo::compilers()
{
    QStringList list;
    list << "msvc" << "vc90" << "vc100" <<"mingw4" << "mingw";
    return list;
}

QStringList PackageInfo::types()
{
    QStringList list;
    list<<"bin"<<"lib"<<"doc"<<"src"<<"dbg";
    return list;
}
