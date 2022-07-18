/****************************************************************************
**
** Copyright (C) 2011 Patrick von Reth <patrick.vonreth@gmail.com>
** Copyright (C) 2011 Ralf Habacker <ralf.habacker@freenet.de>
** All rights reserved.
**
** This file is part of the KDE installer for windows
**
** SPDX-License-Identifier: LGPL-2.0-only
**
****************************************************************************/

#ifndef TYPEHELPER_H
#define TYPEHELPER_H

#include <QFlags>
#include <QList>
#include <QString>
#include <QStringList>

class QRegExp;

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
        MinGW4_W64=8,
        MSVC11=9,
        MSVC12=10,
        MSVC14=11,
        MSVC141=12,
        MSVC141_X64=13,
        MSVC142=14,
        MSVC142_X64=15
    };
    enum Scope {
        allCompiler,
        supportedCompiler,
    };

    class Compiler {
    public:
        Type type;
        QString name;
        QString description;

        Compiler(Type _type, const QString &_name, const QString &_description)
          : type(_type)
          , name(_name)
          , description(_description)
        {}
    };

    CompilerTypes(Scope scope=supportedCompiler);
    ~CompilerTypes();

    /**
      returns a list of string with support compilers
    */
    const QStringList values();

    /**
      returns description of specific compiler
      @param type compiler type 
      @return description usable in gui 
    */
    const QString description(Type type);

    /**
     returns state if the requested compiler is supported
     @param type type of compiler
     @return true if compiler is suppoprted
    */
    bool contains(Type type);

    /**
     returns regular expression of supported compilers
     which could be used to match package file names.
     @return regular expression instance
    */
    QRegExp &regex();

    /**
     returns regular expression of supported compilers which could
     be used to match the end of a package file names.
     @return regular expression instance
    */
    QRegExp &endswith();

    // convert a string to compiler type
    Type fromString(const QString &type);

    // convert a compiler type to a string
    const QString toString(Type compilerType);

protected:
    QList<Compiler> m_compiler;

    QRegExp *m_containsRegExp;
    QRegExp *m_endsRegExp;
};

extern CompilerTypes supportedCompilers;
extern CompilerTypes allCompilers;

class FileTypes {
public:
    enum Type {
        NONE = 0, BIN = 1 ,LIB = 2 ,DOC = 4 ,SRC = 8, DBG = 16, ALL = 31, ANY = 32, META = 95
    };
    Q_DECLARE_FLAGS(Types,Type);
    static Type fromString(const QString &type);
    static const QString toString(Type type);
    static const QStringList values();
    static QRegExp &regex();
    static QRegExp &endswith();
};
Q_DECLARE_OPERATORS_FOR_FLAGS(FileTypes::Types);

#endif
