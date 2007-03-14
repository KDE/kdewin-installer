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
#ifndef PACKAGER_H
#define PACKAGER_H

#include <QStringList>
#include <QList>
#include <QRegExp>
#include "package.h"

class Packager {
    public: 
      // don't know how to make usable package::Type
      enum Type { NONE = 0, BIN = 1 ,LIB = 2 ,DOC = 4 ,SRC = 8, ALL = 15};

      Packager(const QString &packageName, const QString &packageVersion,const QString &notes=QString());

      // mingw only: strip all debugging symbols from files to reduce size
      bool stripFiles(const QString &dir);
      bool createDebugFiles(const QString &dir);

      bool generatePackageFileList(QStringList &result, Packager::Type type, const QString &dir=QString());

      bool makePackage(const QString &dir, const QString &destdir=QString(), bool bComplete=false);
      
    protected: 
        struct MemFile {
            QString    filename;
            QByteArray data;
        };
      bool createZipFile(const QString &baseName, const QStringList &files, const QList<MemFile> &memFiles=QList<MemFile>());
      bool createManifestFiles(QStringList &fileList, Packager::Type type, QList<MemFile> &manifestFiles);

      QString getBaseName(Packager::Type type);
      
    private: 
      QString m_name;
      QString m_version;
      QString m_notes;
      QString m_rootDir;
      bool m_verbose;
}; 

#endif
