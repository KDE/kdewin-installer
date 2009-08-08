/****************************************************************************
**
** Copyright (C) 2006-2009 Ralf Habacker. All rights reserved.
**
** This file is part of the KDE installer for windows
**
** This library is free software; you can redistribute it and/or
** modify it under the terms of the GNU Library General Public
** License version 2 as published by the Free Software Foundation.
**
** This library is distributed in the hope that it will be useful,
** but WITHOUT ANY WARRANTY; without even the implied warranty of
** MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE. See the GNU
** Library General Public License for more details.
**
** You should have received a copy of the GNU Library General Public License
** along with this library; see the file COPYING.LIB.   If not, write to
** the Free Software Foundation, Inc., 51 Franklin Street, Fifth Floor,
** Boston, MA 02110-1301, USA.
**
****************************************************************************/

#include "hashfile.h"
#include "packager.h"
#include "quazip.h"
#include "quazipfile.h"
#include "misc.h"

#include "tarfilter.h"
#include "bzip2iodevice.h"

#include <QBuffer>
#include <QDebug>
#include <QDir>
#include <QFile>
#include <QFileInfo>
#include <QProcess>
#include <QTextStream>

#ifndef QZIP_BUFFER
# define QZIP_BUFFER (1 * 1024 * 1024)
#endif

#ifdef Q_CC_MSVC
#define __PRETTY_FUNCTION__ __FUNCTION__
#endif

Packager::Packager(const QString &packageName, const QString &packageVersion, const QString &packageNotes)
  : m_name(packageName),
    m_version(packageVersion),
    m_notes(packageNotes),
    m_verbose(false),
    m_debugPackage(false),
    m_special(false),
    m_compMode(1)
{
}

Packager::~Packager()
{
    qDeleteAll(m_packagerInfo);
}

bool Packager::compressFiles(const QString &zipFile, const QString &filesRootDir,
                             const QList<InstallFile> &files, const QList<MemFile> &memFiles,
                             const QString &destRootDir , FileSizeInfo &sizeInfo)
{
  switch(m_compMode) {
    default:
    case 1:
      return createZipFile(zipFile, filesRootDir, files, memFiles, destRootDir, sizeInfo);
    case 2:
      return createTbzFile(zipFile, filesRootDir, files, memFiles, destRootDir, sizeInfo);
  }
}

#define ON_ERROR(i)   { qWarning("Error in %s():%d - %d", __PRETTY_FUNCTION__, __LINE__, i); return false; }
#define ON_ERROR_S(s) { qWarning("Error in %s():%d - %s", __PRETTY_FUNCTION__, __LINE__, s); return false; }
bool Packager::createTbzFile(const QString &zipFile, const QString &filesRootDir,
                              const QList<InstallFile> &files, const QList<MemFile> &memFiles,
                              const QString &destRootDir, FileSizeInfo &sizeInfo )
{
  QFile f(zipFile + ".tar.bz2");
  if(!f.open(QIODevice::WriteOnly))
    ON_ERROR_S(qPrintable(QString("Error opening file %1 for writing").arg(zipFile + ".tar.bz2")));

  BZip2IODevice bzip2(&f, 9);
  if(!bzip2.open(QIODevice::WriteOnly))
    ON_ERROR_S(qPrintable(QString("Error opening file %1 for writing (%2)").arg(zipFile + ".tar.bz2").arg(bzip2.errorString())));

  TarFilter tf(&bzip2);
  const QStringList paths = destRootDir.split('/', QString::SkipEmptyParts);
  QString curPath;
  Q_FOREACH(const QString &path, paths)
  {
    curPath = curPath.isEmpty() ? path + '/' : curPath + path + '/';
    if(!tf.addFile(filesRootDir, curPath))
      ON_ERROR_S(qPrintable(tf.lastError()));
  }
  
  sizeInfo.installedSize = 0;
  Q_FOREACH(const InstallFile &file, files)
  {
    QString in  = file.bAbsInputPath ? file.inputFile : filesRootDir + '/' + file.inputFile;
    QString out = destRootDir + (file.outputFile.isEmpty() ? file.inputFile : file.outputFile);
    QFileInfo fi(in);

    sizeInfo.installedSize += fi.size();
    
    if(!tf.addFile(in, out))
      ON_ERROR_S(qPrintable(tf.lastError()));
  }

  Q_FOREACH(const MemFile &file, memFiles)
  {
    sizeInfo.installedSize += file.data.size();
    if(!tf.addData(file.filename, file.data))
      ON_ERROR_S(qPrintable(tf.lastError()));
  }
  tf.writeEOS();

  bzip2.close();
  f.close();
  sizeInfo.compressedSize = f.size();
  return true;
}

