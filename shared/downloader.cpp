/****************************************************************************
**
** Copyright (C) 2005 Ralf Habacker <ralf.habacker@freenet.de>
** Copyright (C) 2007-2008 Christian Ehrlicher <ch.ehrlicher@gmx.de>
** All rights reserved.
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

#include "config.h"
#include "debug.h"
#include "downloader.h"
#include "downloader_p.h"
#include "downloaderprogress.h"
#include "settings.h"

#ifdef Q_OS_WIN
#include <winsock2.h>
#include <ws2tcpip.h>
#endif
#include <curl/curl.h>

#include <qnamespace.h>
#include <QBuffer>
#include <QByteArray>
#include <QEventLoop>
#include <QFile>
#include <QTemporaryFile>
#include <QThread>
#include <QCryptographicHash>

MyThread::MyThread ( CURL *handle, QObject *parent )
        : QThread ( parent ), curlHandle ( handle ), m_bCancel ( false ), m_ret ( CURLE_OK )
{
    curl_easy_setopt ( curlHandle, CURLOPT_PROGRESSFUNCTION, MyThread::curlProgressCallback );
    curl_easy_setopt ( curlHandle, CURLOPT_PROGRESSDATA, this );
}

void MyThread::run()
{
    m_bCancel = false;
    m_ret = curl_easy_perform ( curlHandle );
}

void MyThread::cancel()
{
    m_bCancel = true;
}

CURLcode MyThread::retCode() const
{
    return m_ret;
}

int MyThread::progressCallback ( double dltotal, double dlnow )
{
    if ( m_bCancel )
        return 1;
    emit progress ( dltotal, dlnow );
    return 0;
}

int MyThread::curlProgressCallback ( void *clientp, double dltotal, double dlnow,
                                     double ultotal, double ulnow )
{
    MyThread *that = static_cast<MyThread*> ( clientp );
    return that->progressCallback ( dltotal, dlnow );
}

class DownloaderSingleton
{
public:
    DownloaderSingleton() {};

    Downloader downloader;
};
Q_GLOBAL_STATIC ( DownloaderSingleton, sDownloader );

// needed to hide curl handle from the rest - including curl/curl.h isn't very
// optimal
class Downloader::Private
{
public:
    Private ()
            : curlHandle ( 0 ), thread ( NULL ), cancel ( false ), ret ( CURLE_OK ),
            progress ( 0 ), ioDevice ( NULL ) {}

    ~Private() {
        if ( thread )
            thread->terminate();
        curl_easy_cleanup ( curlHandle );
        delete thread;
    }
    CURL     *curlHandle;
    MyThread *thread;
    bool      cancel;
    CURLcode  ret;
    DownloaderProgress *progress;
    QIODevice  *ioDevice;
    QString     fileName;         // holds filename in case target is a file
    QByteArray  encodedUrl;       // the encoded url for some (old?) curl versions
                                  // which don't copy the url to an own buffer
    Hash hash;
};

Downloader::Downloader ()
        : m_result ( Undefined ), m_loop ( 0 ), d ( new Private () )
{
    curl_global_init ( CURL_GLOBAL_ALL );
}

Downloader::~Downloader()
{
    delete m_loop;
    delete d;
    curl_global_cleanup();
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
    if ( d->progress )
        d->progress->setTitle ( url, fileName );

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
    if ( d->progress )
        d->progress->setTitle ( url );

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

#ifdef _DEBUG
static int my_curl_debug_callback (CURL *, curl_infotype type, char *data, size_t size, void *)
{
  QByteArray ba(data, size);
  ba.replace("\r","");
  ba.replace("\n","");
  switch( type ) {
    case CURLINFO_TEXT:
      qDebug() << "Text: " << ba.data();
      break;
    case CURLINFO_HEADER_IN:
      qDebug() << "HeaderIn: " << ba.data();
      break;
    case CURLINFO_HEADER_OUT:
      qDebug() << "HeaderOut: " << ba.data();
      break;
    case CURLINFO_DATA_IN:
      /// this print complete package content 
      //qDebug() << "DataIn: " << ba.data();
      break;
    case CURLINFO_DATA_OUT:
      qDebug() << "DataOut: " << ba.data();
      break;
    default:
      qDebug() << "Unknown: " << ba.data();
  }
  return 0;
}
#endif

bool Downloader::fetchInternal ( const QUrl &url )
{
    qDebug() << this << __FUNCTION__ << "url: " << url.toString();

    m_usedURL = url;
    m_result = Undefined;
    d->hash.reset();
    d->cancel = false;

    if ( !d->curlHandle ) {
        d->curlHandle = curl_easy_init();
        if ( d->curlHandle == NULL )
            setError ( tr ( "Error initializing curl" ) );
        curl_easy_setopt ( d->curlHandle, CURLOPT_WRITEFUNCTION, Downloader::curlWriteCallback );
        curl_easy_setopt ( d->curlHandle, CURLOPT_WRITEDATA, this );
        curl_easy_setopt ( d->curlHandle, CURLOPT_NOPROGRESS, 0 );
        curl_easy_setopt ( d->curlHandle, CURLOPT_FOLLOWLOCATION, 1 );
        curl_easy_setopt ( d->curlHandle, CURLOPT_USERAGENT, "KDEWin-Installer/" VERSION );
        /// @TODO: activate this to let downloader know about download errors 
        curl_easy_setopt ( d->curlHandle, CURLOPT_FAILONERROR, 1 );
#ifdef _DEBUG
        curl_easy_setopt( d->curlHandle, CURLOPT_VERBOSE, 1 );
        curl_easy_setopt( d->curlHandle, CURLOPT_DEBUGFUNCTION, my_curl_debug_callback );
#endif
    }

    Settings &s = Settings::instance();
    ProxySettings ps;
    ps.from(s.proxyMode(),m_usedURL.scheme());

    d->encodedUrl = m_usedURL.toEncoded();
    curl_easy_setopt ( d->curlHandle, CURLOPT_URL, d->encodedUrl.constData() );
    // curl reads from environment when nothing is set
    if (s.proxyMode() != ProxySettings::Environment ) {
        curl_easy_setopt ( d->curlHandle, CURLOPT_PROXY, ps.hostname.toLocal8Bit().constData() );
        curl_easy_setopt ( d->curlHandle, CURLOPT_PROXYPORT, ps.port );
        QString user =  ps.user.isEmpty() ? QString() : ps.user + ":" +  ps.password;
        curl_easy_setopt ( d->curlHandle, CURLOPT_PROXYUSERPWD, user.toLocal8Bit().constData() );
        //curl_easy_setopt ( d->curlHandle, CURLOPT_PROXYTYPE, CURLPROXY_SOCKS4 );
    }
    if ( m_usedURL.port() != -1 ) {
        curl_easy_setopt ( d->curlHandle, CURLOPT_FTPPORT, m_usedURL.port() );
    }
    if ( !d->thread ) {
        d->thread = new MyThread ( d->curlHandle, this );
        connect ( d->thread, SIGNAL ( finished () ), this, SLOT ( threadFinished () ) );
        connect ( d->thread, SIGNAL ( progress ( double, double ) ), this, SLOT ( progressCallback ( double, double ) ) );
    }
    if ( d->progress ) 
    {    
        d->progress->setValue ( 0 );
        d->progress->show();
        d->progress->setTitle ( m_usedURL );
    }
    d->thread->start();
    if ( !m_loop )
        m_loop = new QEventLoop ( this );
    do {
        m_loop->processEvents ( QEventLoop::WaitForMoreEvents );
    } while ( !d->thread->isFinished() || m_result == Undefined );

    if ( d->progress )
        d->progress->hide();
    qDebug() << __FUNCTION__ << "ret: " << ( d->ret == 0 );

	char *p =0;
    if (curl_easy_getinfo ( d->curlHandle, CURLINFO_EFFECTIVE_URL, &p) == CURLE_OK) 
    {
        m_usedURL = p;
    }
    return ( d->ret == 0 );
}

void Downloader::threadFinished ()
{
    qDebug() << this << __FUNCTION__;
    d->ret = d->thread->retCode();
    bool bRet = ( d->ret == CURLE_OK );
    m_result = d->cancel ? Aborted : bRet ? Finished : Failed;

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
    m_resultString = QString ( curl_easy_strerror ( d->ret ) );
    if ( d->ret )
        setError ( m_resultString );
    emit done ( bRet );
    m_loop->quit();
}

size_t Downloader::curlWrite ( const char * data, size_t size )
{
    d->hash.addData( data, size );
    return d->ioDevice->write ( data, size );
}

size_t Downloader::curlWriteCallback ( void *data, size_t size, size_t nmemb, void *stream )
{
    Downloader *that = reinterpret_cast<Downloader*> ( stream );
    return that->curlWrite ( reinterpret_cast<const char*> ( data ), size * nmemb );
}

int Downloader::progressCallback ( double dltotal, double dlnow )
{
    if ( d->cancel )
        return 1;
    if ( d->progress ) {
        d->progress->setMaximum ( ( int ) dltotal );
        d->progress->setValue ( ( int ) dlnow );
    }
    return d->cancel;
}

int Downloader::curlProgressCallback ( void *clientp, double dltotal, double dlnow,
                                       double ultotal, double ulnow )
{
    Downloader *that = static_cast<Downloader*> ( clientp );
    return that->progressCallback ( dltotal, dlnow );
}

void Downloader::cancel()
{
    d->cancel = true;
    if ( d->thread )
        d->thread->cancel();
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
