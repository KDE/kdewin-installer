/****************************************************************************
**
** Copyright (C) 2011 Patrick von Reth patrick.vonreth@gmail.com> . All rights reserved.
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

CompilerTypes::Type CompilerTypes::fromString(const QString &_type)
{
    static QMap<QString,Type> compilerMap;
    if(compilerMap.isEmpty()){
        compilerMap.insert("vc90",MSVC9);
        compilerMap.insert("vc100",MSVC10);
        compilerMap.insert("x64-vc100",MSVC10_X64);
        compilerMap.insert("mingw4",MinGW4);
        compilerMap.insert("x86-mingw4",MinGW4_W32);
        compilerMap.insert("x64-mingw4",MinGW4_W64);
    }
    QString t = _type.toLower();
    return compilerMap.contains(t)?compilerMap.value(t):CompilerTypes::Unspecified;
}

const QString CompilerTypes::toString(Type type)
{
    switch(type){
        case MSVC9:
            return "vc90";
        case MSVC10:
            return "vc100";
        case MSVC10_X64:
            return "x64-vc100";
        case MinGW4:
            return "mingw4";
        case MinGW4_W32:
            return "x86-mingw4";
        case MinGW4_W64:
            return "x64-mingw4";
        default:
            return "";
    }
}

const QStringList CompilerTypes::values()
{
    static QStringList list;
    if(list.isEmpty()){
        list  << "vc90" << "vc100" << "x64-vc100" << "x86-mingw4" << "x64-mingw4" << "mingw4" ;
    }
    return list;
}

QRegExp CompilerTypes::regex(){
    static QRegExp compilersRx("("+CompilerTypes::values().join("|")+")");
    return compilersRx;
}

QRegExp CompilerTypes::endswith(){
    static QRegExp compilersRx("("+CompilerTypes::values().join("|")+")$");
    return compilersRx;
}

ArchitectureTypes::ArchitectureType ArchitectureTypes::fromString(const QString &_type)
{
    QString arch = _type.toLower();
    if(arch == "x86")
        return x86;
    else if(arch == "x64")
        return x64;
    else 
        return Unspecified;
}

const QString ArchitectureTypes::toString(ArchitectureTypes::ArchitectureType arch)
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

QRegExp ArchitectureTypes::regex(){
    static QRegExp architecturesRx("("+ArchitectureTypes::values().join("|")+")");
    return architecturesRx;
}

QRegExp ArchitectureTypes::endswith(){
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

QRegExp FileTypes::regex(){
    static QRegExp compilersRx("("+FileTypes::values().join("|")+")");
    return compilersRx;
}

QRegExp FileTypes::endswith(){
    static QRegExp compilersRx("("+FileTypes::values().join("|")+")$");
    return compilersRx;
}