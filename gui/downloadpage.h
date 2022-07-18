/****************************************************************************
**
** Copyright (C) 2008 Ralf Habacker <ralf.habacker@freenet.de> 
** All rights reserved.
**
** This file is part of the KDE installer for windows
**
** SPDX-License-Identifier: GPL-2.0-only
****************************************************************************/

#ifndef DOWNLOADPAGE_H
#define DOWNLOADPAGE_H

#include "installwizard.h"

class DownloadPage : public InstallWizardPage
{
public:
    DownloadPage();

    void cancel();
    void initializePage();
    bool isComplete();
    bool validatePage();
    void performAction();
};

#endif
