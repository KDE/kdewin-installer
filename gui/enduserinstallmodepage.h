/****************************************************************************
**
** Copyright (C) 2008 Ralf Habacker <ralf.habacker@freenet.de> 
** All rights reserved.
**
** This file is part of the KDE installer for windows
**
** SPDX-License-Identifier: GPL-2.0-only
****************************************************************************/

#ifndef ENDUSERINSTALLMODEPAGE_H
#define ENDUSERINSTALLMODEPAGE_H

#include "installwizard.h"
#include "ui_enduserinstallmodepage.h"

class EndUserInstallModePage : public InstallWizardPage
{
public:
    typedef enum {Nothing, Update, Remove, Repair } InstallMode;
    EndUserInstallModePage();

    void initializePage();
    bool validatePage();
    InstallMode selectedInstallMode();

protected: 
      Ui::EndUserInstallModePage ui;
};

#endif
