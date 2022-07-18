/****************************************************************************
**
** Copyright (C) 2008 Ralf Habacker <ralf.habacker@freenet.de> 
** All rights reserved.
**
** This file is part of the KDE installer for windows
**
** SPDX-License-Identifier: GPL-2.0-only
****************************************************************************/

#ifndef INSTALLPAGE_H
#define INSTALLPAGE_H

#include "installwizard.h"

class InstallPage : public InstallWizardPage
{
public:
    InstallPage();

    void cancel();
    void initializePage();
    bool isComplete();
    bool validatePage();
    void performAction();
};

#endif

