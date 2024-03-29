/****************************************************************************
**
** Copyright (C) 2006-2010 Ralf Habacker. All rights reserved.
** Copyright (C) 2006-2007 Christian Ehrlicher <ch.ehrlicher@gmx.de>
**
** This file is part of the KDE installer for windows
**
** SPDX-License-Identifier: LGPL-2.0-only
**
****************************************************************************/

#include "misc.h"
#include "settings.h"

#include <qglobal.h>

#ifdef Q_OS_WIN32
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
#include <QMap>

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

#ifdef Q_OS_WIN32
/*
    add correct prefix for win32 filesystem functions
    described in msdn, but taken from Qt's qfsfileeninge_win.cpp
*/
static QString longFileName(const QString &path)
{
    QString absPath = QDir::toNativeSeparators(path);
    QString prefix = QLatin1String("\\\\?\\");
    if (path.startsWith("//") || path.startsWith("\\\\")) {
        prefix = QLatin1String("\\\\?\\UNC\\");
        absPath.remove(0, 2);
    }
    return prefix + absPath;
}
#endif  // Q_OS_WIN32

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

bool generateFileList(QList<InstallFile> &fileList, const QString &root, const QString &subdir, const QString &filter, const QString &exclude, const QString &outputdir, bool verbose)
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

   return ::generateFileList(fileList, root, subdir, subdir, filter, rxList, outputdir, verbose);
}

bool generateFileList(QList<InstallFile> &fileList, const QString &root, const QString &origSubdir, const QString &subdir, const QString &filter, const QList<QRegExp> &excludeList, const QString &outputdir, bool verbose)
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
            qDebug() << "Can't read directory" << QDir::toNativeSeparators(d.absolutePath());
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

          generateFileList(fileList, root, origSubdir, fn, filter, excludeList, outputdir);
       }
       else {
         QString toAdd;
         QString renameTo;
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

         if(!outputdir.isEmpty())
         {
           renameTo = toAdd;
           if(!origSubdir.isEmpty())
             renameTo.replace(origSubdir, outputdir);
           else
             renameTo = outputdir + '/' + toAdd;
         }

         if(toAdd.endsWith(QLatin1String(".manifest"))) {
           manifestList += toAdd;
           continue;
         }
         if(toAdd.endsWith(QLatin1String(".exe")) ||
            toAdd.endsWith(QLatin1String(".dll"))) {
             executableList += toAdd;
         }
         if(!fileList.contains(toAdd))
         {
           fileList += InstallFile(toAdd, renameTo);
         }
       }
   }
   for(int i = 0; i < executableList.count(); i++) {
     const QString manifest = executableList[i] + QLatin1String(".manifest");
     if(manifestList.contains(manifest))
     {
       QString renameTo;
       if(!outputdir.isEmpty())
       {
         renameTo = manifest;
         if(!origSubdir.isEmpty())
           renameTo.replace(origSubdir, outputdir);
         else
           renameTo = outputdir + '/' + manifest;
       }
       fileList += InstallFile(manifest, renameTo);
     }
   }
   return true;
}

#ifdef Q_OS_WIN32
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
#endif  // Q_OS_WIN32

bool deleteFile( const QString &root, const QString &filename )
{
    if ( QFile::exists( filename ) ) {
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
    struct timespec ts;
    ts.tv_sec = ms / 1000;
    ts.tv_nsec = (ms % 1000) * 1000000;
    nanosleep(&ts, NULL);
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


#ifdef Q_OS_WIN32
static BOOL IsWow64()
{
    typedef BOOL (WINAPI *LPFN_ISWOW64PROCESS) (HANDLE, PBOOL);
    BOOL bIsWow64 = FALSE;
    LPFN_ISWOW64PROCESS fnIsWow64Process;

    fnIsWow64Process = (LPFN_ISWOW64PROCESS) GetProcAddress(
        GetModuleHandle(TEXT("kernel32")),"IsWow64Process");
  
    if (NULL != fnIsWow64Process)
    {
        if (!fnIsWow64Process(GetCurrentProcess(),&bIsWow64))
        {
            // handle error
        }
    }
    return bIsWow64;
}
#endif

bool isX64Windows()
{
#if defined(_WIN64)
    return true;  // 64-bit programs run only on Win64
#elif defined(_WIN32)
    // 32-bit programs run on both 32-bit and 64-bit Windows
    // so must sniff
    return IsWow64();
#else
    return false; // Win64 does not support Win16
#endif
}

ReleaseType toReleaseType(const QString &_type)
{
    QString type(_type.toLower());
    if (type == "unstable")
        return Unstable;
    else if (type== "stable")
        return Stable;
    else if (type == "nightly")
        return Nightly;
    else if (type == "attic")
        return Attic;
    else if (type == "single")
        return Single;
    else
        return Undefined;
}

const QString toString(ReleaseType type)
{
    if (type == Stable)
        return "stable";
    else if (type == Unstable)
	    return "unstable";
    else if (type == Nightly)
	    return "nightly";
    else if (type == Attic)
        return "attic";
    else if (type == Single)
        return "single";
    else
	    return "unknown";
}

int toVersionInt(const QString &version)
{
    int result = 0;
    foreach(const QString v, version.split("."))
    {
        result *= 256;
        result += v.toInt();
    }
    if (result < 0x10000)
        result <<= 8;
    else if (result < 0x100)
        result <<= 16;

    return result;
}

bool isDirWritable(const QString &rootDir)
{
#ifndef Q_OS_WIN32
    QFileInfo fi(rootDir);
    return fi.isWritable();
#else
    bool isWritable = true;
    bool deleteDir = false;
    QDir d(rootDir);
    if (!d.exists())
    {
        if (!d.mkdir(rootDir))
        {
            isWritable = false;
            qDebug() << "directory" << rootDir << "is not writable";
        }
        else
            deleteDir = true;
    }
    if (isWritable)
    {
        QFile f(rootDir + "/~temp");
        if (!f.open(QIODevice::ReadWrite))
        {
            isWritable = false;
            qDebug() << "Could not create files in directory" << rootDir;
        }
        else
        {
            f.close();
            f.remove();
        }
        QDir d2;
        if (!d2.mkdir(rootDir + "/~tempdir"))
        {
            isWritable = false;
            qDebug() << "Could not create directories in directory" << rootDir;
        }
        else
        {
            d2.rmdir(rootDir + "/~tempdir");
        }
    }
    if (deleteDir)
        d.rmdir(rootDir);
    return isWritable;
#endif
}
