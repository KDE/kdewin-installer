#include "softwareitem.h"
#include <attica/icon.h>
SoftwareItem::SoftwareItem( Attica::Content *content)
{
    this->setText(content->name());
    //this->setIcon(QIcon::addFile(content->icons()));
    this->m_content = content;
    if (!content->icons().empty())
    {
        qDebug("There should be an Icon");
        Attica::Icon icon = content->icons().first();
        this->setIcon(QIcon(icon.url().toString()));
    }
}
Attica::Content * SoftwareItem::getContent()
{
    return m_content;
}
