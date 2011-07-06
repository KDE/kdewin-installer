#include "installerprogresssocial.h"
#include "ui_installerprogresssocial.h"
#include <QtGui/QDesktopWidget>

InstallerProgressSocial::InstallerProgressSocial(QWidget *parent) :
    QWidget(parent),
    ui(new Ui::installerprogress)
{
    ui->setupUi(this);
    installedpackages = 0;
    packages = 0;
    this->hide();
    const QRect &r = QDesktopWidget().screenGeometry(this);
    this->move(r.width() / 2 - this->width() / 2, r.height() / 2 - this->height() / 2);
}

InstallerProgressSocial::~InstallerProgressSocial()
{
    delete ui;
}

void InstallerProgressSocial::update_progressbar(QString name)
{
    ui->progressBar->setValue((installedpackages*100)/packages);
    ui->software->setText(name);
}
void InstallerProgressSocial::InstallMenuItems()
{
    ui->software->setText(QLatin1String("Setting Start Menu Links.."));
}

void InstallerProgressSocial::FinishedInstallMenuItems()
{
    this->hide();
}
