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

#include <QtCore/QObject>
#include <QtCore/QString>
#include <QtCore/QUrl>


class DownloaderProgress;
class Downloader : public QObject
{
  Q_OBJECT
public:
  typedef enum { Undefined, Finished, Failed, Aborted } ResultType;
  Downloader ( DownloaderProgress *progress=0, QObject *parent=0);
  virtual ~Downloader();

  // start download
  bool start ( const QUrl &url, const QString &fileName = QString() );
  bool start ( const QUrl &url, QByteArray &ba );
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

Q_SIGNALS:
  void done ( bool error );
  void error ( const QString &error );
protected Q_SLOTS:
  void threadFinished ( int ret ); // ret == CURLcode (enum)
  int progressCallback ( double ultotal, double ulnow );
protected:
  void setError ( const QString &errStr );
  bool startInternal ( const QUrl &url );
  size_t curlWrite ( const char * data, size_t  size );
  static size_t curlWriteCallback ( void *ptr, size_t size, size_t nmemb, void *stream );
  static int curlProgressCallback ( void *clientp, double dltotal, double dlnow,
                                    double ultotal, double ulnow );
protected:
  ResultType  m_result;
  QString     m_resultString;
  QUrl        m_usedURL;
  class Private;
  Private * const d;

  friend QDebug &operator<< ( QDebug &, const Downloader & );
};

#endif
