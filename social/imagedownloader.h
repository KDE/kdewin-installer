#ifndef IMAGEDOWNLOADER_H
#define IMAGEDOWNLOADER_H

#include <QObject>
#include <QByteArray>
#include <QNetworkAccessManager>
#include <QNetworkRequest>
#include <QNetworkReply>

class ImageDownloader : public QObject
{
    Q_OBJECT
public:
    explicit ImageDownloader(QUrl imageUrl, QObject *parent = 0);
    virtual ~ImageDownloader();
    QByteArray dowloadedData();
signals:
    void downloaded(ImageDownloader* const);
public slots:
    void fileDownloaded(QNetworkReply* pReply);
private:
    QNetworkAccessManager m_WebCtrl;
    QByteArray m_DownloadedData;
};

#endif // IMAGEDOWNLOADER_H
