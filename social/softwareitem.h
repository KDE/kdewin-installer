#ifndef SOFTWAREITEM_H
#define SOFTWAREITEM_H

#include <QListWidgetItem>
#include <QListWidget>
#include <attica/content.h>

class SoftwareItem : public QListWidgetItem
{
public:
    explicit SoftwareItem( Attica::Content *content = NULL);
    Attica::Content *getContent();
private:
    Attica::Content *m_content;
};

#endif // SOFTWAREITEM_H
