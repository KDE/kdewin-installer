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

//#define PACKAGEINFO_DEBUG

PackageInfo::PackageInfo() : type(FileTypes::NONE), m_isValid(false)
{
}

PackageInfo::PackageInfo(const QString &fileName)
{
    m_isValid = fromFileName(fileName, name, compiler, version, typeString, format);
    type = FileTypes::fromString(typeString);
}

bool PackageInfo::fromString(const QString &name, QString &pkgName, QString &pkgVersion)
{
    QString work(name);

    QString compiler;
    extractCompiler(work,compiler);
    if (!extractVersion(work,pkgVersion))
    {
        qWarning() << "filename without version found" << name;
        return false;
    }

    extractName(work,pkgName);
    pkgName += "-" + compiler;
    return true;
}

PackageInfo PackageInfo::fromString(const QString &_name, const QString &version)
{
    PackageInfo result;
    if (_name.isEmpty())
        return result;

    QString name = _name;

    extractCompiler(name, result.compiler);
    extractType(name, result.typeString);
    result.type = FileTypes::fromString(result.typeString);


    // version is given
    if (!version.isEmpty())
    {
        result.version = version;
    }
    else
    {
        extractVersion(name, result.version);
    }
    result.name = name;
    result.m_isValid = true;
    return result;
}

bool PackageInfo::fromFileName(const QString &fileName, QString &pkgName, QString &pkgCompiler, QString &pkgVersion, QString &pkgType, QString &pkgFormat)
{
    if (fileName.isEmpty())
        return false;

    QString work(fileName);
    extractFormat(work,pkgFormat);
    // we do not support case sensitive
    work = work.toLower();

    extractCompiler(work,pkgCompiler);

    if (!extractType(work,pkgType))
    {
        qWarning() << "filename without type found" << fileName;
        return false;
    }

    extractVersion(work, pkgVersion);
    extractName(work, pkgName);
    return true;
}

PackageInfo PackageInfo::fromFileName(const QString &fileName)
{
    PackageInfo info;
    if (!PackageInfo::fromFileName(fileName, info.name, info.compiler, info.version, info.typeString, info.format))
        return PackageInfo();
    info.type = FileTypes::fromString(info.typeString);
    info.m_isValid = true;
    return info;
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
    QString compiler;
    extractCompiler(name, compiler);
    return name;
}

bool PackageInfo::extractFormat(QString &s, QString &result)
{
#ifdef PACKAGEINFO_DEBUG
    QString tmp(s);
#endif
    // first remove ending
    int idx  = s.lastIndexOf('.');
    if (idx != -1)
    {
        result = s.mid(idx + 1);
        s = s.left(idx).toLower();
        if (result == "bz2")
        {
            int idx2 = s.lastIndexOf('.', idx - 1);
            result = s.mid(idx2 + 1);// @TODO return complete format + '.' + result;
            idx = idx2;
        }
        s = s.left(idx);
    }
    else
    {
        result = "unknown";
    }
#ifdef PACKAGEINFO_DEBUG
    qDebug() << "in:" << tmp << "out:" << s << "extracted" << result;
#endif
    return true;
}

bool PackageInfo::extractType(QString &s, QString &result)
{
#ifdef PACKAGEINFO_DEBUG
    QString tmp(s);
#endif
    QRegExp typeRx = FileTypes::endswith();
    if (typeRx.indexIn(s) == -1)
    {
        return false;
    }
    result = typeRx.capturedTexts()[0];
    s.remove("-" + result);

#ifdef PACKAGEINFO_DEBUG
    qDebug() << "in:" << tmp << "out:" << s << "extracted" << result;
#endif
    return true;
}

bool PackageInfo::extractCompiler(QString &s, QString &result)
{
#ifdef PACKAGEINFO_DEBUG
    QString tmp(s);
#endif
    QRegExp &compilersRx = allCompilers.regex();

    if (compilersRx.indexIn(s) == -1)
    {
        return false;
    }
    result = compilersRx.capturedTexts()[0];
    s.remove("-" + result);

#ifdef PACKAGEINFO_DEBUG
    qDebug() << "in:" << tmp << "out:" << s << "extracted" << result;
#endif
    return true;
}

bool PackageInfo::extractVersion(QString &s, QString &result)
{
#ifdef PACKAGEINFO_DEBUG
    QString tmp(s);
#endif
    // see doc/format-specifications.txt for details
    // "-\\d(\\d[0-9._+]+-[a-zA-Z0-9._]+$"
    QRegExp versionRx("-(gitHEAD|svnHEAD|\\d(\\w|\\d|\\.|_|-|\\+))$");

    if (versionRx.indexIn(s) == -1)
    {
        return false;
    }
    result = versionRx.capturedTexts()[0].remove(0,1);
    s.remove("-" + result);

#ifdef PACKAGEINFO_DEBUG
    qDebug() << "in:" << tmp << "out:" << s << "extracted" << result;
#endif
    return true;
}

bool PackageInfo::extractName(QString &s, QString &result)
{
#ifdef PACKAGEINFO_DEBUG
    QString tmp(s);
#endif
    result = s;
    s = "";

#ifdef PACKAGEINFO_DEBUG
    qDebug() << "in:" << tmp << "out:" << s << "extracted" << result;
#endif
    return true;
}
