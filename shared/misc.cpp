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
#define EOL_SIZE 2
bool readDesktopFile(QIODevice &device, QSettings::SettingsMap &map)
{
    bool ret = true;
    qint64 buffersize = 1025;
    QByteArray buf(buffersize - 1, '\0');
    QString group;
    while(device.readLine(buf.data(), buffersize - 1) != -1) {
        QString buffer = QString(buf).trimmed();
        QString key = buffer.split('=')[ 0 ];
        if(!key.startsWith("[")) {
            map.insert(group + QString("/") + key, buffer.mid(key.size() + 1, buffer.size() - key.size()));
        } else {
            group = buffer.mid(1, buffer.lastIndexOf("]") - 1);
        }
    }
    return ret;
}

bool writeDesktopFile(QIODevice &device, const QSettings::SettingsMap &map)
{
    return false;
}



// from http://msdn.microsoft.com/library/default.asp?url=/library/en-us/shellcc/platform/shell/programmersguide/shell_int/shell_int_programming/shortcuts/shortcut.asp
// CreateLink - uses the Shell's IShellLink and IPersistFile interfaces
//              to create and store a shortcut to the specified object.
//
// Returns true if link <linkName> could be created, otherwise false.
//
// Parameters:
// fileName     - full path to file to create link to
// linkName     - full path to the link to be created
// description  - description of the link (for tooltip)

bool CreateLink(const QString &_fileName, const QString &_linkName, const QString &description, const QString &workingDir = QString())
{
    HRESULT hres;
    IShellLinkW* psl;

    QString fileName = _fileName;
    QString linkName = longFileName(_linkName);

    LPCWSTR lpszPathObj  = (LPCWSTR)fileName.utf16();
    LPCWSTR lpszPathLink = (LPCWSTR)linkName.utf16();
    LPCWSTR lpszDesc     = (LPCWSTR)description.utf16();
    LPCWSTR lpszWorkDir  = (LPCWSTR)workingDir.utf16();

    CoInitialize(NULL);
    // Get a pointer to the IShellLink interface.
    hres = CoCreateInstance(CLSID_ShellLink, NULL, CLSCTX_ALL, IID_IShellLinkW, (LPVOID*)&psl);

    if (SUCCEEDED(hres))
    {
        IPersistFile* ppf;

        // Set the path to the shortcut target and add the description.
        if(!SUCCEEDED(psl->SetPath(lpszPathObj))) {
            qDebug() << "error setting path for link to " << fileName;
            psl->Release();
            return false;
        }
        if(!SUCCEEDED(psl->SetDescription(lpszDesc))) {
            qDebug() << "error setting description for link to " << description;
            psl->Release();
            return false;
        }
        if(!SUCCEEDED(psl->SetWorkingDirectory(lpszWorkDir))) {
            qDebug() << "error setting working Directory for link to " << workingDir;
            psl->Release();
            return false;
        }


        // Query IShellLink for the IPersistFile interface for saving the
        // shortcut in persistent storage.
        hres = psl->QueryInterface(IID_IPersistFile, (LPVOID*)&ppf);

        if (SUCCEEDED(hres))
        {
            hres = ppf->Save(lpszPathLink, TRUE);
            // Save the link by calling IPersistFile::Save.
            if(!SUCCEEDED(hres))
                qDebug() << "error saving link to " << linkName;

            ppf->Release();
        }
        psl->Release();
    } else {
        qDebug() << "Error: Got no pointer to the IShellLink interface.";
    }
    CoUninitialize(); // cleanup COM after you're done using its services
    return SUCCEEDED(hres);
}


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

/*
 * create start menu entries from installed desktop files
 */
