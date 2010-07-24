/****************************************************************************
**
** Copyright (C) 2006-2009 Ralf Habacker. All rights reserved.
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

#ifndef MISC_H
#define MISC_H

#include "registry.h" // for compatibility 

#include <QRegExp>
#include <QString>
#include <QSettings>
class QFile;
class QIODevice;
class QTextStream;

typedef enum { RTUnspecified=0, Stable, Unstable, Nightly } ReleaseType;
ReleaseType toReleaseType(const QString &type); 
const QString toString(ReleaseType type);

typedef enum { Unspecified=0, MinGW=1, MSVC = 2, MSVC8=2, MSVC9=3, MinGW4=4, MSVC_X64=5 } CompilerType;
CompilerType toCompilerType(const QString &type); 
const QString toString(CompilerType compilerType);


struct InstallFile
{
  QString inputFile;
  QString outputFile;
  bool bAbsInputPath;
  bool usedFile;

  InstallFile(const QString &in, const QString &out = QString(), bool absPath = false)
    : inputFile(in), outputFile(out), bAbsInputPath(absPath), usedFile(false)
  {}
  bool operator==(const InstallFile &o)
  {
    return (inputFile == o.inputFile && outputFile == o.outputFile);
  }
  friend QDebug operator<<(QDebug, const InstallFile &);
  friend QDebug operator<<(QDebug, const QList<InstallFile> &);
  friend QTextStream &operator<<(QTextStream &, const InstallFile &);
  friend QTextStream &operator<<(QTextStream &, const QList<InstallFile> &);
};

bool parseQtIncludeFiles(QList<InstallFile> &fileList, const QString &root, const QString &subdir, const QString &filter, const QString &exclude);

typedef struct {
    QString shortDesc;
    QString longDesc;
    QString categories;
    QString requires;
} HintFileDescriptor;

bool parseHintFile(const QString &hintFile,HintFileDescriptor &pkg);
bool parseHintFile(const QByteArray &ba, HintFileDescriptor &pkg);

bool findExecutables(QList<InstallFile> &fileList, const QString &root, const QString &subdir, const QString &filter, const QString &exclude, bool debugExe=false);

bool generateFileList(QList<InstallFile> &result, const QString &root, const QString &subdir, const QString &filter, const QList<QRegExp> &excludeList, bool verbose=false);
bool generateFileList(QList<InstallFile> &result, const QString &root, const QString &subdir, const QString &filter, const QString &exclude = QString(), bool verbose=false);

// try to delete a file, if not possible, move to root/tmp/deleteme
bool deleteFile(const QString &root, const QString &fn);

/// return windows start menu path
QString getStartMenuPath(bool bAllUsers);

/// return path of executable
QString exePath();

/// wait 
void qsleep(int ms);

/// remove a non empty directory structure
bool removeDirectory(const QString& aDir);

/// check if system is 64 bit Windows
bool isX64Windows();


/**
 InstallerCallConfig provides package related informations from the 
 installer filename which could be used to skip related wizard pages 
 or to implement specific gui applications. 
 
 - which package to install (e.g. umbrello)
 - which compiler (e.g vc90, mingw4)
 - which release type (e.g. stable,nightly,unstable)
 - which release version (e.g. 4.4.4)
 - which download server
 
 The filename has to build in the following manner: 

    setup-<packageName>-<compiler>-<version>-<releaseType>[-<mirror>].exe

  parts in [] are optional

  @todo move to separate file or a more descriptive place
*/

class InstallerCallConfig {
public:
    InstallerCallConfig(const QString &fileName=QString());
    bool isValid() { return key == "setup"; }

    static InstallerCallConfig &instance(); 

    QString packageName;
    ReleaseType releaseType; 
    QString version; 
    QString mirror; 
    CompilerType compilerType;

protected:
    bool isLoaded; 
    QString key;
    QString installerBaseName;

};



#endif
