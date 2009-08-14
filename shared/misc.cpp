/****************************************************************************
**
** Copyright (C) 2006-2009 Ralf Habacker. All rights reserved.
** Copyright (C) 2006-2007 Christian Ehrlicher <ch.ehrlicher@gmx.de>
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

#include "misc.h"
#include "settings.h"

#include <qglobal.h>

#ifdef Q_WS_WIN
#include <windows.h>
#include <windowsx.h>
#include <objbase.h>
#include <shlobj.h>
#include <shlwapi.h>
#include <initguid.h>
#endif

#include <QCoreApplication>
#include <QDebug>
#include <QDir>
#include <QFileInfo>
#include <QSettings>
#include <QFile>
#include <QDateTime>
#include <QBuffer>
#include <qplatformdefs.h>

#ifndef MISC_SMALL_VERSION

QDebug operator<<(QDebug out, const InstallFile &c)
{
    out << "InstallFile ("
        << "inputFile" << c.inputFile
        << "outputFile" << c.outputFile
        << "bAbsInputPath" << c.bAbsInputPath
        << ")";
    return out;
}

QDebug operator<<(QDebug out, const QList<InstallFile> &c)
{
    out << "QList<InstallFile> ("
        << "size:" << c.size();
    Q_FOREACH(const InstallFile &p, c)
        out << p;
    out << ")";
    return out;
}

QTextStream &operator<<(QTextStream &out, const InstallFile &c)
{
    out << "InstallFile ("
        << "inputFile" << c.inputFile
        << "outputFile" << c.outputFile
        << "bAbsInputPath" << c.bAbsInputPath
        << ")";
    return out;
}

QTextStream &operator<<(QTextStream &out, const QList<InstallFile> &c)
{
    out << "QList<InstallFile> ("
        << "size:" << c.size();
    Q_FOREACH(const InstallFile &p, c)
        out << p;
    out << ")";
    return out;
}

#ifdef Q_WS_WIN
/*
    add correct prefix for win32 filesystem functions
    described in msdn, but taken from Qt's qfsfileeninge_win.cpp
*/
static QString longFileName(const QString &path)
{
    QString absPath = QDir::convertSeparators(path);
    QString prefix = QLatin1String("\\\\?\\");
    if (path.startsWith("//") || path.startsWith("\\\\")) {
        prefix = QLatin1String("\\\\?\\UNC\\");
        absPath.remove(0, 2);
    }
    return prefix + absPath;
}
#endif  // Q_WS_WIN

bool parseQtIncludeFiles(QList<InstallFile> &fileList, const QString &root, const QString &subdir, const QString &filter, const QString &exclude)
{
  QList<InstallFile> files;
  if(!generateFileList(files, root, subdir, filter, exclude))
    return false;
  // read every header and include the referenced one
  QFile file;
  QFileInfo fi;
  QChar Q('Q');
  QString r = root + '/';
  Q_FOREACH( const InstallFile &instFile, files ) {
    QString f = instFile.inputFile;
    file.setFileName(r + f);
    fi.setFile(file);
    // camel case incudes are fine
    if(fi.fileName().startsWith(Q)) {
      fileList += InstallFile(f, f);
      continue;
    }
    if(!file.open(QIODevice::ReadOnly))
      continue;
    QByteArray content = file.readAll();
    file.close();
    QString dir = QFileInfo(file).absolutePath();
    if(!content.startsWith("#include")) {
      fileList += InstallFile(f, f);
      continue;
    }
    int start = content.indexOf('\"');
    int end = content.lastIndexOf('\"');
    if(start == -1 || end == -1)
      continue;
    content = content.mid(start + 1, end - start - 1);
    fi.setFile(dir + '/' + QFile::decodeName(content));
    if(!fi.exists() || !fi.isFile())
      continue;
    fileList += InstallFile(fi.canonicalFilePath(), f, true);
  }
  return true;
}

bool findExecutables(QList<InstallFile> &fileList, const QString &root, const QString &subdir, const QString &filter, const QString &exclude, bool debugExe)
{
  QList<InstallFile> files;
  if(!generateFileList(files, root, subdir, filter, exclude))
    return false;
  // read every header and include the referenced one
  QFile file;
  QFileInfo fi;
  QString r = root + '/';
  Q_FOREACH( const InstallFile &instFile, files ) {
    QString f = instFile.inputFile;
    // camel case incudes are fine
    if(f.startsWith('Q')) {
      fileList += InstallFile(f, f);
      continue;
    }
    file.setFileName(r + f);
    if(!file.open(QIODevice::ReadOnly))
      continue;
    QByteArray content = file.readAll();
    file.close();
    QString dir = QFileInfo(file).absolutePath();
    if(!content.startsWith("#include"))
      continue;
    int start = content.indexOf('\"');
    int end = content.lastIndexOf('\"');
    if(start == -1 || end == -1)
      continue;
    content = content.mid(start + 1, end - start - 1);
    fi.setFile(dir + '/' + QFile::decodeName(content));
    if(!fi.exists() || !fi.isFile())
      continue;
    fileList += InstallFile(fi.canonicalFilePath(), f, true);
  }
  return true;
}

