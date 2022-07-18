/****************************************************************************
**
** Copyright (C) 2008 Ralf Habacker <ralf.habacker@freenet.de> 
** All rights reserved.
**
** This file is part of the KDE installer for windows
**
** SPDX-License-Identifier: GPL-2.0-only
****************************************************************************/

#ifndef RELEASESELECTIONPAGE_H
#define RELEASESELECTIONPAGE_H

#include "installwizard.h"
#include "ui_releaseselectionpage.h"

class ReleaseSelectionPage : public InstallWizardPage
{
    Q_OBJECT

public:
    ReleaseSelectionPage();

    void initializePage();
    bool isComplete();
    bool validatePage();
    void cleanupPage();
    void performAction();

protected:
    Ui::ReleaseSelectionPage ui;
    bool m_failed;
    };


#endif
