#ifndef CATEGORYBUTTON_H
#define CATEGORYBUTTON_H

#include <QCommandLinkButton>
#include <attica/provider.h>
class CategoryButton : public QCommandLinkButton
{
    Q_OBJECT
public:
    explicit CategoryButton(QWidget *parent = 0, Attica::Category *cat = NULL);
    Attica::Category *category;
signals:

public slots:

};

#endif // CATEGORYBUTTON_H
