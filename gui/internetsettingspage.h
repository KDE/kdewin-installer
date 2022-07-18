/****************************************************************************
**
** Copyright (C) 2008 Ralf Habacker <ralf.habacker@freenet.de>
** All rights reserved.
**
** This file is part of the KDE installer for windows
**
** SPDX-License-Identifier: GPL-2.0-only
****************************************************************************/

#ifndef INTERNETSETTINGSPAGE_H
#define INTERNETSETTINGSPAGE_H

#include "installwizard.h"
#include "ui_internetsettingspage.h"

class InternetSettingsPage : public InstallWizardPage
{
    Q_OBJECT

public:
    InternetSettingsPage();

    void initializePage();
    bool isComplete();
    bool validatePage();

protected:
    Ui::InternetSettingsPage ui;

protected slots:
    void switchProxyFields(bool mode);
};


#endif
