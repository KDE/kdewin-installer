/****************************************************************************
**
** Copyright (C) 2005 Ralf Habacker <ralf.habacker@freenet.de>
** Copyright (C) 2007-2008 Christian Ehrlicher <ch.ehrlicher@gmx.de>
** All rights reserved.
**
** This file is part of the KDE installer for windows
**
** SPDX-License-Identifier: GPL-2.0-only
****************************************************************************/

#include "config.h"
#include "debug.h"
#include "downloader.h"
#include "downloaderprogress.h"
#include "settings.h"

#ifdef Q_OS_WIN
#include <winsock2.h>
#include <ws2tcpip.h>
#endif

#include <qnamespace.h>
#include <QBuffer>
#include <QByteArray>
#include <QCryptographicHash>
#include <QEventLoop>
#include <QFile>
#include <QNetworkAccessManager>
#include <QNetworkProxyFactory>
#include <QNetworkReply>
#include <QPointer>
#include <QSslConfiguration>
#include <QTemporaryFile>

class DownloaderSingleton
{
public:
    DownloaderSingleton() {};

    Downloader downloader;
};
Q_GLOBAL_STATIC ( DownloaderSingleton, sDownloader );

class Downloader::Private
{
public:
    Private ()
      : cancel (false),
        progress ( 0 ),
        ioDevice ( NULL ),
        manager(new QNetworkAccessManager)
    {
    }

    ~Private()
    {
        delete manager;
    }
    bool cancel;
    DownloaderProgress *progress;
    QIODevice  *ioDevice;
    QString     fileName;         // holds filename in case target is a file
    Hash hash;
    QNetworkAccessManager *manager;
    QPointer<QNetworkReply> reply;
    int ret;
};

Downloader::Downloader ()
  : m_result ( Undefined ),
    m_loop(0),
    d ( new Private () )
{
    connect(d->manager, SIGNAL(finished(QNetworkReply*)), this, SLOT(slotReplyFinished(QNetworkReply*)));
    connect(d->manager, SIGNAL(sslErrors(QNetworkReply *, const QList<QSslError> &)),
                               this, SLOT(slotSslErrors(QNetworkReply *, const QList<QSslError> &)));
}

Downloader::~Downloader()
{
    delete d;
}

Downloader *Downloader::instance()
{
    return &sDownloader()->downloader;
}

void Downloader::setProgress ( DownloaderProgress *progress )
{
    d->progress = progress;
}

DownloaderProgress *Downloader::progress()
{
 return d->progress;
}

bool Downloader::fetch ( const QUrl &url, const QString &fileName )
{
    if ( url.isEmpty() )
        return false;

    QTemporaryFile *file = new QTemporaryFile ( fileName + ".part" );
    if ( !file->open () ) {
        setError ( tr ( "Unable to open file %1: %2." ).arg ( fileName ).arg ( file->errorString() ) );
        delete file;
        d->fileName = QString();
        return false;
    }
    d->ioDevice = file;
    d->fileName = fileName;

    qDebug() << "Downloading" << url.toString() << " to " << file->fileName();
    return fetchInternal ( url );
}

bool Downloader::fetch ( const QUrl &url, QByteArray &ba )
{
    d->fileName = QString();

    if ( url.isEmpty() )
        return true;

    ba.clear();
    d->ioDevice = new QBuffer ( &ba );
    if ( !d->ioDevice->open ( QIODevice::WriteOnly ) ) {
        setError ( tr ( "Internal error!" ) );
        delete d->ioDevice;
        d->ioDevice = 0;
        return false;
    }

    qDebug() << "Downloading" << url.toString() << " to memory";
    return fetchInternal ( url );
}

bool Downloader::fetchInternal ( const QUrl &url )
{
    qDebug() << this << __FUNCTION__ << "url: " << url.toString();

    m_usedURL = url;
    m_result = Undefined;
    m_resultString = QString();
    d->hash.reset();
    d->ret = 0;
    startRequest(url);

    if ( d->progress )
        d->progress->show();

    if ( !m_loop )
        m_loop = new QEventLoop ( this );
    do {
        m_loop->processEvents ( QEventLoop::WaitForMoreEvents );
    } while ( !d->reply->isFinished() || m_result == Undefined );

    d->ret = d->reply->error() == QNetworkReply::NoError ? 0 : 1;
    d->reply->deleteLater();

    if ( d->progress )
        d->progress->hide();

    return ( d->ret == 0 );
}