/**
    create a zip file from a list of files
    @param zipFile archive file name
    @param filesRootDir root dir of file list
    @param files list of files based on filesRootDir
    @param destRootDir root dir prefixed to any file of the files list in the archive (must have a trailing '/')
    @param memFiles list of in memory files
*/
bool Packager::createZipFile(const QString &zipFile, const QString &filesRootDir,
                             const QList<InstallFile> &files, const QList<MemFile> &memFiles,
                             const QString &destRootDir , FileSizeInfo &sizeInfo)
{
    QuaZip zip(zipFile + ".zip");
    if(!zip.open(QuaZip::mdCreate))
      ON_ERROR(zip.getZipError());

    QuaZipFile outFile(&zip);
    QFile inFile;
    qint64 iBytesRead;
    QByteArray ba;
    ba.resize(QZIP_BUFFER);
    sizeInfo.installedSize = 0;

    Q_FOREACH(const InstallFile &file, files)
    {
        // FIXME: how to add directories to zip archive?
        if(file.inputFile.endsWith('/'))
          continue;
        inFile.setFileName(file.bAbsInputPath ? file.inputFile : filesRootDir + '/' + file.inputFile);

        if(!inFile.open(QIODevice::ReadOnly))
          ON_ERROR_S(qPrintable(inFile.errorString()));

        QuaZipNewInfo a(destRootDir + (file.outputFile.isEmpty() ? file.inputFile : file.outputFile), inFile.fileName());
        if(!outFile.open(QIODevice::WriteOnly, a))
          ON_ERROR(outFile.getZipError());

        while((iBytesRead = inFile.read(ba.data(), QZIP_BUFFER)) > 0)
            outFile.write(ba.data(), iBytesRead);

        if(outFile.getZipError()!=UNZ_OK)
          ON_ERROR(outFile.getZipError());
        outFile.close();
        if(outFile.getZipError()!=UNZ_OK)
          ON_ERROR(outFile.getZipError());

        inFile.close();
        sizeInfo.installedSize += inFile.size();
    }

    Q_FOREACH(const MemFile &file, memFiles)
    {
        if(!outFile.open(QIODevice::WriteOnly, QuaZipNewInfo(file.filename)))
          ON_ERROR(outFile.getZipError());

        sizeInfo.installedSize += file.data.size();

        outFile.write(file.data, file.data.size());
        if(outFile.getZipError()!=UNZ_OK)
          ON_ERROR(outFile.getZipError());

        outFile.close();
        if(outFile.getZipError()!=UNZ_OK)
          ON_ERROR(outFile.getZipError());
    }

    zip.close();
    if(zip.getZipError()!=UNZ_OK)
      ON_ERROR(zip.getZipError());

    QFileInfo fi(zip.getZipName());
    sizeInfo.compressedSize = fi.size();

    return true;
}

