/****************************************************************************
**
** Copyright (C) 2008 Christian Ehrlicher <ch.ehrlicher@gmx.de>
** All rights reserved.
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

#include "unpacker.h"
#include "unpacker_p.h"

#include <QtCore/QDir>
#include <QtCore/QEventLoop>
#include <QtCore/QFile>
#include <QtCore/QFileInfo>
#include <QtCore/QProcess>

#include "config.h"
#include "installerprogress.h"
#include "quazip.h"
#include "quazipfile.h"
#ifdef BZIP2_UNPACK_SUPPORT
  #include "bzip2iodevice.h"
  #include "tarfilter.h"
#endif
#ifdef SEVENZIP_UNPACK_SUPPORT
  #include "qua7zip.h"
  using namespace qua7zip;
#endif
#ifndef QUNZIP_BUFFER
# define QUNZIP_BUFFER (256 * 1024)
#endif

UPThread::UPThread ( QObject *parent )
        : QThread ( parent ), m_bCancel( false )
{}

UPThread::~UPThread()
{}

void UPThread::run()
{
    bool bOk = false;
    if ( m_filename.endsWith ( ".zip" ) ) {
        bOk = unzipFile();
    } else
    if ( m_filename.endsWith ( ".tar.bz2" ) || m_filename.endsWith ( ".tbz" ) ) {
        bOk = unbz2File();
    } else
    if ( m_filename.endsWith ( ".7z" ) ) {
        bOk = un7zipFile();
    } else
    if ( m_filename.endsWith ( ".exe" ) || m_filename.endsWith ( ".msi" ) ) {
        bOk = unpackExe();
    } else {
        emit error ( tr ( "Don't know what to do with %1" ).arg ( m_filename ) );
    }
    emit done( bOk );
}

void UPThread::unpackFile ( const QString &fn, const QString &destdir, const StringHash &pathRelocations )
{
    m_bCancel = false;
    m_filename = fn;
    m_destdir = destdir;
    m_pathRelocations = pathRelocations;
    m_unpackedFiles.clear();
    start();
}

void UPThread::cancel()
{
  m_bCancel = true;
}

QStringList UPThread::getUnpackedFiles() const
{
  return m_unpackedFiles;
}

void UPThread::start ( Priority priority )
{
    QThread::start ( priority );
}

bool UPThread::unzipFile()
{
    QuaZip z ( m_filename );

    if ( !z.open ( QuaZip::mdUnzip ) ) {
        emit error ( tr ( "Can not open %1" ).arg ( m_filename ) );
        return false;
    }

    QuaZipFile file ( &z );
    QuaZipFileInfo info;

    QString name;
    QFileInfo fi;
    QFile newFile;
    QByteArray ba;
    ba.resize ( QUNZIP_BUFFER );
    for ( bool bOk = z.goToFirstFile(); bOk; bOk = z.goToNextFile() ) {
        if( m_bCancel )
            return false;
        // get file information
        if ( !z.getCurrentFileInfo ( &info ) ) {
            emit error ( tr ( "Can not get file information from zip file %1" ).arg ( m_filename ) );
            return false;
        }
        // relocate path names
        relocateFileName ( info.name, name );

        m_unpackedFiles << name;
        fi.setFile ( m_destdir.filePath ( name ) );

        bool isDir = info.compressedSize == 0 && info.uncompressedSize == 0;
        // is it's a subdir ?
        if ( !makeSurePathExists ( fi, isDir ) )
            return false;
        if ( isDir )
            continue;

        // open file
        if ( !file.open ( QIODevice::ReadOnly ) ) {
            emit error ( tr ( "Can not open file %1 from zip file %2" ).arg ( info.name ).arg ( m_filename ) );
            return false;
        }
        if ( file.getZipError() != UNZ_OK ) {
            emit error ( tr ( "Error reading zip file %1" ).arg ( m_filename ) );
            return false;
        }

        // create new file
        newFile.setFileName ( fi.absoluteFilePath() );
        if ( !newFile.open ( QIODevice::WriteOnly ) ) {
            emit error ( tr ( "Can not creating file %1" ).arg ( fi.absoluteFilePath() ) );
            return false;
        }

        emit progress ( tr ( "Installing %1" ).arg ( newFile.fileName() ) );

        // copy data
        // FIXME: check for not that huge filesize ?
        qint64 iBytesRead;
        while ( ( iBytesRead = file.read ( ba.data(), QUNZIP_BUFFER ) ) > 0 )
            newFile.write ( ba.data(), iBytesRead );

        file.close();
        newFile.close();

        if ( file.getZipError() != UNZ_OK ) {
            emit error ( tr ( "Error reading zip file %1" ).arg ( m_filename ) );
            return false;
        }
    }
    z.close();
    if ( z.getZipError() != UNZ_OK ) {
        emit error ( tr ( "Error reading zip file %1" ).arg ( m_filename ) );
        return false;
    }
    return true;
}

bool UPThread::unbz2File()
{
#ifdef BZIP2_UNPACK_SUPPORT
    QFile file ( m_filename );
    if ( !file.open ( QIODevice::ReadOnly ) ) {
        emit error ( tr ( "Error opening %1 for reading" ).arg ( m_filename ) );
        return false;
    }
    BZip2IODevice bzip2 ( &file );
    if ( !bzip2.open ( QIODevice::ReadOnly ) ) {
        emit error ( tr ( "Error opening %1 for reading (%2)" ).arg ( m_filename ).arg ( bzip2.errorString() ) );
        return false;
    }

    TarFilter tf ( &bzip2 );
    TarFilter::FileInformations tarFileInfo;

    QString name;
    QFileInfo fi;
    QFile newFile;
    while ( tf.getData ( tarFileInfo ) ) {
        if( m_bCancel )
            return false;
        // relocate path names
        relocateFileName ( tarFileInfo.fileName, name );

        m_unpackedFiles << name;
        fi.setFile ( m_destdir.filePath ( name ) );

        // is it's a subdir ?
        if ( !makeSurePathExists ( fi, tarFileInfo.fileType == TarFilter::directory ) )
            return false;
        if ( tarFileInfo.fileType == TarFilter::directory )
            continue;

        if ( tarFileInfo.fileType != TarFilter::regular && tarFileInfo.fileType != TarFilter::regular2 ) {
            emit error ( tr ( "Can not unpack %1 - unsupported filetype %1" ).arg ( tarFileInfo.fileName ).arg ( tarFileInfo.fileType ) );
            continue;
        }

        // create new file
        newFile.setFileName ( fi.absoluteFilePath() );
        if ( !newFile.open ( QIODevice::WriteOnly ) ) {
            emit error ( tr ( "Can not creating file %1" ).arg ( fi.absoluteFilePath() ) );
            return false;
        }

        emit progress ( newFile.fileName() );

        if ( !tf.getData ( &newFile ) ) {
            emit error ( tr ( "Can't write to file %1 (%2)" ).arg ( fi.absoluteFilePath() ).arg ( newFile.errorString() ) );
            return false;
        }
        newFile.close();
    }

    bzip2.close();
    return true;
#else   // BZIP2_UNPACK_SUPPORT
    emit error ( tr ( "BZip2 support not compiled in for %1" ).arg ( m_filename ) );
    return false;
#endif  // BZIP2_UNPACK_SUPPORT
}

bool UPThread::un7zipFile()
{
#ifdef SEVENZIP_UNPACK_SUPPORT
    Qua7zip z ( m_filename );

    if ( !z.open ( Qua7zip::mdUnpack ) ) {
        emit error ( tr ( "Can not open %1" ).arg ( m_filename ) );
        return false;
    }

    if ( !m_destdir.exists() ) {
        emit error ( tr ( "Internal Error - Path %1 does not exist" ).arg ( m_destdir.absolutePath() ) );
        return false;
    }

    Qua7zipFile file ( &z );
    Qua7zipFileInfo info;
    QFileInfo fi;
    QString name;
    QByteArray ba;
    ba.resize ( QUNZIP_BUFFER );

    for ( bool bOk = z.goToFirstFile(); bOk; bOk = z.goToNextFile() ) {
        if( m_bCancel )
            return false;
        // get file information
        if ( !z.getCurrentFileInfo ( info ) ) {
            emit error ( tr ( "Can not get file information from zip file %1" ).arg ( m_filename ) );
            return false;
        }
        // relocate path names
        relocateFileName ( info.fileName, name );

        m_unpackedFiles << name;
        fi.setFile ( m_destdir.filePath ( name ) );

        // is it's a subdir ?
        if ( !makeSurePathExists ( fi, info.isDir ) )
            return false;
        if ( info.isDir )
            return false;

        // open file
        if ( !file.open ( QIODevice::ReadOnly ) ) {
            emit error ( tr ( "Can not open file %1 from zip file %2" ).arg ( info.fileName ).arg ( m_filename ) );
            return false;
        }
        if ( file.get7zipError() != SZ_OK ) {
            emit error ( tr ( "Error reading zip file %1" ).arg ( m_filename ) );
            return false;
        }

        // create new file
        QFile newFile ( fi.absoluteFilePath() );
        if ( !newFile.open ( QIODevice::WriteOnly ) ) {
            emit error ( tr ( "Can not creating file %1" ).arg ( fi.absoluteFilePath() ) );
            return false;
        }

        emit progress ( tr ( "Installing %1" ).arg ( newFile.fileName() ) );

        // copy data
        // FIXME: check for not that huge filesize ?
        qint64 iBytesRead;
        while ( ( iBytesRead = file.read ( ba.data(), QUNZIP_BUFFER ) ) > 0 )
            newFile.write ( ba.data(), iBytesRead );

        file.close();
        newFile.close();

        if ( file.get7zipError() != SZ_OK ) {
            emit error ( tr ( "Error reading zip file %1" ).arg ( m_filename ) );
            return false;
        }
    }
    z.close();
    if ( z.get7zipError() != SZ_OK ) {
        emit error ( tr ( "Error reading zip file %1" ).arg ( m_filename ) );
        return false;
    }
    return true;
#else   // SEVENZIP_UNPACK_SUPPORT
    emit error ( tr ( "7-zip support not compiled in for %1" ).arg ( m_filename ) );
    return false;
#endif  // SEVENZIP_UNPACK_SUPPORT
}

bool UPThread::unpackExe()
{
#ifdef Q_OS_WIN
    QProcess proc;
    proc.start ( m_filename, QStringList ( "/Q" ) );   // FIXME: don't hardcode command line parameters!
    if ( !proc.waitForStarted() )
        return false;
    do {
        msleep ( 50 );
    } while ( !proc.waitForFinished() );

    return ( proc.exitStatus() == QProcess::NormalExit && proc.exitCode() == 0 );
#else
    emit error ( tr ( "Don't know how to execute %1 on a non windows system." ).arg ( m_filename ) );
    return false;
#endif
}

void UPThread::relocateFileName ( const QString &in, QString &out )
{
    StringHash::ConstIterator it = m_pathRelocations.constBegin();
    StringHash::ConstIterator end = m_pathRelocations.constEnd();
    out = in;
    for ( ; it != end; ++it ) {
        out = out.replace ( QRegExp ( it.key() ), it.value() );
        qDebug() << __FUNCTION__ << in << "relocated to" << out;
    }
}

bool UPThread::makeSurePathExists ( const QFileInfo &fi, bool bIsDir )
{
    if ( bIsDir ) {
        if ( fi.exists() ) {
            if ( !fi.isDir() ) {
                emit error ( tr ( "Can not create directory %1" ).arg ( fi.absoluteFilePath() ) );
                return false;
            }
        }
        if ( !m_destdir.mkpath ( fi.absoluteFilePath() ) ) {
            emit error ( tr ( "Can not create directory %1" ).arg ( fi.absolutePath() ) );
            return false;
        }
    }
    // some archives does not have directory entries
    else {
        if ( !m_destdir.exists ( fi.absolutePath() ) ) {
            if ( !m_destdir.mkpath ( fi.absolutePath() ) ) {
                emit error ( tr ( "Can not create directory %1" ).arg ( fi.absolutePath() ) );
                return false;
            }
        }
    }
    return true;
}

/*
    Unpacker
 */
