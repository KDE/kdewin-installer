/****************************************************************************
**
** Copyright (C) 2008 Ralf Habacker <ralf.habacker@freenet.de> 
** All rights reserved.
**
** This file is part of the KDE installer for windows
**
** SPDX-License-Identifier: GPL-2.0-only
****************************************************************************/

#ifndef DOWNLOADSETTINGSPAGE_H
#define DOWNLOADSETTINGSPAGE_H

#include "installwizard.h"
#include "ui_downloadsettingspage.h"

class DownloadSettingsPage : public InstallWizardPage
{
    Q_OBJECT

public:
    DownloadSettingsPage();

    void initializePage();
    bool isComplete();
    bool validatePage();

protected: 
    Ui::DownloadSettingsPage ui;

protected slots:
    void tempPathSelectClicked();
};

#endif
