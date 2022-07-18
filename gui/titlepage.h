/****************************************************************************
**
** Copyright (C) 2008 Ralf Habacker <ralf.habacker@freenet.de> 
** All rights reserved.
**
** This file is part of the KDE installer for windows
**
** SPDX-License-Identifier: GPL-2.0-only
****************************************************************************/

#ifndef TITLEPAGE_H
#define TITLEPAGE_H

#include "installwizard.h"
#include "ui_titlepage.h"

class TitlePage : public InstallWizardPage
{
public:
    TitlePage();

    void initializePage();
    bool validatePage();

protected: 
    Ui::TitlePage ui;
};

#endif
