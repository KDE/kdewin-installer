/****************************************************************************
**
** Copyright (C) 2011 Patrick von Reth <patrick.vonreth@gmail.com>
** Copyright (C) 2011 Ralf Habacker <ralf.habacker@freenet.de>
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

#include "typehelper.h"
#include "misc.h"

#include <QMap>
#include <QRegExp>

CompilerTypes supportedCompilers(CompilerTypes::supportedCompiler);
CompilerTypes allCompilers(CompilerTypes::allCompiler);

//ReleaseType toReleaseType(const QString &_type)
//{
//    QString type(_type.toLower());
//    if (type == "unstable")
//        return Unstable;
//    else if (type== "stable")
//        return Stable;
//    else if (type == "nightly")
//        return Nightly;
//    else 
//        return RTUnspecified;
//}
//
//const QString toString(ReleaseType type)
//{
//    if (type == Stable)
//        return "stable";
//    else if (type == Unstable)
//	    return "unstable";
//    else if (type == Nightly)
//	    return "nightly";
//    else
//	    return "unknown";
//}

CompilerTypes::CompilerTypes(Scope scope) :  m_containsRegExp(0), m_endsRegExp(0)
{
    // see doc/format-specifications.txt for details
    m_compiler
#ifdef BUILD_WITH_DEPRECATED
        << Compiler(MSVC9,  "vc90", "MSVC 2008 32bit (deprecated)")
        << Compiler(MSVC11,  "mingw4", "MINGW4 32bit (deprecated)")
#endif
        << Compiler(MSVC10,  "vc100", "MSVC 2010 32bit")
        << Compiler(MSVC11,  "vc110", "MSVC 2011 32bit")
        << Compiler(MSVC12,  "vc120", "MSVC 2012 32bit")
        << Compiler(MSVC14,  "vc140", "MSVC 2014 32bit")
        << Compiler(MSVC141, "vc141", "MSVC 2017 32bit")
        << Compiler(MSVC142, "vc142", "MSVC 2019 32bit")
        << Compiler(MinGW4_W32, "x86-mingw4", "MinGW4 32bit")
        ;

    if (scope == allCompiler || isX64Windows())
    {
        m_compiler
            << Compiler(MSVC10_X64, "x64-vc100", "MSVC 2010 64bit")
            << Compiler(MinGW4_W64, "x64-mingw4", "MinGW4 64bit")
            ;
    }
    m_containsRegExp = new QRegExp("("+values().join("|")+")");
    m_endsRegExp = new QRegExp("("+values().join("|")+")$");
 }

CompilerTypes::~CompilerTypes()
{
    delete m_containsRegExp;
    delete m_endsRegExp;
}

bool CompilerTypes::contains(Type type)
{
    foreach(const Compiler &c, m_compiler) {
        if (c.type == type)
            return true;
    }
    return false;
}

CompilerTypes::Type CompilerTypes::fromString(const QString &type)
{
    QString name = type.toLower();
    foreach(const Compiler &c, m_compiler) {
        if (c.name == name)
            return c.type;
    }
    return CompilerTypes::Unspecified;
}

const QString CompilerTypes::toString(Type type)
{
    foreach(const Compiler &c, m_compiler) {
        if (c.type == type)
            return c.name;
    }
    return QString();
}

const QStringList CompilerTypes::values()
{
    QStringList result;
    foreach(const Compiler &c, m_compiler)
        result << c.name;
    return result;
}

const QString CompilerTypes::description(Type type)
{
    foreach(const Compiler &c, m_compiler) {
        if (c.type == type)
            return c.description;
    }
    return QString();
}

QRegExp &CompilerTypes::regex()
{
    return *m_containsRegExp;
}

QRegExp &CompilerTypes::endswith()
{
    return *m_endsRegExp;
}

FileTypes::Type FileTypes::fromString(const QString &type)
{
    static QMap<QString,FileTypes::Type> typeMap;
    if(typeMap.isEmpty()){
        typeMap.insert("bin",BIN);
        typeMap.insert("lib",LIB);
        typeMap.insert("doc",DOC);
        typeMap.insert("src",SRC);
        typeMap.insert("dbg",DBG);
        typeMap.insert("meta",META);
    }
    QString _type = type.toLower();
    return typeMap.contains(_type)?typeMap.value(_type):NONE;
}

const QString FileTypes::toString(Type type)
{
  switch(type) {
    case BIN:    return "bin";
    case LIB:    return "lib";
    case DOC:    return "doc";
    case SRC:    return "src";
    case DBG:    return "dbg";
    case META:   return "meta";
    case ALL:    return "all";
    default:     return "unknown";
    }
}

const QStringList FileTypes::values()
{
    // see doc/format-specifications.txt for details
    static QStringList list;
    if(list.isEmpty()){
        list<<"bin"<<"lib"<<"doc"<<"src"<<"dbg";
    }
    return list;
}

QRegExp &FileTypes::regex()
{
    static QRegExp compilersRx("("+FileTypes::values().join("|")+")");
    return compilersRx;
}

QRegExp &FileTypes::endswith()
{
    static QRegExp compilersRx("("+FileTypes::values().join("|")+")$");
    return compilersRx;
}