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

#include <QApplication>
#include <QBuffer>
#include <QDebug>
#include <QEventLoop>
#include <QFileInfo>
#include <QHttp>
#include <QString>
#include <QUrl>
#include <QDateTime>

#include "packagelist.h"
#include "package.h"
#include "downloader.h"
#include "downloaderprogress.h"
#include "settings.h"
#include "misc.h"

/// @TODO: check operation in non blocking mode

Downloader::Downloader(bool _blocking, DownloaderProgress *_progress)
  : m_progress(_progress), m_http(0), m_ioDevice(0), m_httpGetId(~0U),
    m_httpRequestAborted(false), m_blocking(_blocking), m_eventLoop(0)
{
    init();
}

void Downloader::init()
{
    m_http = new QHttp(this);
    connect(m_http, SIGNAL(requestFinished(int, bool)),this, SLOT(httpRequestFinished(int, bool)));
    connect(m_http, SIGNAL(dataReadProgress(int, int)),this, SLOT(updateDataReadProgress(int, int)));
    connect(m_http, SIGNAL(responseHeaderReceived(const QHttpResponseHeader &)),this, SLOT(readResponseHeader(const QHttpResponseHeader &)));
    connect(m_http, SIGNAL(done(bool)),this, SLOT(allDone(bool)));
    connect(m_http, SIGNAL(stateChanged(int)),this, SLOT(stateChanged(int)));
}

Downloader::~Downloader()
{
    m_httpRequestAborted = true;
    if(m_http) {
      m_http->abort();
      qApp->processEvents();
    }
    delete m_ioDevice;
}

void Downloader::setError(const QString &str)
{
    qWarning(qPrintable(str));
}

bool Downloader::start(const QUrl &url, const QString &fileName)
{
    if(url.isEmpty())
        return true;
    if (m_progress)
        m_progress->setTitle(tr("Downloading %1 to %2").arg(url.toString()).arg(fileName));

    QString scheme = url.scheme();
    if (scheme.isEmpty() || scheme == QLatin1String("file"))
    {
        QFile::remove(fileName);
        QFile::copy(url.toLocalFile(),fileName);
        m_fileName = fileName;
        return true;
    }

    QFile *file = new QFile(fileName);
    if (!file->open(QIODevice::WriteOnly))
    {
        setError(tr("Unable to open file %1: %2.").arg(fileName).arg(file->errorString()));
        delete file;
        m_fileName = "";
        return false;
    }
    m_fileName = fileName;

    qDebug() << "Downloading" << url.toString() << " to " << file->fileName();
    return startInternal(url, file);
}

bool Downloader::start(const QUrl &url, QByteArray &ba)
{
    m_fileName = "";
    
    if(url.isEmpty())
        return true;
    if (m_progress)
        m_progress->setTitle(tr("Downloading %1").arg(url.toString()));

    QBuffer *buf = new QBuffer(&ba);
    if(!buf->open(QIODevice::WriteOnly))
    {
        setError(tr("Internal error!"));
        return false;
    }

    QString scheme = url.scheme();
    if (scheme.isEmpty() || scheme == QLatin1String("file"))
    {
      QFile f(url.toLocalFile());
      if(!f.open(QIODevice::ReadOnly)) {
        setError(tr("Unable to open file %1: %2.").arg(f.fileName()).arg(f.errorString()));
        return false;
      }
      ba = f.readAll();
      return true;
    }

    qDebug() << "Downloading" << url.toString() << " to memory";
    return startInternal(url, buf);
}

bool Downloader::startInternal(const QUrl &url, QIODevice *ioDev)
{
    Settings &s = Settings::getInstance();
    QNetworkProxy ps;

    s.proxy(url.scheme(),ps);
    if (!ps.hostName().isEmpty())
    {
        m_http->setProxy(ps);
        qDebug() << "Downloader proxy settings -" << ps;
    }
    m_ioDevice = ioDev;

    m_http->setHost(url.host(), url.port() != -1 ? url.port() : 80);
    if (!url.userName().isEmpty())
        m_http->setUser(url.userName(), url.password());

    QByteArray query = url.encodedQuery();
    m_httpRequestAborted = false;
    m_result = Undefined;
    m_usedURL = url;
    m_httpGetId = m_http->get
                  (url.path() + (!query.isEmpty() ? "?" + url.encodedQuery() : QString()), m_ioDevice);

    if (m_progress)
        m_progress->show();
    if (m_blocking)
    {
        m_eventLoop = new QEventLoop();
        m_eventLoop->exec();
        delete m_eventLoop;
        if (m_result == Redirected) {
            qDebug() << "restarted download from" << m_redirectedURL;
            m_ioDevice->seek(0); // make sure we are on the start of the file or buffer 
            startInternal(m_redirectedURL,m_ioDevice);
        }
        return m_result == Finished ? true : false;
    }
    return true;
}

