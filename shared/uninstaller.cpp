/****************************************************************************
**
** Copyright (C) 2007 Christian Ehrlicher <ch.ehrlicher@gmx.de>
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

#include "installerprogress.h"
#include "md5.h"
#include "misc.h"
#include "uninstaller.h"
#include "uninstaller_p.h"

#include <QtCore/QDebug>
#include <QtCore/QDir>
#include <QtCore/QFile>
#include <QtCore/QFileInfo>
#include <QtCore/QList>
#include <QtCore/QEventLoop>
#include <qplatformdefs.h>

UIThread::UIThread ( QObject *parent )
        : QThread ( parent ), m_bCancel ( false ), m_bRet ( false )
{}

UIThread::~UIThread()
{}

void UIThread::uninstallPackage ( const QString &manifest, const QString &root )
{
    m_bRet = false;
    m_bCancel = false;
    m_manifest = manifest;
    m_root = root;
    start();
}

void UIThread::cancel()
{
    m_bCancel = true;
}


bool UIThread::retCode() const
{
    return m_bRet;
}

void UIThread::run()
{
    QList<FileItem> files;
    QT_STATBUF statBuf;
    QFile f;

    if ( !readManifestFile ( files ) ) {
        m_bRet = false;
        return;
    }

    Q_FOREACH ( const FileItem &fileItem, files ) {
        if ( m_bCancel ) {
            m_bRet = false;
            return;
        }

        if ( QT_STAT ( fileItem.fileName.toLocal8Bit(), &statBuf ) == -1 ) {
            emit warning ( QString ( "Can't remove %1 - not found" ).arg ( fileItem.fileName ) );
            continue;
        }

        if ( false && !fileItem.hash.isEmpty() ) {
            // read file & check hash
            f.setFileName ( fileItem.fileName );
            if ( !f.open ( QIODevice::ReadOnly ) ) {
                emit warning ( QString ( "Can't open %1 - not removing this file!" ).arg ( fileItem.fileName ) );
                continue;
            }
            QByteArray ba = f.readAll();
            f.close();

            if ( QString::fromLatin1 ( fileItem.hash ) != qtMD5 ( ba ) ) {
                emit warning ( QString ( "Not removing %1 because hash does not match (locally modified)!" ).arg ( fileItem.fileName ) );
                continue;
            }
        }

        emit progress ( fileItem.fileName );

        if ( !deleteFile ( m_root, fileItem.fileName ) ) {
            emit warning ( QString ( "Can't remove %1" ).arg ( fileItem.fileName ) );
            continue;
        }
    }
    m_bRet = true;
}

bool isHash ( const QByteArray &str )
{
    if ( str.length() != 32 )
        return false;
    for ( int i = 0; i < 32; i++ ) {
        const char c = str[i];
        if ( c < '0' || c > '9' ) {
            if ( ( c < 'a' || c > 'f' ) && ( c < 'A' || c > 'F' ) )
                return false;
        }
    }
    return true;
}

bool UIThread::readManifestFile ( QList<FileItem> &fileList )
{
    QFile f ( m_manifest );

    if ( !f.open ( QIODevice::ReadOnly|QIODevice::Text ) ) {
        emit error ( QString ( "Can't open %1 for reading!" ).arg ( m_manifest ) );
        return false;
    }
    QByteArray line ( 1024, 0 );
    QByteArray hash, fileName;
    QFileInfo fi;
    int iLength;

    while ( ( iLength = f.readLine ( line.data(), line.size()-1 ) ) != -1 ) {
        QByteArray l = line.left ( iLength );
        while ( l.endsWith ( '\n' ) || l.endsWith ( '\r' ) )
            l = l.left ( l.length()-1 );

        int idx = l.lastIndexOf ( ' ' );
        // not found, or it's an escaped ' '
        if ( idx == -1 || ( idx > 1 && l[idx-1] == '\\' ) ) {
            hash = QByteArray();
            fileName = l;
        } else {
            hash = l.mid ( idx+1 );
            fileName = l.left ( idx );
            if ( !isHash ( hash ) ) {
                if ( !isHash ( fileName ) ) {
                    emit warning ( QString ( "invalid entry in manifest file: '%1'" ).arg ( QString::fromUtf8 ( l ) ) );
                    continue;
                }
                QByteArray tmp = hash;
                hash = fileName;
                fileName = tmp;
            }
        }
        fileName = fileName.replace ( "\\ ", " " );
        fi.setFile ( m_root + '/' + QString::fromUtf8 ( fileName ) );
        if ( !fi.exists() ) {
            emit warning ( QString ( "File %1 does not exist!" ).arg ( fi.fileName() ) );
            continue;
        }
        fileList += FileItem ( QDir::convertSeparators ( fi.absoluteFilePath() ), hash );
    }
    f.close();

    return true;
}

void UIThread::start ( Priority priority )
{
    QThread::start ( priority );
}

/*
    Uninstaller
 */
