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
