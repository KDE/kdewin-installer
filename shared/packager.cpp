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
#include <QProcess>
#include <QBuffer>
#include <QTextStream>

#include "packager.h"
#include "quazip.h" 
#include "quazipfile.h" 
#include "misc.h"

#ifndef QZIP_BUFFER
# define QZIP_BUFFER (256 * 1024)
#endif

Packager::Packager(const QString &packageName, const QString &packageVersion, const QString &packageNotes)
: m_name(packageName), m_version(packageVersion), m_notes(packageNotes)
{
       qDebug() << m_name;
       qDebug() << m_version;
       qDebug() << m_notes;
       m_verbose = true;
}

bool Packager::createZipFile(const QString &baseName, const QString &root, const QStringList &files, const QList<MemFile> &memFiles)
{
    QuaZip zip(baseName + ".zip");
    if(!zip.open(QuaZip::mdCreate)) {
       qWarning("createZipFile(): zip.open(): %d", zip.getZipError());
       return false;
    }
       
    QFile inFile;
    QuaZipFile outFile(&zip);
       
    for (int l = 0; l < files.size(); l++)
    {
       inFile.setFileName(root + '/' + files[l]);
   
       if(!inFile.open(QIODevice::ReadOnly)) 
       {
           qWarning("createZipFile(): inFile.open(): %s", inFile.errorString().toLocal8Bit().constData());
           return false;
       }
       if(!outFile.open(QIODevice::WriteOnly, QuaZipNewInfo(files.at(l), inFile.fileName()))) 
       {
           qWarning("createZipFile(): outFile.open(): %d", outFile.getZipError());
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
          qWarning("createZipFile(): outFile.putChar(): %d", outFile.getZipError());
          return false;
       }
       outFile.close();
       if(outFile.getZipError()!=UNZ_OK) 
       {
          qWarning("createZipFile(): outFile.close(): %d", outFile.getZipError());
          return false;
       }
       inFile.close();
    }

    QList<MemFile>::ConstIterator it = memFiles.constBegin();
    for( ; it != memFiles.constEnd(); ++it) {
       if(!outFile.open(QIODevice::WriteOnly, QuaZipNewInfo((*it).filename))) 
       {
           qWarning("createZipFile(): outFile.open(): %d", outFile.getZipError());
           return false;
       }
       outFile.write((*it).data, (*it).data.size());
       if(outFile.getZipError()!=UNZ_OK) 
       {
          qWarning("createZipFile(): outFile.putChar(): %d", outFile.getZipError());
          return false;
       }
       outFile.close();
       if(outFile.getZipError()!=UNZ_OK) 
       {
          qWarning("createZipFile(): outFile.close(): %d", outFile.getZipError());
          return false;
       }
    }
    
    zip.close();
    if(zip.getZipError()!=0) {
        qWarning("createZipFile(): zip.close(): %d", zip.getZipError());
        return false;
        if(outFile.getZipError()!=UNZ_OK) 
        {
            qWarning("createZipFile(): outFile.putChar(): %d", outFile.getZipError());
            return false;
        }
        outFile.close();
        if(outFile.getZipError()!=UNZ_OK) 
        {
            qWarning("createZipFile(): outFile.close(): %d", outFile.getZipError());
            return false;
        }
    }
    return true;
} 

