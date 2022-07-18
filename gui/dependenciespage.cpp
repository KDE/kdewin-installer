/****************************************************************************
**
** Copyright (C) 2008 Ralf Habacker <ralf.habacker@freenet.de> 
** All rights reserved.
**
** This file is part of the KDE installer for windows
**
** SPDX-License-Identifier: GPL-2.0-only
****************************************************************************/

#include "dependenciespage.h"

#include <QTreeWidget>

DependenciesPage::DependenciesPage() : InstallWizardPage(0)
{
    ui.setupUi(this);
    setTitle(windowTitle());
    setSubTitle(statusTip());
}

void DependenciesPage::initializePage()
{
    engine->checkUpdateDependencies(ui.dependenciesList);
    setSettingsButtonVisible(false);
}

bool DependenciesPage::validatePage()
{
    return true;
}

void DependenciesPage::performAction()
{
    if (ui.dependenciesList->topLevelItemCount() == 0)
        wizard()->next();
}
