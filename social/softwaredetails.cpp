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

#include "softwaredetails.h"
#include "ui_softwaredetails.h"
#include <attica/content.h>
#include "debug.h"

SoftwareDetails::SoftwareDetails(QWidget *parent) :
    QWidget(parent),
    ui(new Ui::SoftwareDetails)
{
    ui->setupUi(this);
    connect(ui->install_button,SIGNAL(pressed()),this,SLOT(installbuttonclicked()));
}

SoftwareDetails::~SoftwareDetails()
{
    delete ui;
}
void SoftwareDetails::setContent(Attica::Content * content)
{
    ui->textBrowser->setText(content->description());
    ui->software_title->setText(content->name());
    m_content = content;

}

void SoftwareDetails::installbuttonclicked()
{
    emit installpackage(m_content->downloadUrlDescription(1).packageName());
    qDebug()<<"download signal dispatched with the packagename:"<<m_content->downloadUrlDescription(1).packageName();
}
