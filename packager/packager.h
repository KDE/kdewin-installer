/****************************************************************************
**
** Copyright (C) 2006-2009 Ralf Habacker. All rights reserved.
**
** This file is part of the KDE installer for windows
**
** SPDX-License-Identifier: LGPL-2.0-only
**
****************************************************************************/
#ifndef PACKAGER_H
#define PACKAGER_H

#include "package.h"
#include "../shared/packagerinfo.h"
#include "misc.h"
#include "hash.h"

class Packager {
    public:
      enum Type { NONE = 0, BIN = 1, LIB = 2, DOC = 4, SRC = 8, DBG = 16, DEBUG = 32, ALL = 31};

      Packager(const QString &packageName, const QString &packageVersion,const QString &notes=QString());
      ~Packager();
      
      void setSourceRoot(const QString &dir) { m_srcRoot = dir; }
      void setSymbolsRoot(const QString &dir) { m_symRoot = dir; }
      void setSourceExcludes(const QString &excludes) { m_srcExcludes = excludes; }
      void setWithDebugPackage(bool mode) { m_debugPackage = mode; }
      void setCompressionMode(unsigned int mode) { m_compMode = (mode < 1 || mode > 2) ? 1 : mode; }
      void setCheckSumMode(const QString mode) { m_checkSumMode = mode; }
      void setVerbose(bool state) { m_verbose = state; }
      void setSpecialPackageMode(bool special) { m_special = special; }
      void setType(const QString &type) { m_type = type; }

      /// mingw only: strip all debugging symbols from files to reduce size
      bool stripFiles(const QString &dir);

      virtual bool generatePackageFileList(QList<InstallFile> &result, Packager::Type type, const QString &dir=QString());
      virtual bool makePackage(const QString &dir, const QString &destdir=QString(), bool bComplete=false);
      virtual bool parseConfig(const QString &fileName) { return false; }

    protected:
        struct MemFile {
            QString    filename;
            QByteArray data;
        };
        /// mingw only: extract debuginformations from dll's
        bool createDebugFiles(const QString &dir);
        bool createHashFile(const QString &packageFileName, const QString &basePath, HashValue &hashValue);
        bool createZipFile(const QString &zipFile, const QString &filesRootDir, const QList<InstallFile> &files, const QList<MemFile> &memFiles, const QString &destRootDir, FileSizeInfo &sizeInfo );
        bool createTbzFile(const QString &zipFile, const QString &filesRootDir,const QList<InstallFile> &files, const QList<MemFile> &memFiles, const QString &destRootDir, FileSizeInfo &sizeInfo );
        bool compressFiles(const QString &zipFile, const QString &filesRootDir, const QList<InstallFile> &files, const QList<MemFile> &memFiles, const QString &destRootDir, FileSizeInfo &sizeInfo );
        bool createManifestFiles(const QString &rootdir, QList<InstallFile> &fileList, Packager::Type type, QList<MemFile> &manifestFiles);
        bool createQtConfig(QList<InstallFile> &fileList, QList<MemFile> &manifestFiles);
        bool makePackagePart(const QString &root, QList<InstallFile> &fileList, QList<MemFile> &manifestFiles, Packager::Type, QString destdir);


      QString getBaseName(Packager::Type type=Packager::NONE, const QChar &versionDelimiter='-');
      QString getBaseFileName(Packager::Type type);
      QString getCompressedExtension(Packager::Type type);

      QString m_name;
      QString m_version;
      QString m_notes;
      QString m_rootDir;
      QString m_srcRoot;
      QString m_symRoot;
      QString m_srcExcludes;
      QString m_checkSumMode;
      QString m_type;
      QStringList m_dependencies;
      bool m_verbose;
      bool m_debugPackage;
      bool m_special;
      unsigned int m_compMode;
      QMap<Packager::Type,PackagerInfo*> m_packagerInfo;
	  friend QDebug operator<<(QDebug, Packager::Type);
};

inline QDebug operator<<(QDebug out, Packager::Type c)
{
    switch(c) {
        case Packager::BIN:
            out << "runtime";
            break;
        case Packager::LIB:
            out << "development";
            break;
        case Packager::DOC:
            out << "documentation";
            break;
        case Packager::SRC:
            out << "source";
            break;
        case Packager::DBG:
            out << "symbols";
            break;
        case Packager::NONE:
            out << "complete package";
            break;
        default:
            break;
	}
	return out;
}
#endif
