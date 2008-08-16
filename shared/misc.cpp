/****************************************************************************
**
** Copyright (C) 2006-2007 Ralf Habacker. All rights reserved.
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

#include <QtCore/QCryptographicHash>
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

   QFileInfoList list = d.entryInfoList();
   QFileInfo fi;

   QStringList manifestList;
   QStringList executableList;

   Q_FOREACH( const QFileInfo &fi, list ) {
       QString fn = fi.fileName();

       bool bFound = false;
       Q_FOREACH( const QRegExp &rx, excludeList) {
           if(rx.exactMatch(fn)) {
               bFound = true;
               break;
           }
       }
       if (bFound)
           continue;

       if (fi.isDir()) {
          if(!subdirs)
            continue;
          if(!subdir.isEmpty())
            fn = subdir + '/' + fn;
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
     QString manifest = executableList[i] + QLatin1String(".manifest");
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

#if defined(__MINGW32__)
# define WIN32_CAST_CHAR (WCHAR*)
#else
# define WIN32_CAST_CHAR (LPCWSTR)
#endif

#define MAX_KEY_LENGTH 255

/**
 \return a value from MS Windows native registry.
 @param akey is usually one of HKEY_CLASSES_ROOT, HKEY_CURRENT_USER, HKEY_LOCAL_MACHINE
        constants defined in WinReg.h.
 @param subKey is a registry subkey defined as a path to a registry folder, eg.
        "SOFTWARE\\Microsoft\\Windows\\CurrentVersion\\Explorer\\Shell Folders"
        ('\' delimiter must be used)
 @param item is an item inside subKey or "" if default folder's value should be returned
 @param ok if not null, will be set to true on success and false on failure
*/
QVariant getWin32RegistryValue(RegKey akey, const QString& subKey, const QString& item, bool *ok)
{
#define FAILURE { if (ok) *ok = false; return QString(); }
    if (ok)
        *ok = true;

    if (subKey.isEmpty())
        FAILURE;

    HKEY key;
    switch(akey) {
        case hKEY_CURRENT_USER: key = HKEY_CURRENT_USER; break;
        case hKEY_LOCAL_MACHINE: key = HKEY_LOCAL_MACHINE; break;
        case hKEY_CLASSES_ROOT: key = HKEY_CLASSES_ROOT; break;
        default: FAILURE;
    }

    HKEY hKey;
    DWORD dwType;
    DWORD dwSize;

    if (ERROR_SUCCESS != RegOpenKeyExW(key, WIN32_CAST_CHAR subKey.utf16(), 0, KEY_READ, &hKey))
        FAILURE;

    if (ERROR_SUCCESS != RegQueryValueExW(hKey, WIN32_CAST_CHAR item.utf16(), NULL, &dwType, NULL, &dwSize))
        FAILURE;

    QVariant res;
    switch(dwType) {
        case REG_MULTI_SZ:
            // FIXME: this should give a QStringList; to be implemented later
            FAILURE;
            break;
        case REG_EXPAND_SZ:
        case REG_SZ: {
            QByteArray ba(dwSize * 2, 0);
            if (ERROR_SUCCESS != RegQueryValueExW(hKey, WIN32_CAST_CHAR item.utf16(), NULL, &dwType, (LPBYTE)ba.data(), &dwSize))
                FAILURE;
            res = QString::fromUtf16( (unsigned short*)ba.data() );
            break;
        }
        case REG_DWORD: {
            qint32 dwVal;
            if (ERROR_SUCCESS != RegQueryValueExW(hKey, WIN32_CAST_CHAR item.utf16(), NULL, &dwType, (LPBYTE)&dwVal, &dwSize))
                FAILURE;
            res = dwVal;
            break;
        }
        case REG_BINARY: {
            QByteArray ba(dwSize, 0);
            if (ERROR_SUCCESS != RegQueryValueExW(hKey, WIN32_CAST_CHAR item.utf16(), NULL, &dwType, (LPBYTE)ba.data(), &dwSize))
                FAILURE;
            res = ba;
            break;
        }
        default:
            FAILURE;
    }
    RegCloseKey(hKey);

    return res;
}