bool Packager::generatePackageFileList(QStringList &fileList, const QString &dir, Packager::Type type)
{
    fileList.clear();
    if (m_name.startsWith("qt") || m_name.startsWith("q++") || m_name.startsWith("q.."))
        switch (type) {
            case BIN:
                generateFileList(fileList, dir, "bin", "*.dll qdbus.exe dbus-viewer.exe", "*d.dll *d4.dll");
                generateFileList(fileList, dir, "plugins", "*.dll","*d.dll *d1.dll");
                generateFileList(fileList, dir, "translations",  "*.qm");
                return true;
            case LIB:
                generateFileList(fileList, dir, "bin",   "*.exe *.bat *d4.dll", "assistant.exe qtdemo.exe qdbus.exe dbus-viewer.exe");
                generateFileList(fileList, dir, ".",     ".qmake.cache");
                generateFileList(fileList, dir, "mkspecs",  "qconfig.pri");
                if (m_name.endsWith("mingw"))
                    generateFileList(fileList, dir, "mkspecs/win32-g++", "*.*");
                // FIXME: add other compiler support 
                generateFileList(fileList, dir, "lib",     "*.a");
                generateFileList(fileList, dir, "include", "*","*_p.h");
                generateFileList(fileList, dir, "src/corelib", "*.h", "*_p.h");
                generateFileList(fileList, dir, "src/gui", "*.h", "*_p.h");
                generateFileList(fileList, dir, "src/qt3support", "*.h", "*_p.h");
                generateFileList(fileList, dir, "src/xml", "*.h", "*_p.h");
                generateFileList(fileList, dir, "src/network", "*.h", "*_p.h");
                generateFileList(fileList, dir, "src/svg", "*.h", "*_p.h");
                generateFileList(fileList, dir, "src/opengl", "*.h", "*_p.h");
                qDebug() << fileList;
                return true;
            case DOC:
                generateFileList(fileList, dir, "bin", "qtdemo.exe assistant.exe");
                generateFileList(fileList, dir, "doc", "*.*");
                return true;
            case SRC:
                // not supported yet
                //generateFileList(fileList, dir, "src", "*.*");
                return true;
            case NONE:
                generateFileList(fileList, dir, "", "*.*", "manifest");
                return true;
            default:
                break;
        }
    else    
        switch (type) {
            case BIN:
                generateFileList(fileList, dir, "bin",   "*.exe *.dll *.bat");
                generateFileList(fileList, dir, "lib",   "*.dll");
                generateFileList(fileList, dir, "share", "*.*");
                generateFileList(fileList, dir, "data",  "*.*");
                generateFileList(fileList, dir, "etc",   "*.*");
                return true;
            case LIB:
                generateFileList(fileList, dir, "lib",     "*.dll.a");
                generateFileList(fileList, dir, "include", "*.*");
                return true;
            case DOC:
                generateFileList(fileList, dir, "doc", "*.*");
                generateFileList(fileList, dir, "man", "*.*");
                return true;
            case SRC:
                generateFileList(fileList, dir, "src", "*.*");
                return true;
            case NONE:
                generateFileList(fileList, dir, "", "*.*", "manifest");
                return true;
            default:
                break;
       }
   return false;
}

bool Packager::createManifestFiles(QStringList &fileList, Packager::Type type, QList<MemFile> &manifestFiles)
{
    QString fileNameBase = getBaseName(type); 
    QString descr;
    MemFile mf;
    
    manifestFiles.clear();

    switch(type) {
        case Packager::BIN:
            descr = "Binaries";
            break;
        case Packager::LIB:
            descr = "developer files";
            break;
        case Packager::DOC:
            descr = "documentation";
            break;
        case Packager::SRC:
            descr = "source code";
            break;
        case Packager::NONE:
            descr = "complete package";
            break;
        default:
            break;
    } 

    QBuffer b(&mf.data);
    b.open(QIODevice::WriteOnly);
    QTextStream out(&b);
    out << fileList.join("\n") << '\n'
        << "manifest/" + fileNameBase + ".mft" << '\n'
        << "manifest/" + fileNameBase + ".ver" << '\n';
    b.close();
    mf.filename = "manifest/" + fileNameBase + ".mft";
    manifestFiles += mf;

    mf.data.clear();

    b.setBuffer(&mf.data);
    b.open(QIODevice::WriteOnly);
    out.setDevice(&b);
    out << m_name + ' ' + m_version + ' ' + descr << '\n'
        << m_name + ": " + m_notes + '\n';
    b.close();
    mf.filename = "manifest/" + fileNameBase + ".ver";
    manifestFiles += mf;
    
    return true;    
}

