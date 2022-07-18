/****************************************************************************
**
** Copyright (C) 2008-2011 Ralf Habacker <ralf.habacker@freenet.de> 
** All rights reserved.
**
** This file is part of the KDE installer for windows
**
** SPDX-License-Identifier: GPL-2.0-only
****************************************************************************/

#ifndef POSTPROCESSPAGE_H
#define POSTPROCESSPAGE_H

#include "installwizard.h"
#include "ui_postprocesspage.h"
#include "postprocessing.h"

class PostProcessPage : public InstallWizardPage
{
    Q_OBJECT

public:
    PostProcessPage();

    void initializePage();
    bool isComplete();
    bool validatePage();
    void cleanupPage();
    void performAction();

protected slots:
    void postProcessingEnd();
    void addItem(const QString &label);

protected:
    Ui::PostProcessPage ui;
    PostProcessing m_postProcessing;
};


#endif