/**
 \insert a value into MS Windows native registry.
 @param akey is usually one of HKEY_CLASSES_ROOT, HKEY_CURRENT_USER, HKEY_LOCAL_MACHINE
        constants defined in WinReg.h.
 @param subKey is a registry subkey defined as a path to a registry folder, eg.
        "SOFTWARE\\Microsoft\\Windows\\CurrentVersion\\Explorer\\Shell Folders"
        ('\' delimiter must be used)
 @param item is an item inside subKey or "" if default folder's value should be returned
 @param ok if not null, will be set to true on success and false on failure
 --SE
*/
bool setWin32RegistryValue(const RegKey& akey, const QString& subKey, const QString& item, const QVariant& value, RegType qvType)
{
    HKEY key;
    HKEY hKey;
    DWORD dwResult;
    DWORD dwType;
    DWORD dwSize;
    DWORD lt;
    BYTE *lpData;

    if (subKey.isEmpty()) return false;

    switch(akey) {
        case hKEY_CURRENT_USER: key = HKEY_CURRENT_USER; break;
        case hKEY_LOCAL_MACHINE: key = HKEY_LOCAL_MACHINE; break;
        case hKEY_CLASSES_ROOT: key = HKEY_CLASSES_ROOT; break;
        default: return false;
    }
/*
    it needs to be sure that QVariant can be converted to the Registry Data Types REG_BINARY | REG_DWORD | REG_EXPAND_SZ | REG_SZ
*/

    if(qvType == qt_unknown) {
        // test type of value to find good registry data type
        switch(value.type()) {
            case QVariant::Date:
            case QVariant::DateTime:
            case QVariant::Time:
            case QVariant::String:        qvType = qt_String; break;
            case QVariant::ByteArray:    qvType = qt_BINARY; break;
            case QVariant::ULongLong:
            case QVariant::LongLong:
            case QVariant::Bool:
            case QVariant::UInt:
            case QVariant::Int:            qvType = qt_DWORD; break;
            case QVariant::StringList:    qvType = qt_MultiString; break;
            default:                    return false;
        }
    }

    switch(qvType) {
        case qt_BINARY:             dwType = REG_BINARY;
                                    lpData = (BYTE *) value.toString().toLocal8Bit().data();
                                    dwSize = sizeof(char) * value.toString().toLocal8Bit().length();
                                    break;
        case qt_DWORD:              dwType = REG_DWORD;
                                    lt = value.toInt();             // thinking that it will return qint32 == long signed int(16)!!!!
                                    lpData = (BYTE *)& lt;
                                    dwSize = sizeof(DWORD);
                                    break;
        case qt_String:             dwType = REG_SZ;
                                    lpData = (BYTE *) value.toString().utf16();
                                    dwSize = 2 * sizeof(char) * value.toString().length();
                                    break;
        case qt_ExpandedString:     dwType = REG_EXPAND_SZ;
                                    lpData = (BYTE *) value.toString().utf16();
                                    dwSize = 2 * sizeof(char) * value.toString().length();
                                    break;
        case qt_MultiString:        dwType = REG_MULTI_SZ;
//FIXME: useful for QStringList - to be implemented later
                                    return false;
                                    break;
        default:                    return false;
    }

    if (ERROR_SUCCESS != RegCreateKeyExW(key, WIN32_CAST_CHAR subKey.utf16(), 0, NULL, REG_OPTION_NON_VOLATILE, KEY_WRITE, NULL, &hKey, &dwResult))
        return false;

    if (ERROR_SUCCESS != RegSetValueExW(hKey, WIN32_CAST_CHAR item.utf16(), 0, dwType, lpData, dwSize))
        return false;
    RegCloseKey(hKey);
    return true;
}

/**
 \delete a value from MS Windows native registry.
 @param akey is usually one of HKEY_CLASSES_ROOT, HKEY_CURRENT_USER, HKEY_LOCAL_MACHINE
        constants defined in WinReg.h.
 @param subKey is a registry subkey defined as a path to a registry folder, eg.
        "SOFTWARE\\Microsoft\\Windows\\CurrentVersion\\Explorer\\Shell Folders"
        ('\' delimiter must be used)
 --SE
*/

