/****************************************************************************
**
** Copyright (C) 2006 Ralf Habacker. All rights reserved.
**
** This file is part of the KDE installer for windows
**
** This file may be used under the terms of the GNU General Public
** License version 2.0 as published by the Free Software Foundation
** and appearing in the file LICENSE.GPL included in the packaging of
** this file.  Please review the following information to ensure GNU
** General Public Licensing requirements will be met:
** http://www.trolltech.com/products/qt/opensource.html
**
** If you are unsure which license is appropriate for your use, please
** review the following information:
** http://www.trolltech.com/products/qt/licensing.html or contact the
** sales department at sales@trolltech.com.
**
** This file is provided AS IS with NO WARRANTY OF ANY KIND, INCLUDING THE
** WARRANTY OF DESIGN, MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE.
**
****************************************************************************/

#include <windows.h>
#include <windowsx.h>
#include <objbase.h>
#include <shlobj.h>
#include <initguid.h>

#include <QDebug>
#include <QDir>
#include <QFileInfo>
#include <QSettings>

#include "misc.h"

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

bool generateFileList(QStringList &fileList, const QString &root, const QString &subdir, const QString &filter, const QString &exclude)
{
   // create a QListQRegExp
   QStringList sl = exclude.split(' ');
   QList<QRegExp> rxList;

   if(!sl.contains("*.bak"))
       sl += "*.bak";

   QStringList::ConstIterator it = sl.constBegin();
   for( ; it != sl.constEnd(); ++it) {
       QRegExp rx(*it);
       rx.setPatternSyntax(QRegExp::Wildcard);

       rxList += rx;
   }

   return ::generateFileList(fileList, root, subdir, filter, rxList);
}

bool generateFileList(QStringList &fileList, const QString &root, const QString &subdir, const QString &filter, const QList<QRegExp> &excludeList)
{
   QDir d;
   if(subdir.isEmpty())
       d = QDir(root);
   else
       d = QDir(root + '/' + subdir);
   if (!d.exists()) {
       qDebug() << "Can't read directory" << QDir::convertSeparators(d.absolutePath());
       return false;
   }
   d.setFilter(QDir::NoDotAndDotDot | QDir::AllEntries | QDir::AllDirs);
   d.setNameFilters(filter.split(' '));
   d.setSorting(QDir::Name);

   QFileInfoList list = d.entryInfoList();
   QFileInfo fi;
     
   for (int i = 0; i < list.size(); i++) {
       const QFileInfo &fi = list[i];
       QString fn = fi.fileName();

       bool bFound = false;
       QList<QRegExp>::ConstIterator it = excludeList.constBegin();
       for( ; it != excludeList.constEnd(); ++it) {
           if((*it).exactMatch(fn)) {
               bFound = true;
               break;
           }
           if (bFound)
               break;
       }
       if (bFound)
           continue;

       if (fi.isDir()) {
           if(!subdir.isEmpty())
               fn = subdir + '/' + fn;
           generateFileList(fileList, root, fn, filter, excludeList);
       }
       else 
           fileList.append(subdir + '/' + fn);
   }
   return true;
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

    // Get a pointer to the IShellLink interface. 
    hres = CoCreateInstance(CLSID_ShellLink, NULL, CLSCTX_INPROC_SERVER, 
                            IID_IShellLink, (LPVOID*)&psl); 
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
            qDebug() << "error setting description for link to " << description;
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
    } 
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
bool createStartMenuEntries(const QString &dir, const QString &category)
{
    QStringList fileList;
    generateFileList(fileList,dir,"","*.desktop");
    
    for(int i = 0; i < fileList.size(); i++) 
    {
        QSettings registry("kdewin-installer");
        registry.beginGroup("StartMenuEntries");
        registry.beginGroup(category);

        QSettings settings(dir + '/' + fileList[i], QSettings::IniFormat);

        QString name = settings.value("Desktop Entry/Name").toString();
        QString mimeType = settings.value("Desktop Entry/Mime Type").toString();
        QString genericName = settings.value("Desktop Entry/GenericName").toString();
        // TODO: get full path here!
        QString exec = settings.value("Desktop Entry/Exec").toString();
        QString icon = settings.value("Desktop Entry/Icon").toString();
        QString categories = settings.value("Desktop Entry/Categories").toString();
        
        if (!exec.isEmpty()) 
        {
            QString p = getStartMenuPath(false);
            if(p.isEmpty()) {
                qDebug() << "Can't determine Start menu folder!";
                continue;
            }

            QString dir = p + '/' + category + '/';
            QDir d(dir);
            if(!d.exists()) {
                if(!d.mkdir(dir)) {
                    qDebug() << "Can't create directory " << d.absolutePath();
                    continue;
                }
            }
            QString pathLink = dir + fileList[i].replace(".desktop",".lnk").replace('/','_');
            QFile f(pathLink);
            if(f.exists()) {
                if(!f.remove()) {
                    qDebug() << "Can't remove already existant file " << d.absolutePath();
                    continue;
                }
            }

            if(!CreateLink(exec, pathLink, "description")) {
                qDebug() << "Can't create link!";
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
    QStringList keys = registry.allKeys();

    for(int i = 0; i < keys.size(); i++) {
        registry.remove(keys[i]);
        QFile::remove(registry.value(keys[i]).toString());
    }
    return true;
}

#if defined(__MINGW32__)
# define WIN32_CAST_CHAR (WCHAR*)
#else
# define WIN32_CAST_CHAR (LPCWSTR)
#endif

/**
 \return a value from MS Windows native registry.
 @param key is usually one of HKEY_CLASSES_ROOT, HKEY_CURRENT_USER, HKEY_LOCAL_MACHINE
        constants defined in WinReg.h. 
 @param subKey is a registry subkey defined as a path to a registry folder, eg.
        "SOFTWARE\\Microsoft\\Windows\\CurrentVersion\\Explorer\\Shell Folders"
        ('\' delimiter must be used)
 @param item is an item inside subKey or "" if default folder's value should be returned
 @param ok if not null, will be set to true on success and false on failure
*/
QString getWin32RegistryValue(RegKey akey, const QString& subKey, const QString& item, bool *ok)
{
#define FAILURE \
	{ if (ok) \
		*ok = false; \
	return QString(); }

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
	TCHAR *lszValue;
	DWORD dwType=REG_SZ;
	DWORD dwSize;

	if (ERROR_SUCCESS!=RegOpenKeyEx(key, WIN32_CAST_CHAR subKey.utf16(), 0, KEY_READ, &hKey))
		FAILURE;

	if (ERROR_SUCCESS!=RegQueryValueEx(hKey, WIN32_CAST_CHAR item.utf16(), NULL, NULL, NULL, &dwSize))
		FAILURE;

	lszValue = new TCHAR[dwSize];

	if (ERROR_SUCCESS!=RegQueryValueEx(hKey, WIN32_CAST_CHAR item.utf16(), NULL, &dwType, (LPBYTE)lszValue, &dwSize)) {
		delete [] lszValue;
		FAILURE;
	}
	RegCloseKey(hKey);

	QString res = QString::fromUtf16( (const ushort*)lszValue );
	delete [] lszValue;
	return res;
}

