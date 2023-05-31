/****************************************************************************
**
** Copyright (C) 2006-2010 Ralf Habacker. All rights reserved.
**
** This file is part of the KDE installer for windows
**
** SPDX-License-Identifier: LGPL-2.0-only
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

  InstallFile(const QString &in=QString(), const QString &out = QString(), bool absPath = false)
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
