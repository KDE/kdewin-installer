/****************************************************************************
**
** Copyright (C) 2008 Ralf Habacker <ralf.habacker@freenet.de> 
** All rights reserved.
**
** This file is part of the KDE installer for windows
**
** SPDX-License-Identifier: GPL-2.0-only
****************************************************************************/

#ifndef MIRRORSETTINGSPAGE_H
#define MIRRORSETTINGSPAGE_H

#include "installwizard.h"
#include "ui_mirrorsettingspage.h"

class MirrorSettingsPage : public InstallWizardPage
{
    Q_OBJECT

public:
    MirrorSettingsPage();

    void initializePage();
    bool isComplete();
    bool validatePage();
    void cleanupPage();
    void performAction();

protected:
    Ui::MirrorSettingsPage ui;
    bool m_failed;
    
protected slots:
    void addNewMirrorClicked();
};


#endif