void Downloader::slotSslErrors(QNetworkReply *reply, const QList<QSslError> & errors)
{
    qDebug() << reply->url();
    foreach(const QSslError error, errors)
        qDebug() << reply->url() << error;
    d->reply->ignoreSslErrors();
}

void Downloader::slotReadyRead()
{
    QByteArray data = d->reply->readAll();
    QVariant value = d->reply->attribute(QNetworkRequest::RedirectionTargetAttribute);
    if (value.isNull())
    {
        d->ioDevice->write(data.constData(), data.size());
        d->hash.addData(data.constData(), data.size());
    }
}

void Downloader::slotReplyFinished(QNetworkReply *reply)
{
    qDebug() << this << __FUNCTION__;
    bool bRet = reply->error() == QNetworkReply::NoError;
    m_result = d->cancel ? Aborted : reply->error() == QNetworkReply::NoError ? Finished : Failed;
    QVariant value = d->reply->attribute(QNetworkRequest::RedirectionTargetAttribute);
    if (!value.isNull())
    {
        qDebug() << "redirected to" << value.toUrl();
        d->ioDevice->reset();
        d->ioDevice->seek(0);
        d->reply->deleteLater();
        m_usedURL = value.toUrl();
        startRequest(value.toUrl());
        return;
    }

    d->reply->deleteLater();
    QString fn;
    if ( bRet && !d->fileName.isEmpty() ) {
        QTemporaryFile *tf = static_cast<QTemporaryFile*> ( d->ioDevice );
        fn = tf->fileName();
        tf->setAutoRemove( false );
        tf->close();
        if ( QFile::exists ( d->fileName ) ) {
            if ( !QFile::remove ( d->fileName ) ) {
                setError ( tr ( "Error removing old %1" ).arg ( d->fileName ) );
                bRet = false;
            }
        }
    } else {
        d->ioDevice->close();
    }
    delete d->ioDevice;

    // temporary files must be deleted before they can be renamed
    if ( bRet && !d->fileName.isEmpty() && !QFile::rename ( fn, d->fileName ) ) {
        setError ( tr ( "Error renaming %1 to %2" ).arg ( fn ).arg ( d->fileName ) );
        bRet = false;
    }

    d->ioDevice = 0;
    if (reply->error() != QNetworkReply::NoError) {
        m_resultString = reply->errorString();
        d->ret = true;
    }

    if ( d->ret )
        setError ( m_resultString );
    emit done ( bRet );
    m_loop->quit();
}

int Downloader::slotProgressCallback ( qint64 dlnow, qint64 dltotal)
{
    if ( d->cancel )
        return 1;
    if ( d->progress ) {
        d->progress->setMaximum ( ( int ) dltotal );
        d->progress->setValue ( ( int ) dlnow );
    }
    return d->cancel;
}

void Downloader::cancel()
{
    d->cancel = true;
    if (d->reply)
        d->reply->abort();
}

bool Downloader::startRequest(const QUrl &url)
{
    Settings &s = Settings::instance();
    ProxySettings ps;
    ps.from(s.proxyMode(), url.scheme());

    if (ps.mode != ProxySettings::None)
    {
        QNetworkProxy proxy;
        proxy.setType(QNetworkProxy::HttpProxy);
        proxy.setHostName(ps.hostname);
        proxy.setPort(ps.port);
        proxy.setUser(ps.user);
        proxy.setPassword(ps.password);
        QNetworkProxy::setApplicationProxy(proxy);
    }
    else
        QNetworkProxy::setApplicationProxy(QNetworkProxy(QNetworkProxy::NoProxy));

    if (d->progress)
    {
        d->progress->setValue(0);
        d->progress->setTitle(url);
    }

    QNetworkRequest request(url);
    request.setSslConfiguration(QSslConfiguration::defaultConfiguration());
    d->reply = d->manager->get(request);
    connect(d->reply, SIGNAL(readyRead()), this, SLOT(slotReadyRead()));
    connect(d->reply, SIGNAL(downloadProgress(qint64,qint64)), this, SLOT(slotProgressCallback(qint64, qint64)));
    return d->reply->error() == QNetworkReply::NoError;
}

void Downloader::setError ( const QString &errStr )
{
    qCritical() << qPrintable ( errStr );
    emit error ( errStr );
}

void Downloader::setCheckSumType(Hash::Type type) 
{
    d->hash.setType(type);
}

QByteArray Downloader::checkSum() const
{
    return d->hash.result();
}

QDebug &operator<< ( QDebug &debug, const Downloader & )
{
    return debug;
}
