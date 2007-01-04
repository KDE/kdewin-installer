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

#include <QtCore>
class QHttpResponseHeader;
class QHttp;
class QEventLoop;
class QIODevice;
class DownloaderProgress;

class Downloader: public QObject
{
	Q_OBJECT

public:
	Downloader(bool blocking=false, DownloaderProgress *progress=0);

  virtual ~Downloader();
  bool start(const QString &url, const QString &fileName = QString::null);
  bool start(const QString &url, QByteArray &ba);
	void cancel();

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
  bool startInternal(const QString &_url, QIODevice *ioDev);
private:
  DownloaderProgress *m_progress;
  QHttp      *m_http;
  QIODevice  *m_ioDevice;
  QFile      *m_file;
  int         m_httpGetId;
  bool        m_httpRequestAborted;
  bool        m_blocking;
  QEventLoop *m_eventLoop;
};

#endif
