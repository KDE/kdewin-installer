/****************************************************************************
**
** Copyright (C) 2007 Christian Ehrlicher <ch.ehrlicher@gmx.de>
** All rights reserved.
**
** This file is part of the KDE installer for windows
**
** SPDX-License-Identifier: LGPL-2.0-only
**
****************************************************************************/

#include "debug.h"
#include "hash.h"
#include "installerprogress.h"
#include "misc.h"
#include "uninstaller.h"
#include "uninstaller_p.h"

#include <QDir>
#include <QFile>
#include <QFileInfo>
#include <QList>
#include <QEventLoop>

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

        if ( !QFile::exists ( fileItem.fileName) ) {
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
            QByteArray hash = Hash::instance().hash( f );
            f.close();

            if ( fileItem.hash != hash ) {
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
            fileName = l.left ( idx ).trimmed();
            if ( !Hash::isHash ( hash ) ) {
                if ( !Hash::isHash ( fileName ) ) {
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
        fileList += FileItem ( QDir::toNativeSeparators ( fi.absoluteFilePath() ), hash );
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
  : m_progress(0),
    m_thread ( NULL ),
    m_bRet ( false ),
    m_loop ( NULL )
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
    qCritical() << qPrintable ( errStr );
    emit error ( errStr );
}

void Uninstaller::setWarning ( const QString &warnStr )
{
    qWarning() << qPrintable ( warnStr );
    emit warning ( warnStr );
}
