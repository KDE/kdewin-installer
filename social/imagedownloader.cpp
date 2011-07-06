#include "imagedownloader.h"

ImageDownloader::ImageDownloader(QUrl imageUrl, QObject *parent) :
    QObject(parent)
{
    connect(&m_WebCtrl, SIGNAL(finished(QNetworkReply*)),this, SLOT(fileDownloaded(QNetworkReply*)));
    QNetworkRequest request(imageUrl);
    m_WebCtrl.get(request);
}

ImageDownloader::~ImageDownloader()
{

}

void ImageDownloader::fileDownloaded(QNetworkReply *pReply)
{
    m_DownloadedData = pReply->readAll();
    emit downloaded(this);
}
QByteArray ImageDownloader::dowloadedData()
{
    return m_DownloadedData;
}
