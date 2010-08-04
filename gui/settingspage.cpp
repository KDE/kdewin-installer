/****************************************************************************
**
** Copyright (C) 2008 Ralf Habacker <ralf.habacker@freenet.de> 
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
