/****************************************************************************
**
** Copyright (C) 2008-2009 Ralf Habacker <ralf.habacker@freenet.de> 
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

#include "config.h"
#include "titlepage.h"

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
