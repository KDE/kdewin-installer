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
