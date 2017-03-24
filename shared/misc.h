/****************************************************************************
**
** Copyright (C) 2006-2010 Ralf Habacker. All rights reserved.
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
#include <QFlags>

class QFile;
class QIODevice;
class QTextStream;


typedef enum { Undefined = 0, Single, Stable, Unstable, Nightly, Attic } ReleaseType;
ReleaseType toReleaseType(const QString &type); 
const QString toString(ReleaseType type);

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

/**
 generate file list
 @param result returned list
 @param root root path where to search
 @param subdir subdir appended to root to search for, use '.' for recursive search from root directory
 @param filter pattern for included file types (separated by space)
 @param excludeList regex for excluded file types - use either this or filter
 @param verbose dump collected files
 @return False in case of errore
*/
 bool generateFileList(QList<InstallFile> &result, const QString &root, const QString &subdir, const QString &filter, const QList<QRegExp> &excludeList, bool verbose=false);

 /**
 generate file list
 @param result returned list
 @param root root path where to search
 @param subdir subdir appended to root to search for, use '.' for recursive search from root directory
 @param filter regex for included file types (separated by space)
 @param exclude pattern for excluded file types (separated by space) - use either this or filter
 @param verbose dump collected files
 @return False in case of errore
*/
bool generateFileList(QList<InstallFile> &result, const QString &root, const QString &subdir, const QString &filter, const QString &exclude = QString(), bool verbose=false);

// try to delete a file, if not possible, move to root/tmp/deleteme
bool deleteFile(const QString &root, const QString &fn);

/// return windows start menu path
QString getStartMenuPath(bool bAllUsers);

/// return int value of version string 'a.b.c' as 0x00aabbcc, 'a.b' as 0x00aabb00, 'a' as 0x00aa0000
int toVersionInt(const QString &version);

/// return path of executable
QString exePath();

/// wait 
void qsleep(int ms);

/// remove a non empty directory structure
bool removeDirectory(const QString& aDir);

/// check if system is 64 bit Windows
bool isX64Windows();

//// check if a directory is writable
bool isDirWritable(const QString &rootDir);

#endif
