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

#include <QDebug>
#include <QDir>
#include <QFileInfo>
#include <QSettings>

#include "misc.h"

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

#include <windows.h>
#include <windowsx.h>
#include <objbase.h>
#include <shlobj.h>
#include <initguid.h>

// from http://msdn.microsoft.com/library/default.asp?url=/library/en-us/shellcc/platform/shell/programmersguide/shell_int/shell_int_programming/shortcuts/shortcut.asp
// CreateLink - uses the Shell's IShellLink and IPersistFile interfaces 
//              to create and store a shortcut to the specified object. 
//
// Returns the result of calling the member functions of the interfaces. 
//
// Parameters:
// lpszPathObj  - address of a buffer containing the path of the object. 
// lpszPathLink - address of a buffer containing the path where the 
//                Shell link is to be stored. 
// lpszDesc     - address of a buffer containing the description of the 
//                Shell link. 

HRESULT CreateLink(LPCSTR lpszPathObj, LPCSTR lpszPathLink, LPCSTR lpszDesc) 
{ 
    HRESULT hres; 
    IShellLink* psl; 
 
    // Get a pointer to the IShellLink interface. 
    hres = CoCreateInstance(CLSID_ShellLink, NULL, CLSCTX_INPROC_SERVER, 
                            IID_IShellLink, (LPVOID*)&psl); 
    if (SUCCEEDED(hres)) 
    { 
        IPersistFile* ppf; 
 
        // Set the path to the shortcut target and add the description. 
        psl->SetPath(lpszPathObj); 
        psl->SetDescription(lpszDesc); 
 
        // Query IShellLink for the IPersistFile interface for saving the 
        // shortcut in persistent storage. 
        hres = psl->QueryInterface(IID_IPersistFile, (LPVOID*)&ppf); 
 
        if (SUCCEEDED(hres)) 
        { 
            WCHAR wsz[MAX_PATH]; 
 
            // Ensure that the string is Unicode. 
            MultiByteToWideChar(CP_ACP, 0, lpszPathLink, -1, wsz, MAX_PATH); 
			
            // Add code here to check return value from MultiByteWideChar 
            // for success.
 
            // Save the link by calling IPersistFile::Save. 
            hres = ppf->Save(wsz, TRUE); 
            ppf->Release(); 
        } 
        else 
        	qDebug() << "error" ;
        psl->Release(); 
    } 
    return hres; 
}


/* 
 * create start menu entries from installed desktop files 
 */
bool createStartMenuEntries(const QString &dir, const QString &category)
{
		// use Packager::generateFileList to build a list of files with ending .desktop 
		// -> generateFileList should be independent from Packager 
		QStringList fileList;
		generateFileList(fileList,dir,"","*.desktop");
		
		for(int i = 0; i < fileList.size(); i++) 
		{
		  QSettings settings(dir + "/" + fileList[i], QSettings::IniFormat);
	    QString name = settings.value("Desktop Entry/Name").toString();
	    QString mimeType = settings.value("Desktop Entry/Mime Type").toString();
	    QString genericName = settings.value("Desktop Entry/GenericName").toString();
	    QString exec = settings.value("Desktop Entry/Exec").toString();
	    QString icon = settings.value("Desktop Entry/Icon").toString();
			QString categories = settings.value("Desktop Entry/Categories").toString();
			
			if (!exec.isEmpty()) 
			{
    			qDebug() << fileList[i].replace(".desktop",".lnk").replace("/","_") << name << mimeType << genericName << exec << icon << categories; 
          CreateLink(exec.toAscii().data(),fileList[i].replace(".desktop",".lnk").replace("/","_").toAscii().data(),"description");
      }
    }
 		// for all found files create entry in windows startmenu 
		// question: how to do ? 
		// note: this method should be called after installing when the related setting page 
		//       entry is checked 
    return true;
}

bool removeStartMenuEntries(const QString &dir, const QString &category)
{
		// delete all installed startmenu for this package 
		// question: how to store created start menu entries 
    return true;
}
