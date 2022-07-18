/****************************************************************************
**
** Copyright (C) 2008 Ralf Habacker <ralf.habacker@freenet.de> 
** All rights reserved.
**
** This file is part of the KDE installer for windows
**
** SPDX-License-Identifier: GPL-2.0-only
****************************************************************************/

#include "downloadsettingspage.h"

#include <QFileDialog>
#include <QMessageBox>

DownloadSettingsPage::DownloadSettingsPage() : InstallWizardPage(0)
{
    ui.setupUi(this);
    setTitle(windowTitle());
    setSubTitle(statusTip());

    connect( ui.tempPathSelect,SIGNAL(clicked()),this,SLOT(tempPathSelectClicked()) );
}

void DownloadSettingsPage::initializePage()
{
    Settings &s = Settings::instance();
    ui.tempPathEdit->setText(s.downloadDir());
}

bool DownloadSettingsPage::validatePage()
{
    Settings &s = Settings::instance();
    QFileInfo fi(ui.tempPathEdit->text());
    if (!fi.isWritable())
    {
        QMessageBox::critical(this,
                              tr("Error"),
                              tr("You do not have write permissions on the selected download directory."),
                              QMessageBox::Ok);
        return false;
    }
    s.setDownloadDir(ui.tempPathEdit->text());
    return true;
}

bool DownloadSettingsPage::isComplete()
{
    return !ui.tempPathEdit->text().isEmpty();
}

void DownloadSettingsPage::tempPathSelectClicked()
{
    QString fileName = QFileDialog::getExistingDirectory(this,
                       tr("Select Package Download Directory"),
                       "",
                       QFileDialog::ShowDirsOnly| QFileDialog::DontResolveSymlinks);
    if(!fileName.isEmpty())
        ui.tempPathEdit->setText(QDir::toNativeSeparators(fileName));
}
