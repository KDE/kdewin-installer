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


bool Packager::createZipFile(const QStringList &files, const QString &zipFile)
{
    QuaZip zip(zipFile);
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
       fileName.replace(m_root+"\\","");
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



Packager::Packager()
{
    m_excludeList << "*.bak";
}


bool Packager::generateFileList(const QString &dir, const QString &filter, const QString &exclude)
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

   return generateFileList(dir, QString(), filter, rxList);
}

bool Packager::generateFileList(const QString &root, const QString &subdir, const QString &filter, const QList<QRegExp> &excludeList)
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
           generateFileList(root, fn, filter, excludeList);
       }
       else 
           m_fileList.append(subdir + '/' + fn);
   }
   return true;
}
bool Packager::generatePackageFileList(const QString &dir,Packager::Type type)
{
   m_root = QDir::convertSeparators(dir);
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
   return true;
}

bool Packager::makePackage(const QString &dir, const QString &packageName, const QString &packageVersion)
{
    // generate file lists
    // create manifest files 
    // create zip file 
    QString zipFileName;
    zipFileName = packageName+"-"+packageVersion+"-bin.zip";
    generatePackageFileList(dir,Packager::BIN);
    if (m_fileList.size() > 0)
        createZipFile(zipFileName,m_fileList);

    zipFileName = packageName+"-"+packageVersion+"-lib.zip";
    generatePackageFileList(dir,Packager::LIB);
    if (m_fileList.size() > 0)
        createZipFile(zipFileName,m_fileList);

    zipFileName = packageName+"-"+packageVersion+"-doc.zip";    
    generatePackageFileList(dir,Packager::DOC);
    if (m_fileList.size() > 0)
        createZipFile(zipFileName,m_fileList);

    zipFileName = packageName+"-"+packageVersion+"-src.zip";    
    generatePackageFileList(dir,Packager::SRC);
    if (m_fileList.size() > 0)
        createZipFile(zipFileName,m_fileList);
    return true;
}