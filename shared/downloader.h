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

#ifndef DOWNLOADER_H
#define DOWNLOADER_H

#include <QObject>
#include <QUrl>

class DownloaderProgress;
class QEventLoop;
class QFile;
class QHttp;
class QHttpResponseHeader;
class QIODevice;
class QString;

class Downloader: public QObject
{
    Q_OBJECT

public:
    typedef enum { Undefined, Finished, Failed, Aborted, Redirected } ResultType;
    Downloader(bool blocking=false, DownloaderProgress *progress=0);

    virtual ~Downloader();
    // set http proxy
    void setProxy(const QString &host, int port);

    // start download
    bool start(const QUrl &url, const QString &fileName = QString());
    bool start(const QUrl &url, QByteArray &ba);
    /// cancel started download 
    void cancel();
    /// return result string 
    QString resultString() { return m_resultString; }
    /// return download result
    ResultType result()    { return m_result; }
    /// return real used url for downloading
    const QUrl &usedURL()   { return m_usedURL; }

signals:
    void done(bool error);

private slots:
    void httpRequestFinished(int requestId, bool error);
    void readResponseHeader(const QHttpResponseHeader &responseHeader);
    void updateDataReadProgress(int bytesRead, int totalBytes);
    void allDone(bool error);
    void stateChanged(int state);

private:
    void init();
    void setError(const QString&);
    bool startInternal(const QUrl &url, QIODevice *ioDev);
private:
    DownloaderProgress *m_progress;
    QHttp      *m_http;
    QIODevice  *m_ioDevice;
    int         m_httpGetId;
    bool        m_httpRequestAborted;
    bool        m_blocking;
    QEventLoop *m_eventLoop;
    QString		m_resultString;
    ResultType  m_result;      
    int         m_statusCode;       // used internal
    QUrl        m_redirectedURL;    // holds redirected url when request finished with 302 status 
    QUrl        m_usedURL;          // holds really used url, may change when 302 status 
    friend QDebug &operator<<(QDebug &, const Downloader &);
};

#endif
