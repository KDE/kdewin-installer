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

#include "config.h"
#include "titlepage.h"

TitlePage::TitlePage()
{
    setTitle(tr("KDE for Windows Installer"));
    setSubTitle(tr("Version " VERSION));
    ui.setupUi(this);
}

void TitlePage::initializePage()
{
    setPixmap(QWizard::WatermarkPixmap, QPixmap(":/images/watermark.png"));
    Settings &s = Settings::instance();
    ui.skipBasicSettings->setCheckState(s.isSkipBasicSettings() ? Qt::Checked : Qt::Unchecked);
}

int TitlePage::nextId() const
{
    if (ui.skipBasicSettings->checkState() == Qt::Checked)
    {
        if (GlobalConfig::isRemoteConfigAvailable())
            return InstallWizard::packageSelectorPage;
        else        
            return InstallWizard::mirrorSettingsPage;
    }
    else
        return InstallWizard::installDirectoryPage;
}

bool TitlePage::validatePage()
{
    Settings &s = Settings::instance();
    s.setSkipBasicSettings(ui.skipBasicSettings->checkState() == Qt::Checked);
    return true;
}