bool generateFileList(QList<InstallFile> &fileList, const QString &root, const QString &subdir, const QString &filter, const QString &exclude, bool verbose)
{
   // create a QListQRegExp
	QStringList sl = exclude.split(' ');
   QList<QRegExp> rxList;

   if(!sl.contains(QLatin1String("*.bak")))
       sl += "*.bak";

   Q_FOREACH( const QString &srx, sl ) {
       QRegExp rx(srx);
       rx.setPatternSyntax(QRegExp::Wildcard);

       rxList += rx;
   }

   return ::generateFileList(fileList, root, subdir, filter, rxList, verbose);
}

bool generateFileList(QList<InstallFile> &fileList, const QString &root,const QString &subdir, const QString &filter, const QList<QRegExp> &excludeList, bool verbose)
{
   QDir d;
   bool subdirs = true;
   if(subdir.isEmpty()) {
       d = QDir(root);
       subdirs = false;
   } else
       d = QDir(root + '/' + subdir);
   if (!d.exists()) {
        if (verbose)
            qDebug() << "Can't read directory" << QDir::convertSeparators(d.absolutePath());
        return false;
   }
   QStringList filt = (filter + QLatin1String(" *.manifest")).split(' ');
   // work around a qt bug (could not convince qt-bugs that it's a bug...)
   if(filt.contains(QLatin1String("*.*")))
     filt.append(QLatin1String("*"));
   d.setFilter(QDir::NoDotAndDotDot | QDir::AllEntries | QDir::AllDirs);
   d.setNameFilters(filt);
   d.setSorting(QDir::Name);

   const QFileInfoList list = d.entryInfoList();
   QFileInfo fi;

   QStringList manifestList;
   QStringList executableList;

   Q_FOREACH( const QFileInfo &fi, list ) {
       QString fn = fi.fileName();
       bool bFound = false;
       Q_FOREACH(const QRegExp &rx, excludeList) {
         if(rx.exactMatch(fn)) {
           bFound = true;
           break;
         }
       }
       if (bFound)
         continue;

       if (fi.isDir()) {
          QString toAdd;
          if(!subdirs)
            continue;
          if(!subdir.isEmpty())
            fn = subdir + '/' + fn;
          if(fn.startsWith(QLatin1String("./")))
            toAdd += fn.mid(2) + '/';
          else
            toAdd += fn + '/';
          //fileList += toAdd;
          //qDebug() << "added ..." << toAdd;

          generateFileList(fileList, root, fn, filter, excludeList);
       }
       else {
         QString toAdd;
         if(subdir.isEmpty())
           toAdd = fn;
         else
         if(subdir.startsWith(QLatin1String("./")))
           toAdd = subdir.mid(2) + '/' + fn;
         else
         if(subdir == QLatin1String("."))
           toAdd = fn;
         else
           toAdd = subdir + '/' + fn;

         if(toAdd.endsWith(QLatin1String(".manifest"))) {
           manifestList += toAdd;
           continue;
         }
         if(toAdd.endsWith(QLatin1String(".exe")) ||
            toAdd.endsWith(QLatin1String(".dll"))) {
           executableList += toAdd;
         }
         if(!fileList.contains(toAdd))
          fileList += toAdd;
       }
   }
   for(int i = 0; i < executableList.count(); i++) {
     const QString manifest = executableList[i] + QLatin1String(".manifest");
     if(manifestList.contains(manifest))
       fileList += manifest;
   }
   return true;
}

#ifdef Q_WS_WIN
QString getStartMenuPath(bool bAllUsers)
{
    int idl = bAllUsers ? CSIDL_COMMON_PROGRAMS : CSIDL_PROGRAMS;
    HRESULT hRes;
    WCHAR wPath[MAX_PATH+1];

    hRes = SHGetFolderPathW(NULL, idl, NULL, 0, wPath);
    if (SUCCEEDED(hRes))
    {
        QString s = QString::fromUtf16((unsigned short*)wPath);
        return s;
    }
    return QString();
}
#endif  // Q_WS_WIN

bool deleteFile( const QString &root, const QString &filename )
{
    QT_STATBUF statBuf;
    if (QT_STAT( filename.toLocal8Bit(), &statBuf ) != -1) {
        if( QFile::remove( filename ) )
            return true;
        // file already exists - rename old one
        if( !QDir().mkpath( root + QLatin1String ( "/tmp/removeme" ) ) )
            return false;
        QString fn = filename;
        fn = fn.replace( '\\', '/' );
        fn = fn.mid( fn.lastIndexOf( '/' ) );
        fn = root + QLatin1String ( "/tmp/removeme" ) + fn;
        if( !QFile::rename( filename, fn ) )
            return false;
    }
    return true;
}

#endif  // MISC_SMALL_VERSION

QString exePath()
{
    return QCoreApplication::applicationDirPath();    
}

void qsleep(int ms)
{
#ifdef Q_OS_WIN
    Sleep(ms);
#else
#warning implement nanosleep
    //nanosleep(ms);
#endif
}

bool removeDirectory(const QString& aDir)
{
    QDir dir( aDir );
    bool has_err = false;
    if (dir.exists())//QDir::NoDotAndDotDot
    {
        QFileInfoList entries = dir.entryInfoList(QDir::NoDotAndDotDot | 
        QDir::Dirs | QDir::Files);
        int count = entries.size();
        foreach(QFileInfo entryInfo, entries)
        {
            QString path = entryInfo.absoluteFilePath();
            if (entryInfo.isDir())
            {
                has_err = removeDirectory(path);
            }
            else
            {
                QFile file(path);
                if (!file.remove())
                    has_err = true;
            }
        }
        if (!dir.rmdir(dir.absolutePath()))
            has_err = true;
    }
    return(has_err);
}
