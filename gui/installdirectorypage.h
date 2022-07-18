/****************************************************************************
**
** Copyright (C) 2008 Ralf Habacker <ralf.habacker@freenet.de> 
** All rights reserved.
**
** This file is part of the KDE installer for windows
**
** SPDX-License-Identifier: GPL-2.0-only
****************************************************************************/

#ifndef InstallDirectoryPage_H
#define InstallDirectoryPage_H

#include "installwizard.h"

#include "ui_installdirectorypage.h"

class InstallDirectoryPage : public InstallWizardPage
{
    Q_OBJECT

public:
    InstallDirectoryPage();

    void initializePage();
    bool isComplete();
    bool validatePage();
protected:
    Ui::InstallDirectoryPage ui;
    
protected slots:
    void rootPathSelectClicked();
};

#endif