bool Packager::makePackage(const QString &dir, const QString &destdir, bool bComplete)
{
    // generate file lists
    // create manifest files 
    // create zip file 

    QStringList fileList; 
    QList<MemFile> manifestFiles;
    if (m_verbose)
        qDebug() << "creating bin package" << getBaseName(Packager::BIN); 
    generatePackageFileList(fileList, dir, Packager::BIN);
    createManifestFiles(fileList, Packager::BIN, manifestFiles);    
    if (fileList.size() > 0)
        createZipFile(getBaseName(Packager::BIN), dir, fileList, manifestFiles);
    else
        qDebug() << "no binary files found!";

    if (m_verbose)
        qDebug() << "creating lib package" << getBaseName(Packager::LIB); 
    generatePackageFileList(fileList, dir, Packager::LIB);
    createManifestFiles(fileList, Packager::LIB, manifestFiles);
    if (fileList.size() > 0)
        createZipFile(getBaseName(Packager::LIB), dir, fileList, manifestFiles);

    if (m_verbose)
        qDebug() << "creating lib package" << getBaseName(Packager::DOC); 
    generatePackageFileList(fileList, dir, Packager::DOC);
    createManifestFiles(fileList, Packager::DOC, manifestFiles);
    if (fileList.size() > 0)
        createZipFile(getBaseName(Packager::DOC), dir, fileList, manifestFiles);

    if (m_verbose)
        qDebug() << "creating lib package" << getBaseName(Packager::SRC); 
    generatePackageFileList(fileList, dir, Packager::SRC);
    createManifestFiles(fileList, Packager::SRC, manifestFiles);
    if (fileList.size() > 0)
        createZipFile(getBaseName(Packager::SRC), dir, fileList, manifestFiles);

    if(bComplete) {
        if (m_verbose)
            qDebug() << "creating complete package" << getBaseName(Packager::NONE); 
        generatePackageFileList(fileList, dir, Packager::NONE);
        createManifestFiles(fileList, Packager::NONE, manifestFiles);
        if (fileList.size() > 0)
            createZipFile(getBaseName(Packager::NONE), dir, fileList, manifestFiles);
    }
    return true;
}

QString Packager::getBaseName(Packager::Type type)
{
    QString name = m_name + '-' + m_version;

    switch(type) {
        case BIN:
            name += "-bin";
            break;
        case LIB:
            name += "-lib";
            break;
        case DOC:
            name += "-doc";
            break;
        case SRC:
            name += "-src";
            break;
        default:
            break;
    };
    return name;
}

bool Packager::stripFiles(const QString &dir)
{
    QStringList fileList; 
    generateFileList(fileList,dir,"bin","*.exe *.dll","*d.dll *d4.dll");
    for (int i = 0; i < fileList.size(); i++) 
    {
        QFileInfo fi(dir + "/" + fileList.at(i));
        // FIXME: add file in use detection, isWritable() returns not the required state
        // if no windows related functions is available at least parsing the output for 
        // the string "strip: unable to rename" indicates this condition
#if 0
        if (!fi.isWritable())
        {
            qDebug() << "file " << fi.absoluteFilePath() << " is in use"; 
            exit(1);
        }
#endif
        qDebug() << "strip -s " + fi.absoluteFilePath(); 
        QProcess::execute("strip -s " + fi.absoluteFilePath());
    }
    return true;
}
// create debug files for mingw
//  see http://www.daemon-systems.org/man/strip.1.html

bool Packager::createDebugFiles(const QString &dir)
{
    QStringList fileList; 
    generateFileList(fileList,dir,"bin","*.exe *.dll");
    generateFileList(fileList,dir,"lib","*.exe *.dll");
    for (int i = 0; i < fileList.size(); i++) 
    {
        QFileInfo fi(dir + '/' + fileList.at(i));
        // FIXME: add file in use detection, isWritable() returns not the required state
        // if no windows related functions is available at least parsing the output for 
        // the string "strip: unable to rename" indicates this condition
#if 0
        if (!fi.isWritable())
        {
            qDebug() << "file " << fi.absoluteFilePath() << " is in use"; 
            exit(1);
        }
#endif
        QProcess::execute("objcopy --only-keep-debug " + fi.absoluteFilePath() + " " + fi.absoluteFilePath() + ".dbg");
        QProcess::execute("objcopy --strip-debug " + fi.absoluteFilePath());
        QProcess::execute("objcopy --add-gnu-debuglink=" + fi.absoluteFilePath() + ".dbg " + fi.absoluteFilePath());
    }
    return true;
}
