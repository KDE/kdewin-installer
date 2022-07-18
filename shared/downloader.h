/****************************************************************************
**
** Copyright (C) 2005-2017 Ralf Habacker. All rights reserved.
**
** This file is part of the KDE installer for windows
**
** SPDX-License-Identifier: GPL-2.0-only
****************************************************************************/

#ifndef DOWNLOADER_H
#define DOWNLOADER_H

#include "hash.h"
#include "downloaderprogress.h"


#include <QObject>
#include <QSslError>
#include <QString>
#include <QUrl>

class DownloaderProgress;
class QEventLoop;
class QNetworkReply;

class Downloader : public QObject
{
    Q_OBJECT
public:
    typedef enum { Undefined, Finished, Failed, Aborted } ResultType;
public:
    // dtor
    virtual ~Downloader();
    // singleton
    static Downloader *instance();
    // for user interaction
    void setProgress ( DownloaderProgress *progress );
    DownloaderProgress *progress();
    // start download
    bool fetch ( const QUrl &url, const QString &fileName = QString() );
    bool fetch ( const QUrl &url, QByteArray &ba );

    /// cancel started download
    void cancel();
    /// return result string
    QString resultString() const {
        return m_resultString;
    }
    /// return download result
    ResultType result()    const {
        return m_result;
    }
    /// return real used url for downloading
    QUrl       usedURL()   const {
        return m_usedURL;
    }
    // set checksum type
    void setCheckSumType(Hash::Type type);
    
    // calculated checksum
    QByteArray checkSum() const;
    
Q_SIGNALS:
    void done ( bool error );
    void error ( const QString &error );
protected Q_SLOTS:
    void slotSslErrors(QNetworkReply *reply, const QList<QSslError> &errors);
    void slotReadyRead ();
    void slotReplyFinished ( QNetworkReply*reply );
    int slotProgressCallback ( qint64 now, qint64 total );
protected:
    bool startRequest(const QUrl &url);
    void setError ( const QString &errStr );
    bool fetchInternal ( const QUrl &url );

protected:
    ResultType  m_result;
    QString     m_resultString;
    QUrl        m_usedURL;
    QByteArray  m_checkSum;
    QEventLoop *m_loop;
    class Private;
    Private * const d;

    friend QDebug &operator<< ( QDebug &, const Downloader & );
private:
    Downloader ();

    friend class DownloaderSingleton;
};

#endif
