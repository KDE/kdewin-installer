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
	    QString MimeType = settings.value("Desktop Entry/Mime Type").toString();
	    QString GenericName = settings.value("Desktop Entry/GenericName").toString();
	    QString Exec = settings.value("Desktop Entry/Exec").toString();
	    QString Icon = settings.value("Desktop Entry/Icon").toString();
			QString Categories = settings.value("Desktop Entry/Categories").toString();
			
			qDebug() << name << MimeType << GenericName << Exec << Icon << Categories; 
    }
 		// for all found files create entry in windows startmenu 
		// question: how to do ? 
		// note: this method should be called after installing when the related setting page 
		//       entry is checked 
}

bool removeStartMenuEntries(const QString &dir, const QString &category)
{
		// delete all installed startmenu for this package 
		// question: how to store created start menu entries 
}
