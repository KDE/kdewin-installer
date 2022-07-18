/****************************************************************************
**
** Copyright (C) 2008 Ralf Habacker <ralf.habacker@freenet.de>
** All rights reserved.
**
** This file is part of the KDE installer for windows
**
** SPDX-License-Identifier: GPL-2.0-only
****************************************************************************/

#ifndef DEPENDENCIESPAGE_H
#define DEPENDENCIESPAGE_H


#include "installwizard.h"

#include "ui_dependenciespage.h"

class PackageSelectorPage;

class DependenciesPage: public InstallWizardPage
{
public:
    DependenciesPage();

    void initializePage();
    bool validatePage();
    void performAction();

protected:
    Ui::DependenciesPage ui;

private:
    friend class PackageSelectorPage;
};

#endif
