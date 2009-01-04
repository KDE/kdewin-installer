/****************************************************************************
**
** Copyright (C) 2006-2007 Ralf Habacker. All rights reserved.
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

struct InstallFile
{
  QString inputFile;
  QString outputFile;
  bool bAbsInputPath;

  InstallFile(const QString &in, const QString &out = QString(), bool absPath = false)
    : inputFile(in), outputFile(out), bAbsInputPath(absPath)
  {}
  bool operator==(const InstallFile &o)
  {
    return (inputFile == o.inputFile && outputFile == o.outputFile);
  }
  friend QDebug operator<<(QDebug, const InstallFile &);
  friend QDebug operator<<(QDebug, const QList<InstallFile> &);
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

// create a md5 hash from file contents
QByteArray md5Hash(QFile &f);
QByteArray md5Hash(const QString &file);

/// return windows start menu path
QString getStartMenuPath(bool bAllUsers);

/// return path of executable
QString exePath();

#endif