bool createStartMenuEntries(const QString &dir, const QString &installDir, const QString &category)
{
    QList<InstallFile> fileList;
    generateFileList(fileList,dir,"","*.desktop");

    const QSettings::Format DesktopFormat = QSettings::registerFormat("desktop", readDesktopFile, writeDesktopFile);

    Q_FOREACH( const InstallFile &installFile, fileList )
    {
        QString file = installFile.inputFile;
        QString startMenuCategory;
        QSettings registry("kdewin-installer");
        registry.beginGroup("StartMenuEntries");
        registry.beginGroup(category);

        QSettings settings(dir + '/' + file, DesktopFormat);

        settings.beginGroup("Desktop Entry");
        const QString name = settings.value("Name").toString();
        const QString mimeType = settings.value("Mime Type").toString();
        const QString genericName = settings.value("GenericName").toString();
        const QString exec = installDir + settings.value("Exec").toString().split(' ')[0];
        const QString icon = settings.value("Icon").toString();

        QStringList categories = settings.value("Categories").toString().split(';');
        file.replace(".desktop", ".lnk");

        const QStringList catIgnore = QString( FREEDESKTOP_IGNORE_CATEGORIES ).split(';');
        const QStringList catMain = QString( FREEDESKTOP_MAIN_CATEGORIES ).split(';');
        const QStringList catSub = QString( FREEDESKTOP_SUB_CATEGORIES ).split(';');
        QString catMainFirst;
        QString catSubFirst;

        // catIgnore is the shortest List
        Q_FOREACH( const QString &str, catIgnore ) {
            categories.removeAll(str);
        }

        // categories isn't to long as well
        Q_FOREACH( const QString &str, categories ) {
            if( catMain.contains( str ) ) {
                startMenuCategory = str;
                startMenuCategory.append("/");
                break;
            }
        }

        Q_FOREACH( const QString &str, categories ) {
            if( catSub.contains( str ) ) {

                    startMenuCategory += str + "/";
                break;
            }
        }

        if(!category.isEmpty() && category != "Miscelleanous") {
            startMenuCategory = category;
            startMenuCategory.append("/");
        }

        settings.endGroup();
        if (!exec.isEmpty())
        {
            QString p = getStartMenuPath(false);
            if(p.isEmpty()) {
                qDebug() << "Can't determine Start menu folder!";
                continue;
            }

            QString dir = p + "/KDE/" + startMenuCategory;
            QDir d(dir);
            if(!d.exists()) {
                if(!d.mkpath(dir)) {
                    qDebug() << "Can't create directory " << d.absolutePath();
                    continue;
                }
            }
            QString pathLink = dir + file.replace(".desktop",".lnk").replace('/','_');
            QFile f(pathLink);
            if(f.exists()) {
                if(!f.remove()) {
                    qDebug() << "Can't remove already existant file " << f.fileName();
                    continue;
                }
            }

            if(!CreateLink(exec, pathLink, "description", installDir)) {
                qDebug() << "Can't create link!" << exec << pathLink;
                continue;
            }
            registry.setValue("exec", pathLink);
        }
    }
    // note: this method should be called after installing when the related setting page
    //       entry is checked
    return true;
}

/* Removes all Entries from a specified category */
bool removeStartMenuEntries(const QString &dir, const QString &category)
{
    QSettings registry("kdewin-installer");
    registry.beginGroup("StartMenuEntries");
    registry.beginGroup(category);
    const QStringList keys = registry.allKeys();

    Q_FOREACH(const QString &key, keys) {
        registry.remove(key);
        QFile::remove(registry.value(key).toString());
    }
    return true;
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

void myMessageOutput(QtMsgType type, const char *msg)
{
    QFile log(Settings::instance().downloadDir()+"/kdewin-installer.log");
    log.open(QIODevice::Append);

    log.write(QDateTime::currentDateTime().toString("[yyyy-MM-dd hh:mm:ss] ").toLocal8Bit().data());
    switch (type) {
     case QtDebugMsg:
         log.write("Debug:");
         log.write(msg);
         log.write("\n");
         log.flush();
         break;
     case QtWarningMsg:
         log.write("Warning:");
         log.write(msg);
         log.write("\n");
         log.flush();
         break;
     case QtCriticalMsg:
         log.write("Critical:");
         log.write(msg);
         log.write("\n");
         log.flush();
         break;
     case QtFatalMsg:
         log.write("Fatal:");
         log.write(msg);
         log.write("\n");
         log.flush();
         abort();
    }
    log.close();
}

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

/**
 redirect all Qt debug, warning and error messages to a file
*/
void setMessageHandler()
{
    qInstallMsgHandler(myMessageOutput);
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
