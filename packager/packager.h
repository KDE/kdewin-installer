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
#ifndef PACKAGER_H
#define PACKAGER_H

#include "package.h"
#include "misc.h"
#include "hash.h"

#include <QFile>
#include <QStringList>
#include <QList>
#include <QRegExp>


class PackagerInfo
{
public:
    bool writeToFile(const QString &file)
    {    
        QString checksum; 
        int size; 
        
        QFile f(file);
        if (!f.open(QIODevice::WriteOnly))
            return false;
        QString line;
        line += "<?xml version=\"1.0\" encoding=\"UTF-8\"?>\n";
        line += "<package type=\"runtime\">\n";
        line += "  <version>" + version + "</version>\n";
        line += "  <checksum type=\"" + hash.typeAsString() + "\">" + hash.value().toHex() + "</checksum>\n";
        line += "  <size type=\"installed\">" + QString::number(installedSize) + "</size>\n";
        line += "  <dependencies>" + dependencies.join(" ") + "</dependencies>\n";
        line += "</package>\n";
        f.write(line.toUtf8());
        f.close();
    }

    qint64 installedSize;
    HashValue hash;
	QString version;
    QStringList dependencies; 
}; 

class Packager {
    public:
      enum Type { NONE = 0, BIN = 1 ,LIB = 2 ,DOC = 4 ,SRC = 8, DEBUG = 16, ALL = 15};

      Packager(const QString &packageName, const QString &packageVersion,const QString &notes=QString());

      void setSourceRoot(const QString &dir) { m_srcRoot = dir; }
      void setSourceExcludes(const QString &excludes) { m_srcExcludes = excludes; }
      void setWithDebugPackage(bool mode) { m_debugPackage = mode; }
      void setCompressionMode(unsigned int mode) { m_compMode = (mode < 1 || mode > 2) ? 1 : mode; }
      void setCheckSumMode(const QString mode) { m_checkSumMode = mode; }
      void setVerbose(bool state) { m_verbose = state; }
      void setSpecialPackageMode(bool special) { m_special = special; }
      void setType(const QString &type) { m_type = type; m_name += '-' + type; }

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
        bool createZipFile(const QString &zipFile, const QString &filesRootDir, const QList<InstallFile> &files, const QList<MemFile> &memFiles, const QString &destRootDir, qint64 &installedSize );
        bool createTbzFile(const QString &zipFile, const QString &filesRootDir,const QList<InstallFile> &files, const QList<MemFile> &memFiles, const QString &destRootDir, qint64 &installedSize );
        bool compressFiles(const QString &zipFile, const QString &filesRootDir, const QList<InstallFile> &files, const QList<MemFile> &memFiles, const QString &destRootDir, qint64 &installedSize );
        bool createManifestFiles(const QString &rootdir, QList<InstallFile> &fileList, Packager::Type type, QList<MemFile> &manifestFiles);
        bool createQtConfig(QList<InstallFile> &fileList, QList<MemFile> &manifestFiles);
        bool makePackagePart(const QString &root, QList<InstallFile> &fileList, QList<MemFile> &manifestFiles, Packager::Type, QString destdir);


      QString getBaseName(Packager::Type type);
      QString getCompressedExtension(Packager::Type type);

      QString m_name;
      QString m_version;
      QString m_notes;
      QString m_rootDir;
      QString m_srcRoot;
      QString m_srcExcludes;
      QString m_checkSumMode;
      QString m_type;
      QString m_nameType;
      bool m_verbose;
      bool m_debugPackage;
      bool m_special;
      unsigned int m_compMode;
};

#endif
