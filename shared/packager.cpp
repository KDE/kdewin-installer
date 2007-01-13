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

#include <packager.h> 


// FIXME: generate manifest files 



Packager::Packager()
{
    m_excludeList << "*.bak";
}


bool Packager::generateFileList(const QString &dir, const QString &filter, const QString &exclude)
{
   QDir d(dir);
   if ( !d.exists() ) 
   {
       qDebug() <<"Can't read directory" << QDir::convertSeparators( dir );
       return false;
   }
   d.setFilter( QDir::NoDotAndDotDot | QDir::AllEntries | QDir::AllDirs);
   d.setNameFilters(filter.split(' '));
   d.setSorting( QDir::Name );
   // FIXME: use wildcard
   QString excludeWithoutWildCard = exclude;
   QStringList excludeList = excludeWithoutWildCard.replace("*","").split(' ');
    
   const QFileInfoList list = d.entryInfoList();
   QFileInfo fi;
     
   for (int i = 0; i < list.size(); i++) {
       QFileInfo fi = list.at(i);

       bool excluded = false;
       for(int j = 0; j < excludeList.size(); j++)
       {
           if ( excludeList.at(j).size() > 0 && fi.fileName().contains(excludeList.at(j)) )
               excluded = true;
               break;
       }
       if (excluded)
           continue;
           
       if ( fi.isDir() )
           generateFileList( d.absolutePath() + "/" + fi.fileName(), filter, exclude);
       else 
       {
           m_fileList.append( dir + "/" + fi.fileName() );
       }
   }
}
bool Packager::generatePackageFileList(const QString &dir,Packager::Type type)
{
   if (type == Packager::BIN)
   {
       m_fileList.clear();
       generateFileList(dir + "/bin","*.exe *.dll","*.bak");
       generateFileList(dir + "/lib","*.dll","*.bak");
       generateFileList(dir + "/share","*.*","*.bak");
       generateFileList(dir + "/data","*.*","*.bak");
       generateFileList(dir + "/etc","*.*","*.bak");
       generateFileList(dir + "/manifest","*-bin.*");
       return true;
   }        
   else if (type == Packager::LIB)
   {
       m_fileList.clear();
       generateFileList(dir + "/lib","*.dll.a","*.bak");
       generateFileList(dir + "/include","*.*","*.bak");
       generateFileList(dir + "/manifest","*-lib.*");
       return true;
   }
   else if (type == Packager::DOC)
   {
       m_fileList.clear();
       return false;
   }
   else if (type == Packager::SRC)
   {
       m_fileList.clear();
       return false;
   }
}

bool Packager::makePackage(const QString &dir, const QString &zipFileName)
{
    // generate file lists
    // create manifest files 
    // create zip file 
}
