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

#include <QtNetwork>
#include <QtCore>

#include "packagelist.h"
#include "package.h"
#include "downloader.h"

class DownloaderProgress {
	public:
		void show() {visible = true; }
		void hide() {visible = false; }
		void setLabel(const QString &label) { qDebug() << label; }
		void setMaximum(int value) {} 
		void setValue(int value) 
		{ 
			int unit = value/10240;
			if (oldunit != value/10240) {
				if (visible)
					putchar('.');
				oldunit = unit;
			}
		}

	private:
		int oldunit;
		bool visible;		
};


Downloader::Downloader(bool _blocking) 
{
	blocking = _blocking;
  progress = new DownloaderProgress();

	http = new QHttp(this);
	
	connect(http, SIGNAL(requestFinished(int, bool)),this, SLOT(httpRequestFinished(int, bool)));
	connect(http, SIGNAL(dataReadProgress(int, int)),this, SLOT(updateDataReadProgress(int, int)));
	connect(http, SIGNAL(responseHeaderReceived(const QHttpResponseHeader &)),this, SLOT(readResponseHeader(const QHttpResponseHeader &)));
	connect(http, SIGNAL(done(bool)),this, SLOT(allDone(bool)));
	connect(http, SIGNAL(stateChanged(int)),this, SLOT(stateChanged(int)));
}

Downloader::~Downloader() 
{
	delete progress;
	delete http;	
}

void Downloader::setError(const QString &text)
{
	qDebug() << text; 
}

bool Downloader::start(const QString &_url, const QString &_fileName)
{
	QString fileName = _fileName;
  QUrl url(_url);
	if (fileName == "") {
    QFileInfo fileInfo(url.path());
    fileName = fileInfo.fileName();
  }

  file = new QFile(fileName);
  if (!file->open(QIODevice::WriteOnly)) {
    setError(tr("Unable to save the file %1: %2.").arg(fileName).arg(file->errorString()));
    delete file;
    file = 0;
    return false;
  }

  http->setHost(url.host(), url.port() != -1 ? url.port() : 80);
  if (!url.userName().isEmpty())
    http->setUser(url.userName(), url.password());

  httpRequestAborted = false;
  QByteArray query = url.encodedQuery(); 
  httpGetId = http->get(url.path() + (!query.isEmpty() ? "?" + url.encodedQuery() : ""), file);

 	progress->show();
 	progress->setLabel(tr("Downloading %1 to %2.").arg(_url).arg(fileName));
	if (blocking) {
		eventLoop = new QEventLoop(); 
		eventLoop->exec();
		delete eventLoop;
	}		
	return true;
}

void Downloader::cancel()
{
	progress->setLabel(tr("Download canceled."));
	httpRequestAborted = true;
	http->abort();
}

void Downloader::httpRequestFinished(int requestId, bool error)
{
	if (httpRequestAborted) {
		if (file) {
			file->close();
			file->remove();
			delete file;
			file = 0;
		}
	
		if (progress)
			progress->hide();
			return;
	}
	
	if (requestId != httpGetId)
    return;
	
	if (progress)
	  progress->hide();
		file->close();
	
	if (error) {
    file->remove();
    setError(tr("Download failed: %1.").arg(http->errorString()));
	} else {
		progress->setLabel(tr("download ready"));
	}
	delete file;
	file = 0;
}

void Downloader::readResponseHeader(const QHttpResponseHeader &responseHeader)
{
	if (responseHeader.statusCode() != 200) {
		setError(tr("Download failed: %1.").arg(responseHeader.reasonPhrase()));
		httpRequestAborted = true;
		progress->hide();
		http->abort();
		return;
	}
}

void Downloader::updateDataReadProgress(int bytesRead, int totalBytes)
{
	if (httpRequestAborted)
	    return;
	
	if (progress) {
	  progress->setMaximum(totalBytes);
	  progress->setValue(bytesRead);
	}
}

void Downloader::allDone(bool error)
{
	emit done(error);
	if (blocking)
		eventLoop->quit();
}


void Downloader::stateChanged(int state)
{
	QString stateLabel;
	switch (state) {
		case QHttp::Unconnected  : stateLabel = "Unconnected"; break;
		case QHttp::HostLookup   : stateLabel = "HostLookup "; break;
		case QHttp::Connecting   : stateLabel = "Connecting "; break;
		case QHttp::Sending      : stateLabel = "Sending    "; break;
		case QHttp::Reading      : stateLabel = "Reading    "; break;
		case QHttp::Connected    : stateLabel = "Connected  "; break;
		case QHttp::Closing      : stateLabel = "Closing    "; break;
	}
	progress->setLabel(stateLabel);
}
