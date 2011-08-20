/****************************************************************************
**
** Copyright (C) 2011 Constantin Tudorica <tudalex@gmail.com>
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

#include "imagedownloader.h"
#include <QNetworkReply>
#include <QMessageBox>
#include <QDebug>

ImageDownloader::ImageDownloader(QUrl imageUrl, QObject *parent) :
    QObject(parent)
{


    QNetworkRequest request(imageUrl);

    m_Reply  = m_WebCtrl.get(request);
    connect(m_Reply, SIGNAL(error(QNetworkReply::NetworkError)),this, SLOT(error(QNetworkReply::NetworkError)) );
    connect(m_Reply, SIGNAL(finished()),this, SLOT(fileDownloaded()));
}

ImageDownloader::~ImageDownloader()
{

}

void ImageDownloader::fileDownloaded()
{

    m_DownloadedData = m_Reply->readAll();

    emit downloaded();
}
QByteArray ImageDownloader::dowloadedData()
{
    return m_DownloadedData;
}

void ImageDownloader::error(QNetworkReply::NetworkError code)
{
    qDebug()<<"There has been an error";
}
