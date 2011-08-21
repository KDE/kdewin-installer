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

#include "settingspage.h"
#include "ui_settingspage.h"
#include "installerenginesocial.h"
#include <QFileDialog>

SettingsPage::SettingsPage(QWidget *parent) :
    QWidget(parent),
    ui(new Ui::SettingsPage)
{
    ui->setupUi(this);
    connect(ui->browseButton,SIGNAL(clicked()),this,SLOT(browseButtonClicked()));
    connect(ui->saveButton,SIGNAL(clicked()),this,SLOT(saveButtonClicked()));

    Settings &s = Settings::instance();
    #ifndef Q_WS_WIN
        ui->proxyIE->setText(tr("Environment settings"));
    #endif
    ProxySettings proxy;
    switch (s.proxyMode()) {
        case ProxySettings::InternetExplorer:
            ui->proxyIE->setChecked(true);
            break;
        case ProxySettings::Manual:
            ui->proxyManual->setChecked(true);
            break;
        case ProxySettings::FireFox:
            ui->proxyFireFox->setChecked(true);
            break;
        case ProxySettings::Environment:
            ui->proxyIE->setChecked(true);
            break;
        case ProxySettings::None:
        default:
            ui->proxyOff->setChecked(true);
            break;
    }
    proxy.from(s.proxyMode());

    ui->proxyHost->setText(proxy.hostname);
    ui->proxyPort->setText(proxy.port ? QString("%1").arg(proxy.port) : QString());
    ui->proxyUserName->setText(proxy.user);
    ui->proxyPassword->setText(proxy.password);

    ui->proxyHost->setEnabled(ui->proxyManual->isChecked());
    ui->proxyPort->setEnabled(ui->proxyManual->isChecked());
    ui->proxyUserName->setEnabled(ui->proxyManual->isChecked());
    ui->proxyPassword->setEnabled(ui->proxyManual->isChecked());
    ui->rootFolder->setText(s.installDir());

}

SettingsPage::~SettingsPage()
{
    delete ui;
}

void SettingsPage::browseButtonClicked()
{
    QString fileName = ui->rootFolder->text();
    if( fileName.isEmpty() )
        fileName = QString::fromLocal8Bit( qgetenv( "ProgramFiles" ) );
    fileName = QFileDialog::getExistingDirectory(this,
                       tr("Select Root Installation Directory"),
                       fileName,
                       QFileDialog::ShowDirsOnly| QFileDialog::DontResolveSymlinks);
    if(!fileName.isEmpty())
        ui->rootFolder->setText(QDir::toNativeSeparators(fileName));


}

void SettingsPage::saveButtonClicked()
{
    emit setRoot(ui->rootFolder->text());

    Settings &s = Settings::instance();
    ProxySettings::ProxyMode m = ProxySettings::None;
    if(ui->proxyIE->isChecked())
    #ifdef Q_WS_WIN
          m = ProxySettings::InternetExplorer;
    #else
          m = ProxySettings::Environment;
    #endif
    if(ui->proxyFireFox->isChecked())
        m = ProxySettings::FireFox;
    if(ui->proxyManual->isChecked())
        m = ProxySettings::Manual;
    s.setProxyMode(m);
    if (ui->proxyManual->isChecked())
    {
        ProxySettings proxy;
        proxy.hostname = ui->proxyHost->text();
        proxy.port = ui->proxyPort->text().toInt();
        proxy.user = ui->proxyUserName->text();
        proxy.password = ui->proxyPassword->text();
        s.setProxy(proxy);
    }
    this->close();
}

void SettingsPage::setCurrentRoot(QString rootPath)
{
    //ui->rootFolder->setText(rootPath);
}

