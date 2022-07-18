/****************************************************************************
**
** Copyright (C) 2008-2010 Ralf Habacker <ralf.habacker@freenet.de> 
** All rights reserved.
**
** This file is part of the KDE installer for windows
**
** SPDX-License-Identifier: GPL-2.0-only
****************************************************************************/

#ifndef USERCOMPILERMODEPAGE_H
#define USERCOMPILERMODEPAGE_H

#include "installwizard.h"

#include "ui_usercompilermodepage.h"

class UserCompilerModePage : public InstallWizardPage
{
    Q_OBJECT

public:
    UserCompilerModePage();

    void initializePage();
    bool isComplete();
    bool validatePage();
    
protected slots:
    void slotModeButtonClicked(int id);

protected:
    void setCompilerMode(bool);
    Ui::UserCompilerModePage ui;
};

#endif
