#include "descriptionbrowser.h"
#include "downloader.h"
DescriptionBrowser::DescriptionBrowser(QWidget *parent) :
    QTextBrowser(parent)
{
    this->setOpenExternalLinks(true);
}

QVariant DescriptionBrowser::loadResource(int type, const QUrl &name)
{
    if (type == QTextDocument::ImageResource)
    {
        QByteArray data;
        Downloader::instance()->fetch(name,data);
        return QVariant(data);

    }
    return QVariant();
}