bool Packager::generatePackageFileList(QList<InstallFile> &fileList, Packager::Type type, const QString &root)
{	
	QString dir = root.isEmpty() ? m_rootDir : root;
    QString exclude;
    fileList.clear();
    QString lName = m_name.toLower();
    // special packaging for Qt
    if (m_special)
        switch (type) {
            case BIN:
                // assistant.exe can be used separatly from qt doc - better in bin than doc package
                generateFileList(fileList, dir, "bin", "*.dll assistant.exe qdbus.exe qdbusviewer.exe", "*d.dll *d4.dll");
                generateFileList(fileList, dir, "plugins", "*.dll","*d.dll *d4.dll *d1.dll");
                generateFileList(fileList, dir, "translations", "*.qm");
                generateFileList(fileList, dir, "manifest", "*-bin.cmd" );
                generateFileList(fileList, dir, QString(), "*.xml", "");
                return true;
            case DEBUG:
                generateFileList(fileList, dir, "bin",  "*d4.dll", "");
                generateFileList(fileList, dir, "lib",  "d4*.pdb", "");
                generateFileList(fileList, dir, "plugins", "*d.dll *d4.dll *d1.dll");
                generateFileList(fileList, dir, "plugins", "*d.pdb *d4.pdb *d1.pdb");
                if (m_type == "mingw" || m_type == "mingw4")
                    generateFileList(fileList, dir, "lib",      "*d4.a *d.a");
                else
                    generateFileList(fileList, dir, "lib",      "*d4.lib *d.lib");
                return true;
            case LIB:
                generateFileList(fileList, dir, "bin",  "*.exe *.bat", "assistant.exe qtdemo.exe qdbus.exe qdbusviewer.exe");
                generateFileList(fileList, dir, "", ".qmake.cache");
                // trolltech installs whole mkspecs folder too
                generateFileList(fileList, dir, "mkspecs", "*.*", ".svn CVS");
                generateFileList(fileList, dir, "manifest", "*-lib.cmd" );
                // add phrase books
                generateFileList(fileList, dir, "tools/linguist/phrasebooks", "*.qph");
                generateFileList(fileList, dir, "phrasebooks", "*.qph");

                if (m_type == "mingw" || m_type == "mingw4")
                    generateFileList(fileList, dir, "lib",      "*.a", "*d4.a *d.a");
                else
                    generateFileList(fileList, dir, "lib",      "*.lib",  "*d4.lib *d.lib");
                parseQtIncludeFiles(fileList, dir, "include", "*", "private *_p.h *.pr* .svn CVS");
                return true;
            case DOC:
                generateFileList(fileList, dir, "bin", "qtdemo.exe");
                generateFileList(fileList, dir, "doc", "*.*", ".svn CVS");
                generateFileList(fileList, dir, "manifest", "*-doc.cmd" );               
                return true;
            case SRC:
                // TODO: not implemented, exclude temporay files and svn/cvs dirs
                //generateFileList(fileList, dir, "src", "*.*");
                exclude = m_srcExcludes + " .git .svn CVS .#* *.rej *.orig *.bak";
                // * and not *.* because *.* does not find "foo" (filename without extension) - Qt-bug?
                if (m_srcRoot.isEmpty())
                    generateFileList(fileList, dir, "src", "*",exclude);
                else
                    generateFileList(fileList, m_srcRoot, ".", "*",exclude);
                generateFileList(fileList, dir, "manifest", "*-src.cmd" );                   
                return true;
            case NONE:
                generateFileList(fileList, dir, ".", "*.*", "manifest .git .svn CVS");
                return true;
            default:
                break;
        }
    else
    {
        switch (type) {
            case BIN:
                generateFileList(fileList, dir, "bin",  "*.exe *.bat");
#if DEBUG_LIB_SUPPORT
                generateFileList(fileList, dir, "bin",  "*.exe *.bat", "*d.exe");
                generateFileList(fileList, dir, "bin",  "*.dll", "*d.dll");
                generateFileList(fileList, dir, "lib",  "*.dll", "*d.dll");
#else
                generateFileList(fileList, dir, "bin",  "*.dll");
                generateFileList(fileList, dir, "lib",  "*.dll");
                generateFileList(fileList, dir, "lib",  "*.exe *.bat");
#endif
                generateFileList(fileList, dir, "manifest", "*-bin.cmd" );

                generateFileList(fileList, dir, "share", "*.*");
                // lib may have additional files provided  by at least aspell-xx and automoc4 
                generateFileList(fileList, dir, "lib",      "*.*","*d.dll *.dll *.lib *.a");

                generateFileList(fileList, dir, "plugins", "*.dll");
                generateFileList(fileList, dir, "data", "*.*");
                generateFileList(fileList, dir, "etc",  "*.*");
                return true;
            case LIB:
#if DEBUG_LIB_SUPPORT
                generateFileList(fileList, dir, "bin",      "*d.dll *d.exe");
                generateFileList(fileList, dir, "lib",      "*d.dll");
#else
#endif
                generateFileList(fileList, dir, "manifest", "*-lib.cmd" );
                generateFileList(fileList, dir, "lib",      "*.lib");   // msvc libs (static & import libs)
                generateFileList(fileList, dir, "lib",      "*.a");          // gcc (static) libs
                generateFileList(fileList, dir, "include", "*.*");
                return true;
            case DOC:
                generateFileList(fileList, dir, "manifest", "*-doc.cmd" );
                generateFileList(fileList, dir, "doc", "*.*");
                generateFileList(fileList, dir, "man", "*.*");
                return true;
            case SRC:
                exclude = m_srcExcludes + " .git .svn CVS .#* *.rej *.orig *.bak";
                // * and not *.* because *.* does not find "foo" (filename without extension) - Qt-bug?
                if (m_srcRoot.isEmpty())
                    generateFileList(fileList, dir, "src", "*",exclude);
                else
                    generateFileList(fileList, m_srcRoot, ".", "*",exclude);
                generateFileList(fileList, dir, "manifest", "*-src.cmd" );                   
                return true;
            case NONE:
                generateFileList(fileList, dir, ".", "*.*", "manifest");
                return true;
            default:
                break;
        }
    }
    return false;
}