Unpacker::Unpacker ( InstallerProgress *progress, QObject *parent )
        : QObject ( parent ), m_progress ( progress ), m_thread ( NULL ), m_bFinished ( true ), m_bRet ( false )
{
}

Unpacker::~Unpacker()
{
}

bool Unpacker::unpackFile ( const QString &fn, const QString &destpath, const StringHash &pathRelocations )
{
    m_bFinished = true;
    m_bRet = false;

    QDir path ( destpath );
    if ( !path.exists() ) {
        setError ( tr ( "Internal Error - Path %1 does not exist" ).arg ( path.absolutePath() ) );
        return false;
    }
    if ( !QFile::exists ( fn ) ) {
        setError ( tr ( "Internal Error - File %1 does not exist" ).arg ( fn ) );
        return false;
    }

    m_bFinished = false;
    if ( !m_thread ) {
        m_thread = new UPThread ( this );
        connect ( m_thread, SIGNAL ( done ( bool ) ), this, SLOT ( threadFinished ( bool ) ) );
        connect ( m_thread, SIGNAL ( progress ( QString ) ), this, SLOT ( progressCallback ( QString ) ) );
        connect ( m_thread, SIGNAL ( error ( QString ) ), this, SLOT ( setError ( QString ) ) );
    }
    if ( m_progress ) {
        m_progress->setValue ( 0 );
        m_progress->show();
        m_progress->setTitle ( tr ( "Unpacking %1" ).arg ( fn ) );
    }
    m_thread->unpackFile ( fn, destpath, pathRelocations );
    QEventLoop *loop;
    do {
        loop = new QEventLoop ( this );
        loop->processEvents ( QEventLoop::AllEvents, 50 );
    } while ( !m_bFinished );
    delete loop;
    if ( m_progress )
        m_progress->hide();

    return m_bRet;
}

void Unpacker::cancel()
{
  if( m_thread )
    m_thread->cancel();
}

QStringList Unpacker::getUnpackedFiles() const
{
  return m_thread ? m_thread->getUnpackedFiles() : QStringList();
}

void Unpacker::threadFinished ( bool bOk )
{
    m_bFinished = true;
    m_bRet = bOk;
    emit done ( bOk );
}

void Unpacker::progressCallback ( const QString &file )
{
    if ( m_progress )
        m_progress->setTitle ( tr ( "Installing %1" ).arg ( file ) );
}

void Unpacker::setError ( const QString &errStr )
{
    qWarning ( qPrintable ( errStr ) );
    emit error ( errStr );
}

#include "unpacker.moc"
#include "unpacker_p.moc"
