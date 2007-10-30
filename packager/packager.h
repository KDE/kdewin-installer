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
#include "misc.h"

class Packager {
    public:
      enum Type { NONE = 0, BIN = 1 ,LIB = 2 ,DOC = 4 ,SRC = 8, ALL = 15};

      Packager(const QString &packageName, const QString &packageVersion,const QString &notes=QString());

      /// mingw only: strip all debugging symbols from files to reduce size
      bool stripFiles(const QString &dir);
      /// mingw only: extract debuginformations from dll's
      bool createDebugFiles(const QString &dir);
      void setSourceRoot(const QString &dir) { m_srcRoot = dir; }
      void setSourceExcludes(const QString &excludes) { m_srcExcludes = excludes; }
      void setWithDebugLibs(bool mode) { m_debugLibs = mode; }
      void setCompressionMode(unsigned int mode) { m_compMode = (mode < 1 || mode > 2) ? 1 : mode; }

      bool generatePackageFileList(QList<InstallFile> &result, Packager::Type type, const QString &dir=QString());

      bool makePackage(const QString &dir, const QString &destdir=QString(), bool bComplete=false);
      void setVerbose(bool state) { m_verbose = state; }

    protected:
        struct MemFile {
            QString    filename;
            QByteArray data;
        };
        bool createZipFile(const QString &zipFile, const QString &filesRootDir, const QList<InstallFile> &files, const QList<MemFile> &memFiles=QList<MemFile>(), const QString &destRootDir=QString() );
        bool createTbzFile(const QString &zipFile, const QString &filesRootDir,const QList<InstallFile> &files, const QList<MemFile> &memFiles=QList<MemFile>(), const QString &destRootDir=QString() );
        bool compressFiles(const QString &zipFile, const QString &filesRootDir, const QList<InstallFile> &files, const QList<MemFile> &memFiles=QList<MemFile>(), const QString &destRootDir=QString() );
        bool createManifestFiles(const QString &rootdir, QList<InstallFile> &fileList, Packager::Type type, QList<MemFile> &manifestFiles);
      bool createQtConfig(QList<InstallFile> &fileList, QList<MemFile> &manifestFiles);

      QString getBaseName(Packager::Type type);

    private:
      QString m_name;
      QString m_version;
      QString m_notes;
      QString m_rootDir;
      QString m_srcRoot;
      QString m_srcExcludes;
      bool m_verbose;
      bool m_debugLibs;
      unsigned int m_compMode;
};

#endif
