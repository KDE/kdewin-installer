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

#ifndef TYPEHELPER_H
#define TYPEHELPER_H

#include <QFlags>

class QRegExp;
class QStringList;


class CompilerTypes{
public:
    enum CompilerTypeFlag{
        Unspecified=0,
        MinGW=1,
        MSVC=2,
        MSVC8=2,
        MSVC9=3,
        MSVC10=4,
        MinGW4=5,
        MSVC10_X64=6,
        MinGW4_W32=7,
        MinGW4_W64=8 
    };
     Q_DECLARE_FLAGS(CompilerType,CompilerTypeFlag);
    static CompilerTypes::CompilerType fromString(const QString &type); 
    static const QString toString(CompilerTypes::CompilerType compilerType);
    static const QStringList values();
    static QRegExp regex();
    static QRegExp endswith();
};
Q_DECLARE_OPERATORS_FOR_FLAGS(CompilerTypes::CompilerType);

class ArchitectureTypes{
public:
    enum ArchitectureFlag{
        Unspecified,
        x86,
        x64
    };
     Q_DECLARE_FLAGS(ArchitectureType,ArchitectureFlag);
    static ArchitectureTypes::ArchitectureType fromString(const QString &type); 
    static const QString toString(ArchitectureTypes::ArchitectureType architecture);
    static const QStringList values();
    static QRegExp regex();
    static QRegExp endswith();
};
 Q_DECLARE_OPERATORS_FOR_FLAGS(ArchitectureTypes::ArchitectureType);

class FileTypes{
public:
    enum FileTypeFlags{
        NONE = 0, BIN = 1 ,LIB = 2 ,DOC = 4 ,SRC = 8, DBG = 16, ALL = 31, ANY = 32, META = 95
    };
     Q_DECLARE_FLAGS(FileType,FileTypeFlags);
    static FileTypes::FileType fromString(const QString &type); 
    static const QString toString(FileTypes::FileType type);
    static const QStringList values();
    static QRegExp regex();
    static QRegExp endswith();
};
Q_DECLARE_OPERATORS_FOR_FLAGS(FileTypes::FileType);

#endif