class UninstallerSingleton
{
public:
    UninstallerSingleton() {};

    Uninstaller uninstaller;
};
Q_GLOBAL_STATIC ( UninstallerSingleton, sUninstaller );

Uninstaller::Uninstaller()
        : m_thread ( NULL ), m_bRet ( false ), m_loop ( NULL )
{}

Uninstaller::~Uninstaller()
{
    delete m_loop;
    delete m_thread;
}

Uninstaller *Uninstaller::instance()
{
    return &sUninstaller()->uninstaller;
}

void Uninstaller::setProgress ( InstallerProgress *progress )
{
    m_progress = progress;
}

// uninstall a package
bool Uninstaller::uninstallPackage ( const QString &pathToManifest, const QString &root )
{
    qDebug() << __FUNCTION__ << "path: " << pathToManifest << "root: " << root;
    m_bRet = false;
    m_bFinished = false;

    if ( !QFile::exists ( pathToManifest ) ) {
        setError ( tr ( "Manifest %1 not found - can't uninstall package!" ).arg ( pathToManifest ) );
        return false;
    }

    if ( !m_thread ) {
        m_thread = new UIThread ( this );
        connect ( m_thread, SIGNAL ( finished () ), this, SLOT ( threadFinished () ) );
        connect ( m_thread, SIGNAL ( progress ( QString ) ), this, SLOT ( progressCallback ( QString ) ) );
        connect ( m_thread, SIGNAL ( error ( QString ) ), this, SLOT ( setError ( QString ) ) );
        connect ( m_thread, SIGNAL ( warning ( QString ) ), this, SLOT ( setWarning ( QString ) ) );
    }
    if ( m_progress ) {
        m_progress->show();
        m_progress->setTitle ( tr ( "Uninstalling %1" ).arg ( QDir::toNativeSeparators ( pathToManifest ) ) );
    }
    m_thread->uninstallPackage ( pathToManifest, root );
    if ( !m_loop )
        m_loop = new QEventLoop ( this );
    do {
        m_loop->processEvents ( QEventLoop::WaitForMoreEvents );
    } while ( !m_thread->isFinished() || !m_bFinished );
    if ( m_progress )
        m_progress->hide();

    qDebug() << __FUNCTION__ << "ret: " << m_bRet;
    return m_bRet;
}

void Uninstaller::cancel()
{
    if ( m_thread )
        m_thread->cancel();
}

void Uninstaller::threadFinished ()
{
    m_bRet = m_thread->retCode();
    m_bFinished = true;
    emit done ( m_bRet );
    m_loop->quit();
}

void Uninstaller::progressCallback ( const QString &file )
{
    if ( m_progress )
        m_progress->setTitle ( tr ( "Removing %1" ).arg ( QDir::toNativeSeparators ( file ) ) );
}

void Uninstaller::setError ( const QString &errStr )
{
    qWarning ( qPrintable ( errStr ) );
    emit error ( errStr );
}

void Uninstaller::setWarning ( const QString &warnStr )
{
    qWarning ( qPrintable ( warnStr ) );
    emit warning ( warnStr );
}

#include "uninstaller.moc"
#include "uninstaller_p.moc"
