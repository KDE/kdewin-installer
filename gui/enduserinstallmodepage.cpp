/****************************************************************************
**
** Copyright (C) 2008 Ralf Habacker <ralf.habacker@freenet.de> 
** All rights reserved.
**
** This file is part of the KDE installer for windows
**
** SPDX-License-Identifier: GPL-2.0-only
****************************************************************************/

#include "config.h"
#include "enduserinstallmodepage.h"
#include "installerdialogs.h"

#include <QButtonGroup>

EndUserInstallModePage::EndUserInstallModePage() : InstallWizardPage(0)
{
    ui.setupUi(this);
    setTitle(windowTitle());
    setSubTitle(statusTip());
    // logical grouping isn't available in the designer yet :-P
    QButtonGroup *groupA = new QButtonGroup(this);
    groupA->addButton(ui.updateInstallButton);
    groupA->addButton(ui.repairInstallButton);
    groupA->addButton(ui.removeInstallButton);
    ui.updateInstallButton->setChecked(true);
    ui.repairInstallButton->setVisible(false);
}

void EndUserInstallModePage::initializePage()
{
}

EndUserInstallModePage::InstallMode EndUserInstallModePage::selectedInstallMode()
{
    if (ui.updateInstallButton->isChecked())
        return Update;
    else if (ui.repairInstallButton->isChecked())
        return Repair;
    else if (ui.removeInstallButton->isChecked())
        return Remove;
    else 
        return Nothing;
}

bool EndUserInstallModePage::validatePage()
{
    if (ui.removeInstallButton->isChecked())
    {
        if (InstallerDialogs::instance().confirmRemovalDialog())
            engine->selectAllPackagesForRemoval();
    }
    else if (ui.repairInstallButton->isChecked())
    {
        if (InstallerDialogs::instance().confirmRepairDialog())
            engine->selectPackagesForReinstall();
    }
    return true;
}

