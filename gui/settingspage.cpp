/****************************************************************************
**
** Copyright (C) 2008 Ralf Habacker <ralf.habacker@freenet.de> 
** All rights reserved.
**
** This file is part of the KDE installer for windows
**
** SPDX-License-Identifier: GPL-2.0-only
****************************************************************************/

#include "settings.h"
#include "settingspage.h"
#include "database.h"
#include "mirrors.h"

#include "ui_settingspage.h"

#include <QtDebug>
#include <QFileDialog>
#include <QInputDialog>

SettingsPage::SettingsPage(QWidget *parent)
    : QDialog(parent), ui(new Ui::SettingsDialog),  s(Settings::instance())
{
//    init();
}

void SettingsPage::init()
{
    // required to reset changes from single page access
    ui->setupUi(this);
    ui->displayTitlePage->setCheckState(s.isPackageManagerMode() ? Qt::Checked : Qt::Unchecked);
    ui->installDetails->setCheckState(s.installDetails() ? Qt::Checked : Qt::Unchecked);
    ui->autoNextStep->setCheckState(s.autoNextStep() ? Qt::Checked : Qt::Unchecked);

}

void SettingsPage::accept()
{
    hide();

    s.setPackageManagerMode(ui->displayTitlePage->checkState() == Qt::Checked ? true : false);
    s.setInstallDetails(ui->installDetails->checkState() == Qt::Checked ? true : false);
    s.setAutoNextStep(ui->autoNextStep->checkState() == Qt::Checked ? true : false);
}

void SettingsPage::reject()
{
    hide();
    init(); // reinit page to restore old settings, is this really required ?
}

#if test
int main(int argc, char **argv)
{
    QApplication app(argc, argv);

    SettingsPage settingsPage;

    settingsPage.show();
    app.exec();
}
#endif
