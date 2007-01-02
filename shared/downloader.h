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
class DownloaderProgress;
class QEventLoop;
class QFile;

class Downloader: public QObject
{
	Q_OBJECT

public:
  Downloader(bool blocking=false);
  virtual ~Downloader();
	bool start(const QString &url, const QString &fileName="");
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
	void setError(const QString&);
  QHttp *http;
  QFile *file;
  int httpGetId;
  bool httpRequestAborted;
  DownloaderProgress *progress;
  bool blocking;
  QEventLoop *eventLoop;
};

#endif
