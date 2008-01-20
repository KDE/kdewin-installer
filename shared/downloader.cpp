/****************************************************************************
**
** Copyright (C) 2005 Ralf Habacker. All rights reserved.
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

#include <winsock2.h>
#include <ws2tcpip.h>
#include <curl/curl.h>
#include "downloader.h"
#include "downloader_p.h"

#include <QtCore/QBuffer>
#include <QtCore/QByteArray>
#include <QtCore/QDebug>
#include <QtCore/QEventLoop>
#include <QtCore/QFile>
#include <QtCore/QTemporaryFile>
#include <QtCore/QThread>
#include <QtGui/QApplication>
#include <QtNetwork/QNetworkProxy>

#include "downloaderprogress.h"
#include "settings.h"



MyThread::MyThread ( CURL *handle, QObject *parent )
    : QThread ( parent ), curlHandle ( handle ), bCancel ( false )
{
  curl_easy_setopt ( curlHandle, CURLOPT_PROGRESSFUNCTION, MyThread::curlProgressCallback );
  curl_easy_setopt ( curlHandle, CURLOPT_PROGRESSDATA, this );
}

void MyThread::run()
{
  bCancel = false;
  CURLcode ret = curl_easy_perform ( curlHandle );
  emit done ( ret );
}

void MyThread::cancel()
{
  bCancel = true;
}

int MyThread::progressCallback ( double dltotal, double dlnow )
{
  if ( bCancel )
    return 1;
  emit progress ( dltotal, dlnow );
  return bCancel;
}

int MyThread::curlProgressCallback ( void *clientp, double dltotal, double dlnow,
                                     double ultotal, double ulnow )
{
  MyThread *that = static_cast<MyThread*> ( clientp );
  return that->progressCallback ( dltotal, dlnow );
}

// needed to hide curl handle from the rest - including curl/curl.h isn't very
// optimal
class Downloader::Private
{
public:
  Private ( DownloaderProgress *_progress )
      : curlHandle ( 0 ), thread ( NULL ), cancel ( false ), finished ( false ), ret ( CURLE_OK ),
      progress ( _progress ), ioDevice ( NULL ) {}
  ~Private() {
    if ( thread )
      thread->terminate();
    curl_easy_cleanup ( curlHandle );
    delete thread;
  }
  CURL     *curlHandle;
  MyThread *thread;
  bool      cancel;
  bool      finished;
  CURLcode  ret;
  DownloaderProgress *progress;
  QIODevice  *ioDevice;
  QString     fileName;         /// holds filename in case target is a file
};

Downloader::Downloader ( DownloaderProgress *progress, QObject *parent )
    : QObject( parent), m_result ( Undefined ), d ( new Private ( progress ) )
{
  curl_global_init ( CURL_GLOBAL_ALL );
}

Downloader::~Downloader()
{
  curl_global_cleanup();
}

bool Downloader::start ( const QUrl &url, const QString &fileName )
{
  if ( url.isEmpty() )
    return false;
  if ( d->progress )
    d->progress->setTitle ( tr ( "Downloading %1 to %2" ).arg ( url.toString() ).arg ( fileName ) );

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
  return startInternal ( url );
}

bool Downloader::start ( const QUrl &url, QByteArray &ba )
{
  d->fileName = QString();

  if ( url.isEmpty() )
    return true;
  if ( d->progress )
    d->progress->setTitle ( tr ( "Downloading %1" ).arg ( url.toString() ) );

  d->ioDevice = new QBuffer ( &ba );
  if ( !d->ioDevice->open ( QIODevice::WriteOnly ) ) {
    setError ( tr ( "Internal error!" ) );
    delete d->ioDevice;
    d->ioDevice = 0;
    return false;
  }

  qDebug() << "Downloading" << url.toString() << " to memory";
  return startInternal ( url );
}

bool Downloader::startInternal ( const QUrl &url )
{
  m_usedURL = url;
  m_result = Undefined;
  d->cancel = false;

  if ( !d->curlHandle ) {
    d->curlHandle = curl_easy_init();
    if ( d->curlHandle == NULL )
      setError ( tr ( "Error initializing curl" ) );
    curl_easy_setopt ( d->curlHandle, CURLOPT_WRITEFUNCTION, Downloader::curlWriteCallback );
    curl_easy_setopt ( d->curlHandle, CURLOPT_WRITEDATA, this );
    curl_easy_setopt ( d->curlHandle, CURLOPT_NOPROGRESS, 0 );
  }

  Settings &s = Settings::getInstance();
  Settings::proxySettings ps;
  s.proxy ( url.scheme(), ps );

  curl_easy_setopt ( d->curlHandle, CURLOPT_URL, url.toEncoded().constData() );
  // curl reads from environment when nothing is set
  if ( !ps.hostname.isEmpty() && s.proxyMode() != Settings::Environment ) {
    curl_easy_setopt ( d->curlHandle, CURLOPT_PROXY, ps.hostname.toLocal8Bit().constData() );
    curl_easy_setopt ( d->curlHandle, CURLOPT_PROXYPORT, ps.port );
    QString user =  ps.user + ":" +  ps.password;
    curl_easy_setopt ( d->curlHandle, CURLOPT_PROXYUSERPWD, user.toLocal8Bit().constData() );
  }
  if ( url.port() != -1 ) {
    curl_easy_setopt ( d->curlHandle, CURLOPT_FTPPORT, url.port() );
  }
  if ( !d->thread ) {
    d->thread = new MyThread ( d->curlHandle, this );
    connect ( d->thread, SIGNAL ( done ( int ) ), this, SLOT ( threadFinished ( int ) ) );
    connect ( d->thread, SIGNAL ( progress ( double, double ) ), this, SLOT ( progressCallback ( double, double ) ) );
  }
  if ( d->progress ) {
    d->progress->setValue ( 0 );
    d->progress->show();
    d->progress->setTitle ( tr ( "Downloading %1" ).arg ( url.toString() ) );
  }
  d->finished = false;
  d->thread->start();
  QEventLoop *loop = new QEventLoop ( this );
  do {
    loop->processEvents ( QEventLoop::AllEvents, 50 );
  } while ( !d->finished );
  if ( d->progress )
    d->progress->hide();
  delete loop;
  return ( d->ret == 0 );
}

void Downloader::threadFinished ( int _ret )
{
  d->ret = static_cast<CURLcode> ( _ret );
  d->finished = true;
  bool bRet = ( d->ret == CURLE_OK );
  m_result = d->cancel ? Aborted : bRet ? Finished : Failed;

  if ( !d->fileName.isEmpty() ) {
    QTemporaryFile *tf = static_cast<QTemporaryFile*> ( d->ioDevice );
    QString fn = tf->fileName();
    tf->close();
    if ( QFile::exists ( d->fileName ) ) {
      if ( !QFile::remove ( d->fileName ) ) {
        setError ( tr ( "Error removing old %1" ).arg ( d->fileName ) );
        bRet = false;
      }
    }
    if ( !QFile::rename ( fn, d->fileName ) ) {
      setError ( tr ( "Error renaming %1 to %2" ).arg ( fn ).arg ( d->fileName ) );
      bRet = false;
    }
  } else {
    d->ioDevice->close();
  }
  delete d->ioDevice;
  d->ioDevice = 0;
  m_resultString = QString ( curl_easy_strerror ( d->ret ) );
  if ( d->ret )
    setError ( m_resultString );
  emit done ( bRet );
}

size_t Downloader::curlWrite ( const char * data, size_t size )
{
  fprintf ( stderr, "%s\n", data );
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
    d->progress->setMaximum ( dltotal );
    d->progress->setValue ( dlnow );
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
  setError( tr( "Canceled by user" ) );
}

void Downloader::setError ( const QString &errStr )
{
  qWarning( qPrintable(errStr) );
  emit error ( errStr );
}

QDebug &operator<< ( QDebug &debug, const Downloader & )
{
  return debug;
}

#include "downloader.moc"
#include "downloader_p.moc"