bool Packager::createManifestFiles(const QString &rootDir, QList<InstallFile> &fileList, Packager::Type type, QList<MemFile> &manifestFiles)
{
    QString fileNameBase = getBaseFileName(type);
    QString descr;
    MemFile mf;

    manifestFiles.clear();

    switch(type) {
        case Packager::BIN:
            descr = "binaries";
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
    out.setCodec(QTextCodec::codecForName("UTF-8"));
    Q_FOREACH(const InstallFile &file, fileList) {
        const QString &fn = file.inputFile;
        if(!fn.endsWith('/')) 
        {
            QFile f(file.bAbsInputPath ? fn : rootDir + '/' + fn);
            if(!f.open(QIODevice::ReadOnly)) 
            {
                qWarning("Can't open '%s' !", qPrintable(fn));
                continue;
            }
            if (m_checkSumMode.isEmpty())
                continue;
            QString fn = file.outputFile.isEmpty() ? file.inputFile : file.outputFile;
            fn.replace(' ', "\\ "); // escape ' '
            HashFile hf(m_checkSumMode == "sha1" ? HashFile::SHA1 : HashFile::MD5,fn,rootDir); 
            out << hf.toHashFileContent();
        }
    }
    // qt needs a specific config file
    // \todo: move qt.conf creation one level above tobe able to 
    // move createManifestFiles into a generic ManifestFile class usable 
    // by installer too 
    if ((m_name.startsWith("qt") || m_name.startsWith("q++") || m_name.startsWith("q.."))
            && type == Packager::BIN)
    {
        createQtConfig(fileList,manifestFiles);
        out << "bin/qt.conf\n";
    }

    out << "manifest/" + fileNameBase + ".mft" << '\n'
        << "manifest/" + fileNameBase + ".ver" << '\n';

    b.close();
    mf.filename = "manifest/" + fileNameBase + ".mft";
    manifestFiles += mf;

    mf.data.clear();

    b.setBuffer(&mf.data);
    b.open(QIODevice::WriteOnly);
    out.setDevice(&b);
	out << getBaseName(Packager::NONE,' ') + ' ' + descr << '\n'
        << m_notes + '\n';
    b.close();
    mf.filename = "manifest/" + fileNameBase + ".ver";
    manifestFiles += mf;

    return true;
}

bool Packager::createHashFile(const QString &packageFileName, const QString &basePath, HashValue &hash)
{
    HashFile hashFile(m_checkSumMode == "md5" ? HashFile::MD5 : HashFile::SHA1, packageFileName, basePath);
    if (!hashFile.computeHash())
        return false; 
    hash = HashValue(hashFile.type(), hashFile.getHash());
    hashFile.save();

    return true;
}

bool Packager::makePackagePart(const QString &root, QList<InstallFile> &fileList, QList<MemFile> &manifestFiles, Packager::Type type, QString destdir)
{
    QString _destdir = destdir;
    if (!destdir.isEmpty() && !destdir.endsWith("/") && !destdir.endsWith("\\"))
        _destdir += "/";

    PackagerInfo *info;
    if (!m_packagerInfo[type])
    {
        info = new PackagerInfo;
        m_packagerInfo[type] = info;
    }
    else 
        info = m_packagerInfo[type];
    info->version = m_version;
    info->name = m_name;
    info->compilerType = m_type;
    
    generatePackageFileList(fileList, type);
    if (fileList.size() > 0)
    {
        if (m_verbose)
        {
            qDebug() << "creating" << type << "of package" << destdir + getBaseFileName(type);
            Q_FOREACH(const InstallFile &f, fileList)
                if (!f.inputFile.endsWith("/"))
                    qDebug() << "\t" << f.inputFile << " -> " << f.outputFile;
        }
        createManifestFiles(root, fileList, type, manifestFiles);
        compressFiles(_destdir + getBaseFileName(type), root, fileList, manifestFiles, QString(), info->size);
        if (!m_checkSumMode.isEmpty())
            createHashFile(getBaseFileName(type) + getCompressedExtension(type), _destdir, info->hash);
        info->writeToFile(destdir + "/" + getBaseFileName(type) + ".xml");
        return true;
    }
    // \todo this message should not be printed when in xml mode
    if  (m_verbose)
        qDebug() << "no files found for part" << type << "of package" << m_name;
    return false;
}       

bool Packager::makePackage(const QString &root, const QString &destdir, bool bComplete)
{
    // generate file lists
    // create manifest files
    // create zip file
    m_rootDir = root;
    QList<InstallFile> fileList;
    QList<MemFile> manifestFiles;
        
    makePackagePart(root, fileList, manifestFiles, Packager::BIN, destdir);
    makePackagePart(root, fileList, manifestFiles, Packager::LIB, destdir);
    makePackagePart(root, fileList, manifestFiles, Packager::DOC, destdir);    
    makePackagePart(m_srcRoot.isEmpty() ? m_rootDir : m_srcRoot, fileList, manifestFiles, Packager::SRC, destdir);
    if(bComplete) {
        makePackagePart(root, fileList, manifestFiles, Packager::NONE, destdir);
    }

    return true;
}

QString Packager::getCompressedExtension(Packager::Type type)
{
    switch(m_compMode) {
        default:
        case 1:
            return ".zip";
        case 2:
            return ".tar.bz2";
    }
}
QString Packager::getBaseName(Packager::Type type, const QChar &versionDelimiter)
{
    QString nameType = m_type.isEmpty() ? m_name : m_name + '-' + m_type;
    return nameType + versionDelimiter + m_version;
}

QString Packager::getBaseFileName(Packager::Type type)
{
    QString name = getBaseName(type);

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
    QList<InstallFile> fileList;
#if DEBUG_LIB_SUPPORT
    generateFileList(fileList,dir,"bin","*.exe *.dll","*d.exe *d.dll *d4.dll");
    generateFileList(fileList,dir,"lib","*.exe *.dll","*d.exe *d.dll *d4.dll");
#else
    generateFileList(fileList,dir,"bin","*.exe *.dll","");
    generateFileList(fileList,dir,"lib","*.exe *.dll","");
#endif
    for (int i = 0; i < fileList.size(); i++)
    {
        QFileInfo fi(dir + "/" + fileList.at(i).inputFile);
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
    QList<InstallFile> fileList;
    generateFileList(fileList,dir,"bin","*.exe *.dll");
    generateFileList(fileList,dir,"lib","*.exe *.dll");
    for (int i = 0; i < fileList.size(); i++)
    {
        QFileInfo fi(dir + '/' + fileList.at(i).inputFile);
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

bool Packager::createQtConfig(QList<InstallFile> &fileList, QList<MemFile> &manifestFiles)
{
    MemFile mf;
    QBuffer b(&mf.data);
    b.open(QIODevice::WriteOnly);
    QTextStream out(&b);
    out << "[Paths]\n";
    out << "Prefix=\n";
    out << "Documentation=../doc\n";
    out << "Headers=../include\n";
    out << "Libraries=../lib\n";
    out << "Binaries=\n";
    out << "Plugins=../plugins\n";
    out << "Data=..\n";
    out << "Translations=../translations\n";
    out << "Settings=../etc\n";
    out << "Examples=../examples\n";
    out << "Demos=../demos\n";

    b.close();
    mf.filename = "bin/qt.conf";
    manifestFiles += mf;
    return true;
}
