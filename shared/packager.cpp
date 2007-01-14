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

#include "packager.h"
#include "quazip.h" 
#include "quazipfile.h" 


// FIXME: generate manifest files 
// FIXME: do not display full path only beginning from given dir

#ifndef QZIP_BUFFER
# define QZIP_BUFFER (256 * 1024)
#endif


Packager::Packager(const QString &packageName, const QString &packageVersion)
: m_name(packageName), m_version(packageVersion)
{
}

bool Packager::createZipFile(const QString &fileName, const QStringList &files, const QString &rootdir)
{
    QuaZip zip(fileName);
    if(!zip.open(QuaZip::mdCreate)) {
       qWarning("testCreate(): zip.open(): %d", zip.getZipError());
       return false;
    }
       
    QFile inFile;
    QuaZipFile outFile(&zip);
       
    for (int l = 0; l < files.size(); l++)
    {
       inFile.setFileName(files.at(l));
   
       if(!inFile.open(QIODevice::ReadOnly)) 
       {
           qWarning("testCreate(): inFile.open(): %s", inFile.errorString().toLocal8Bit().constData());
           return false;
       }
       QString fileName = QDir::convertSeparators(inFile.fileName());
       // FIXME: check if trailing '\' is already there 
       fileName.replace(QDir::convertSeparators(rootdir)+"\\","");
       if(!outFile.open(QIODevice::WriteOnly, QuaZipNewInfo(fileName, inFile.fileName()))) 
       {
           qWarning("testCreate(): outFile.open(): %d", outFile.getZipError());
           return false;
       }
       // copy data
       // FIXME: check for not that huge filesize ?
       qint64 iBytesRead;
       QByteArray ba;
       ba.resize(QZIP_BUFFER);

       while((iBytesRead = inFile.read(ba.data(), QZIP_BUFFER)) > 0)
          outFile.write(ba.data(), iBytesRead);

       if(outFile.getZipError()!=UNZ_OK) 
       {
          qWarning("testCreate(): outFile.putChar(): %d", outFile.getZipError());
          return false;
       }
       outFile.close();
       if(outFile.getZipError()!=UNZ_OK) 
       {
          qWarning("testCreate(): outFile.close(): %d", outFile.getZipError());
          return false;
       }
       inFile.close();
   }
    
    zip.close();
    if(zip.getZipError()!=0) {
        qWarning("testCreate(): zip.close(): %d", zip.getZipError());
        return false;
    }
    return true;
} 

bool Packager::generateFileList(QStringList &result, const QString &dir, const QString &filter, const QString &exclude)
{
   QDir d(dir);
   if ( !d.exists() ) 
   {
       qDebug() <<"Skipping not existing directory" << QDir::convertSeparators( dir );
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
           generateFileList(result, d.absolutePath() + "/" + fi.fileName(), filter, exclude);
       else 
       {
           result.append( QDir::convertSeparators(dir + "/" + fi.fileName()) );
       }
   }
   return true;
}

bool Packager::generatePackageFileList(QStringList &fileList, const QString &dir, Packager::Type type)
{
   if (type == Packager::BIN)
   {
       fileList.clear();
       generateFileList(fileList,dir + "/bin","*.exe *.dll","*.bak");
       generateFileList(fileList,dir + "/lib","*.dll","*.bak");
       generateFileList(fileList,dir + "/share","*.*","*.bak");
       generateFileList(fileList,dir + "/data","*.*","*.bak");
       generateFileList(fileList,dir + "/etc","*.*","*.bak");
       generateFileList(fileList,dir + "/manifest","*-bin.*");
       return true;
   }        
   else if (type == Packager::LIB)
   {
       fileList.clear();
       generateFileList(fileList,dir + "/lib","*.dll.a","*.bak");
       generateFileList(fileList,dir + "/include","*.*","*.bak");
       generateFileList(fileList,dir + "/manifest","*-lib.*");
       return true;
   }
   else if (type == Packager::DOC)
   {
       // FIXME: add doc package generating
       fileList.clear();
       return true;
   }
   else if (type == Packager::SRC)
   {
       // FIXME: add src package generating 
       fileList.clear();
       return true;
   }
}

bool Packager::makePackage(const QString &dir, const QString &destdir)
{
    // generate file lists
    // create manifest files 
    // create zip file 
    QStringList fileList; 
    
    QString zipFileName;
    zipFileName = m_name+"-"+m_version+"-bin.zip";
    generatePackageFileList(fileList,dir,Packager::BIN);
    if (fileList.size() > 0)
        createZipFile(zipFileName,fileList,dir);

    zipFileName = m_name+"-"+m_version+"-lib.zip";
    generatePackageFileList(fileList,dir,Packager::LIB);
    if (fileList.size() > 0)
        createZipFile(zipFileName,fileList,dir);

    zipFileName = m_name+"-"+m_version+"-doc.zip";
    generatePackageFileList(fileList,dir,Packager::DOC);
    if (fileList.size() > 0)
        createZipFile(zipFileName,fileList,dir);

    zipFileName = m_name+"-"+m_version+"-src.zip";    
    generatePackageFileList(fileList,dir,Packager::SRC);
    if (fileList.size() > 0)
        createZipFile(zipFileName,fileList,dir);
}
