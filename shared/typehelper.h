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

#ifndef TYPEHELPER_H
#define TYPEHELPER_H

#include <QFlags>
#include <QList>
#include <QString>

class QRegExp;
class QStringList;


/**
 holds a set of supported compilers
 and type conversation methods
*/
class CompilerTypes {
public:
    // Please do not change the constants, they are used in installation config files
    enum Type {
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

    /**
      returns a list of string with support compilers
    */
    static const QStringList &values();

    /**
      returns description of specific compiler
      @param type compiler type 
      @return description usable in gui 
    */
    static const QString description(Type type);

    /**
     returns state if the requested compiler is supported
     @param type type of compiler
     @return true if compiler is suppoprted
    */
    static bool contains(Type type);

    /**
     returns regular expression of supported compilers
     which could be used to match package file names.
     @return regular expression instance
    */
    static QRegExp regex();

    /**
     returns regular expression of supported compilers which could
     be used to match the end of a package file names.
     @return regular expression instance
    */
     static QRegExp endswith();

    // convert a string to compiler type
    static Type fromString(const QString &type);

    // convert a compiler type to a string
    static const QString toString(Type compilerType);

protected:
    static QList<Type> m_types;
    static QStringList m_typeStrings;
    static QStringList m_descriptions;

    static void init();
 };

class ArchitectureTypes {
public:
    enum Type {
        Unspecified,
        x86,
        x64
    };
    Q_DECLARE_FLAGS(Types,Type);
    static Type fromString(const QString &type);
    static const QString toString(Type architecture);
    static const QStringList values();
    static QRegExp regex();
    static QRegExp endswith();
};
Q_DECLARE_OPERATORS_FOR_FLAGS(ArchitectureTypes::Types);

class FileTypes {
public:
    enum Type {
        NONE = 0, BIN = 1 ,LIB = 2 ,DOC = 4 ,SRC = 8, DBG = 16, ALL = 31, ANY = 32, META = 95
    };
    Q_DECLARE_FLAGS(Types,Type);
    static Type fromString(const QString &type);
    static const QString toString(Type type);
    static const QStringList values();
    static QRegExp regex();
    static QRegExp endswith();
};
Q_DECLARE_OPERATORS_FOR_FLAGS(FileTypes::Types);

#endif
