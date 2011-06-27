#include "categorybutton.h"

CategoryButton::CategoryButton(QWidget *parent, Attica::Category *cat) :
    QCommandLinkButton(parent)
{
    if (cat!=NULL)
    {
        category = cat;
        this->setText(cat->name());
    }
}
