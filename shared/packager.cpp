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
#include <QFile>
#include <QFileInfo>

#include "packager.h"
#include "quazip.h" 
#include "quazipfile.h" 


// FIXME: generate manifest files 
// FIXME: do not display full path only beginning from given dir

#ifndef QZIP_BUFFER
# define QZIP_BUFFER (256 * 1024)
#endif


Packager::Packager(const QString &packageName, const QString &packageVersion, const QString &packageNotes)
: m_name(packageName), m_version(packageVersion), m_notes(packageNotes)
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

bool Packager::generateFileList(QStringList &fileList, const QString &dir, const QString &filter, const QString &exclude)
{
   // create a QListQRegExp
   QStringList sl = exclude.split(';');
   QList<QRegExp> rxList;

   // FIXME: add m_excludeList, or use only m_excludeList ?

   QStringList::ConstIterator it = sl.constBegin();
   for( ; it != sl.constEnd(); ++it) {
       QRegExp rx(*it);
       rx.setPatternSyntax(QRegExp::Wildcard);

       rxList += rx;
   }

   return generateFileList(fileList, dir, QString(), filter, rxList);
}

bool Packager::generateFileList(QStringList &fileList, const QString &root, const QString &subdir, const QString &filter, const QList<QRegExp> &excludeList)
{
   QDir d;
   if(subdir.isEmpty())
       d = QDir(root);
   else
       d = QDir(root + '/' + subdir);
   if (!d.exists()) {
       qDebug() <<"Can't read directory" << QDir::convertSeparators(d.absolutePath());
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
       fileList += dir + "/manifest/"+m_name+"-"+m_version+"-bin.mft";
       fileList += dir + "/manifest/"+m_name+"-"+m_version+"-bin.ver";
       return true;
   }        
   else if (type == Packager::LIB)
   {
       fileList.clear();
       generateFileList(fileList,dir + "/lib","*.dll.a","*.bak");
       generateFileList(fileList,dir + "/include","*.*","*.bak");
       generateFileList(fileList,dir + "/manifest","*-lib.*","*.bak");
       fileList += dir + "/manifest/"+m_name+"-"+m_version+"-lib.mft";
       fileList += dir + "/manifest/"+m_name+"-"+m_version+"-lib.ver";
       return true;
   }
   else if (type == Packager::DOC)
   {
       // FIXME: add doc package generating
       fileList.clear();
       //fileList += dir + "/manifest/"+m_name+"-"+m_version+"-doc.mft";
       //fileList += dir + "/manifest/"+m_name+"-"+m_version+"-doc.ver";
       return true;
   }
   else if (type == Packager::SRC)
   {
       // FIXME: add src package generating 
       fileList.clear();
       //fileList += dir + "/manifest/"+m_name+"-"+m_version+"-src.mft";
       //fileList += dir + "/manifest/"+m_name+"-"+m_version+"-src.ver";
       return true;
   }
}

bool Packager::createManifestFiles(QStringList &fileList, const QString &dir, Packager::Type type)
{
/*
 does not compile 
   if (type == Packager::BIN) 
   {
       QFile mftFile(dir+"/"+m_name+"-"+m_version+"-bin.mft");
       mftFile.open(QIODevice::WriteOnly);
       mftFile.write(fileList.join("\n"));
       mftFile.close();
       QFile verFile(dir+"/"+m_name+"-"+m_version+"-bin.ver");
       verFile.open(QIODevice::WriteOnly);
       verFile.write(m_name + " " + m_version+": Binaries\n");
       verFile.write(m_name + ": " + m_notes+"\n");
       verfile.close();
   } 
   else if (type == Packager::LIB)
   {
       QFile mftFile(dir+"/"+m_name+"-"+m_version+"-lib.mft");
       mftFile.open(QIODevice::WriteOnly);
       mftFile.write(fileList.join("\n"));
       mftFile.close();
       QFile verFile(dir+"/"+m_name+"-"+m_version+"-lib.ver");
       verFile.open(QIODevice::WriteOnly);
       verFile.write(m_name + " " + m_version+": developer files\n");
       verFile.write(m_name + ": " + m_notes+"\n");
       verFile.close();
   } 
   else if (type == Packager::DOC)
   {
       QFile mftFile(dir+"/"+m_name+"-"+m_version+"-doc.mft");
       mftFile.open(QIODevice::WriteOnly);
       mftFile.write(fileList.join("\n"));
       mftFile.close();
       QFile verFile(dir+"/"+m_name+"-"+m_version+"-doc.ver");
       verFile.open(QIODevice::WriteOnly);
       verFile.write(m_name + " " + m_version+": documentation\n");
       verFile.write(m_name + ": " + m_notes+"\n");
       verfile.close();
   } 
   else if (type == Packager::SRC)
   {
       QFile mftFile(dir+"/"+m_name+"-"+m_version+"-src.mft");
       mftFile.open(QIODevice::WriteOnly);
       mftFile.write(fileList.join("\n"));
       mftFile.close();
       QFile verFile(dir+"/"+m_name+"-"+m_version+"-src.ver");
       verFile.open(QIODevice::WriteOnly);
       verFile.write(m_name + " " + m_version+": source code\n");
       verFile.write(m_name + ": " + m_notes+"\n");
       verFile.close();
   } 
*/
   return true;    
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
    createManifestFiles(fileList,dir,Packager::BIN);
    if (fileList.size() > 0)
        createZipFile(zipFileName,fileList,dir);

    zipFileName = m_name+"-"+m_version+"-lib.zip";
    generatePackageFileList(fileList,dir,Packager::LIB);
    createManifestFiles(fileList,dir,Packager::LIB);
    if (fileList.size() > 0)
        createZipFile(zipFileName,fileList,dir);

    zipFileName = m_name+"-"+m_version+"-doc.zip";
    generatePackageFileList(fileList,dir,Packager::DOC);
    createManifestFiles(fileList,dir,Packager::DOC);
    if (fileList.size() > 0)
        createZipFile(zipFileName,fileList,dir);

    zipFileName = m_name+"-"+m_version+"-src.zip";    
    generatePackageFileList(fileList,dir,Packager::SRC);
    createManifestFiles(fileList,dir,Packager::SRC);
    if (fileList.size() > 0)
        createZipFile(zipFileName,fileList,dir);
}
