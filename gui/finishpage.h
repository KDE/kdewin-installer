/****************************************************************************
**
** Copyright (C) 2008 Ralf Habacker <ralf.habacker@freenet.de>
** All rights reserved.
**
** This file is part of the KDE installer for windows
**
** SPDX-License-Identifier: GPL-2.0-only
****************************************************************************/

#ifndef FINISHPAGE_H
#define FINISHPAGE_H

#include "installwizard.h"

#include <QCheckBox>

class FinishPage : public InstallWizardPage
{
public:
    FinishPage();

    void initializePage();
    bool isComplete();
    bool validatePage();

protected:
    QLabel *label;
    QLabel *label2;
    QCheckBox *runSystemSettingsBox;
//    Ui::FinishPage ui;
};

#endif
