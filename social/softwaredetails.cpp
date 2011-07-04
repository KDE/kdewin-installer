#include "softwaredetails.h"
#include "ui_softwaredetails.h"

SoftwareDetails::SoftwareDetails(QWidget *parent) :
    QWidget(parent),
    ui(new Ui::SoftwareDetails)
{
    ui->setupUi(this);
}

SoftwareDetails::~SoftwareDetails()
{
    delete ui;
}
