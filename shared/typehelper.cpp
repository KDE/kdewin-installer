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
#include <QStringList>

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

QList<CompilerTypes::Type> CompilerTypes::m_types;
QStringList CompilerTypes::m_typeStrings;
QStringList CompilerTypes::m_descriptions;

void CompilerTypes::init()
{
    if (m_types.size() == 0)
    {
#ifdef BUILD_WITH_DEPRECATED
        m_types << MSVC9 << MinGW4;
        m_typeStrings << "vc90" << "mingw4";
        m_descriptions << "MSVC 2008 32bit (deprecated)" << "MinGW4 (deprecated)";
#endif
        m_types << MSVC10 << MinGW4_W32;
        m_typeStrings << "vc100" << "x86-mingw4";
        m_descriptions << "MSVC 2010 32bit" << "MinGW4 32bit";
        if (isX64Windows())
        {
        // @todo: enable msvc x64 builds if we ever set them up
#ifdef MSVC_X64
            m_types << MSVC10_X64;
            m_typeStrings << "x64-vc100";
            m_descriptions << "MSVC 2010 64bit";
#endif
            m_types << MinGW4_W64;
            m_typeStrings << "x64-mingw4";
            m_descriptions << "MinGW4 64bit";
        }
     }
 }

bool CompilerTypes::contains(Type type)
{
    init();
    return m_types.contains(type);
}

CompilerTypes::Type CompilerTypes::fromString(const QString &type)
{
    init();
    int i = m_typeStrings.indexOf(type.toLower());
    return i != -1 ? m_types[i] : CompilerTypes::Unspecified;
}

const QString CompilerTypes::toString(Type type)
{
    init();
    int i = m_types.indexOf(type);
    return i != -1 ? m_typeStrings[i] : QString();
}

const QStringList &CompilerTypes::values()
{
    init();
    return m_typeStrings;
}

const QString CompilerTypes::description(Type type)
{
    init();
    int i = m_types.indexOf(type);
    return i != -1 ? m_descriptions[i] : QString();
}

QRegExp CompilerTypes::regex(){
    static QRegExp compilersRx("("+values().join("|")+")");
    return compilersRx;
}

QRegExp CompilerTypes::endswith(){
    static QRegExp compilersRx("("+values().join("|")+")$");
    return compilersRx;
}


ArchitectureTypes::Type ArchitectureTypes::fromString(const QString &_type)
{
    QString arch = _type.toLower();
    if(arch == "x86")
        return x86;
    else if(arch == "x64")
        return x64;
    else 
        return Unspecified;
}

const QString ArchitectureTypes::toString(ArchitectureTypes::Type arch)
{
    switch(arch){
        case x86:
            return "x86";
        case x64:
            return "x64";
        default:
            return "";
    }
}

const QStringList ArchitectureTypes::values()
{
    static QStringList list;
    if(list.isEmpty()){
        list  << "x86" << "x64" ;
    }
    return list;
}

QRegExp ArchitectureTypes::regex()
{
    static QRegExp architecturesRx("("+ArchitectureTypes::values().join("|")+")");
    return architecturesRx;
}

QRegExp ArchitectureTypes::endswith()
{
    static QRegExp architecturesRx("("+ArchitectureTypes::values().join("|")+")$");
    return architecturesRx;
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
    static QStringList list;
    if(list.isEmpty()){
        list<<"bin"<<"lib"<<"doc"<<"src"<<"dbg";
    }
    return list;
}

QRegExp FileTypes::regex()
{
    static QRegExp compilersRx("("+FileTypes::values().join("|")+")");
    return compilersRx;
}

QRegExp FileTypes::endswith()
{
    static QRegExp compilersRx("("+FileTypes::values().join("|")+")$");
    return compilersRx;
}