void Downloader::cancel()
{
    if (m_progress)
        m_progress->setStatus(tr("Download canceled."));
    m_httpRequestAborted = true;
    qDebug() << "Download canceled.";
    m_http->abort();
    if (!m_fileName.isEmpty())
        QFile::remove(m_fileName);
}

void Downloader::httpRequestFinished(int requestId, bool error)
{
    qDebug() << __FUNCTION__ << requestId << error;
    if (requestId != m_httpGetId)
      return;

    if (m_result == Redirected) {
        // redirect will be started in startInternal() for non blocking mode
        return;
    }

    if (m_httpRequestAborted) {
        m_result = Aborted;
        QFile *f = qobject_cast<QFile*>(m_ioDevice);
        if (f)
            f->remove();
        setError(tr("Download aborted: %1.").arg(m_http->errorString()));
    }
    else if (error) {
        m_result = Failed;
        QFile *f = qobject_cast<QFile*>(m_ioDevice);
        if (f)
            f->remove();
        setError(tr("Download failed: %1.").arg(m_http->errorString()));
    }
    else {
        m_result = Finished;
        if (m_progress)
            m_progress->setStatus(tr("download ready"));
    }

    if (m_progress)
      m_progress->hide();

    m_ioDevice->close();
    delete m_ioDevice;
    m_ioDevice = 0;
}

void Downloader::readResponseHeader(const QHttpResponseHeader &responseHeader)
{
    m_statusCode = responseHeader.statusCode();
    if (m_statusCode == 301 || m_statusCode == 302) {
        qWarning() << __FUNCTION__ << "Download failed" << m_statusCode << responseHeader.reasonPhrase() << "new location:" << responseHeader.value("location");
        setError(tr("Download failed: %1 %2.").arg(m_statusCode).arg(responseHeader.reasonPhrase() + " new location: " + responseHeader.value("location")) );
        m_redirectedURL = responseHeader.value("location");
        m_result = Redirected;
    }
    else if (m_statusCode != 200)
    {
        qWarning() << __FUNCTION__ << "Download failed" << m_statusCode << responseHeader.reasonPhrase();
        foreach(QString key, responseHeader.keys()) 
            qWarning()  << key << responseHeader.allValues(key);
        setError(tr("Download failed: %1 %2.").arg(m_statusCode).arg(responseHeader.reasonPhrase()));
        m_result = Failed;
    }
    return;
}

void Downloader::updateDataReadProgress(int bytesRead, int totalBytes)
{
    if (m_httpRequestAborted)
        return;

    if (m_progress)
    {
        m_progress->setMaximum(totalBytes);
        m_progress->setValue(bytesRead);
    }
}

void Downloader::allDone(bool error)
{
    emit done(error);
    if (m_blocking)
        m_eventLoop->quit();
}


void Downloader::stateChanged(int state)
{
    QString stateLabel;
    switch (state)
    {
    case QHttp::Unconnected  :
        stateLabel = tr("Unconnected");
        break;
    case QHttp::HostLookup   :
        stateLabel = tr("HostLookup ");
        break;
    case QHttp::Connecting   :
        stateLabel = tr("Connecting ");
        break;
    case QHttp::Sending      :
        stateLabel = tr("Sending    ");
        break;
    case QHttp::Reading      :
        stateLabel = tr("Reading    ");
        break;
    case QHttp::Connected    :
        stateLabel = tr("Connected  ");
        break;
    case QHttp::Closing      :
        stateLabel = tr("Closing    ");
        break;
    }
    if (m_progress)
        m_progress->setStatus(stateLabel);
    qDebug() << "Downloader state changed:" << stateLabel;
}

QDebug &operator<<(QDebug &out, const Downloader &c)
{
    out << "Downloader ("
        << "m_blocking:"      << c.m_blocking
        << "m_resultString:"  << c.m_resultString
        << "m_result:"        << c.m_result      
        << "m_statusCode:"    << c.m_statusCode
        << "m_redirectedURL:" << c.m_redirectedURL
        << "m_usedURL:"       << c.m_usedURL
        << ")";
    return out;
}

#include "downloader.moc"
