/****************************************************************************
**
** Copyright (C) 2008-2009 Ralf Habacker <ralf.habacker@freenet.de> 
** All rights reserved.
**
** This file is part of the KDE installer for windows
**
** SPDX-License-Identifier: GPL-2.0-only
****************************************************************************/

#include "config.h"
#include "titlepage.h"

#include <QButtonGroup>

TitlePage::TitlePage()
{
    ui.setupUi(this);
    setTitle(windowTitle());
    setSubTitle(tr("Version " VERSION));
    // do not move it to initializePage() - it destroys our layout (qt4.4.0)
    // could not reproduce it with qt examples, therefore no bugreport :(
    setPixmap(QWizard::WatermarkPixmap, QPixmap(":/watermark.png"));
    // logical grouping isn't available in the designer yet :-P
    QButtonGroup *groupA = new QButtonGroup(this);
    groupA->addButton(ui.onlineInstallButton);
    groupA->addButton(ui.localInstallButton);
    groupA->addButton(ui.downloadOnlyButton);
    ui.onlineInstallButton->setChecked(true);
}

void TitlePage::initializePage()
{
    Settings &s = Settings::instance();
    ui.skipBasicSettings->setCheckState(s.isSkipBasicSettings() ? Qt::Checked : Qt::Unchecked);

    bool hided = s.isFirstRun() || !InstallWizard::skipSettings();
    ui.skipBasicSettings->setVisible(!hided);
    ui.skipBasicSettingsLabel->setVisible(!hided);
}

bool TitlePage::validatePage()
{
    Settings &s = Settings::instance();
    if (ui.localInstallButton->isChecked()) 
    {
        InstallerEngine::setInstallMode(InstallerEngine::localInstall);
        InstallerEngine::defaultConfigURL = "file:///" + s.downloadDir().replace("\\","/");
    }
    else if (ui.downloadOnlyButton->isChecked()) 
        InstallerEngine::setInstallMode(InstallerEngine::downloadOnly);
    else 
        InstallerEngine::setInstallMode(InstallerEngine::onlineInstall);

    s.setSkipBasicSettings(ui.skipBasicSettings->checkState() == Qt::Checked);
    return true;
}
