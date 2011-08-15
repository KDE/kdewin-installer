#ifndef DESCRIPTIONBROWSER_H
#define DESCRIPTIONBROWSER_H

#include <QTextBrowser>

class DescriptionBrowser : public QTextBrowser
{
    Q_OBJECT
public:
    explicit DescriptionBrowser(QWidget *parent = 0);
    QVariant loadResource(int type, const QUrl &name);
signals:

public slots:

};

#endif // DESCRIPTIONBROWSER_H