bool delWin32RegistryValue(const RegKey& akey, const QString& subKey)
{
    HKEY key;
    HKEY hKey;
    DWORD dwSubKeys;
    DWORD dwValues;
    DWORD keyLength;
    QString keyName;
    int i;

    if (subKey.isEmpty()) {
        qDebug() << "empty key";
        return false;
    }

    QString hSubKey = subKey.right(subKey.length() - subKey.lastIndexOf("\\") - 1);
    QString lSubKey = subKey.left(subKey.lastIndexOf("\\"));

    switch(akey) {
        case hKEY_CURRENT_USER: key = HKEY_CURRENT_USER; break;
        case hKEY_LOCAL_MACHINE: key = HKEY_LOCAL_MACHINE; break;
        case hKEY_CLASSES_ROOT: key = HKEY_CLASSES_ROOT; break;
        default: return false;
    }


    if (ERROR_SUCCESS != RegOpenKeyExW(key, WIN32_CAST_CHAR subKey.utf16(), 0, KEY_ALL_ACCESS, &hKey)) {
        // is it a value instead?
        if (ERROR_SUCCESS != RegOpenKeyExW(key, WIN32_CAST_CHAR lSubKey.utf16(), 0, KEY_ALL_ACCESS, &hKey)) {
            qDebug() << "Could not open registry Key for writing";
            return false;
        } else {
            if (ERROR_SUCCESS != RegDeleteValueW(hKey, WIN32_CAST_CHAR hSubKey.utf16())) {
                qDebug() << "Could not delete registry value" << lSubKey << hSubKey;
                return false;
            }
            RegCloseKey(hKey);
        }
    } else {
        if( ERROR_SUCCESS != RegQueryInfoKeyW(hKey, NULL, NULL, NULL, &dwSubKeys, NULL, NULL, &dwValues, NULL, NULL, NULL, NULL) ){
            qDebug() << "Could not query key" << subKey;
            return false;
        } else {
            for( i = dwSubKeys - 1; i >= 0; i-- ) {
                keyLength = MAX_KEY_LENGTH;
                keyName.resize(MAX_KEY_LENGTH);
                if(ERROR_SUCCESS != RegEnumKeyExW(hKey, (DWORD) i, (wchar_t *) keyName.data(), &keyLength, NULL, NULL, NULL, NULL)) {
                    qDebug() << "Could not enumerate registry key" << subKey << "for keys";
                    return false;
                }
                keyName.resize(keyLength);
                if(ERROR_SUCCESS != SHDeleteKeyW(hKey, WIN32_CAST_CHAR keyName.utf16())) {
                    qDebug() << "Could not delete Key";
                    return false;
                };
            }
            for( i = dwValues - 1; i >= 0; i-- ) {
                keyLength = MAX_KEY_LENGTH;
                keyName.resize(MAX_KEY_LENGTH);
                if(ERROR_SUCCESS != RegEnumValueW(hKey, (DWORD) i, (wchar_t *) keyName.data(), &keyLength, NULL, NULL, NULL, NULL)) {
                    qDebug() << "Could not enumerate registry key" << subKey << "for values";
                    return false;
                }
                keyName.resize(keyLength);
                if (ERROR_SUCCESS != RegDeleteValueW(hKey, WIN32_CAST_CHAR keyName.utf16())) {
                    qDebug() << "Could not delete registry value" << subKey << keyName;
                    return false;
                }
            }
        }
        RegCloseKey(hKey);
        if(lSubKey.isEmpty()) return true;
        if (ERROR_SUCCESS != RegOpenKeyExW(key, WIN32_CAST_CHAR lSubKey.utf16(), 0, KEY_ALL_ACCESS, &hKey)) {
            qDebug() << "Could not open registry key" << lSubKey << "for writing";
            return false;
        } else {
            if (ERROR_SUCCESS != RegDeleteKeyW(hKey, WIN32_CAST_CHAR hSubKey.utf16())) {
                qDebug() << "Could not delete registry key" << hSubKey;
                return false;
            }
            RegCloseKey(hKey);
        }
    }

    return true;
}
#endif  // Q_WS_WIN

/*
  tries to delete a file named filename
  if it's not possible, move them to root/tmp/removeme
*/
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

QByteArray md5Hash(QFile &file)
{
    static const int bufSize = 1024*1024;
    QCryptographicHash md5( QCryptographicHash::Md5 );
    QByteArray ba;
    ba.resize ( bufSize );
    qint64 iBytesRead;
    while ( ( iBytesRead = file.read ( ba.data(), bufSize ) ) > 0 )
        md5.addData ( ba.data(), iBytesRead );
    return md5.result();
}

QByteArray md5Hash(const QString &file)
{
    QFile f(file);
    if(!f.open(QIODevice::ReadOnly))
      return QByteArray();
    return md5Hash(f);
}
