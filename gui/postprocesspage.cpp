/****************************************************************************
**
** Copyright (C) 2008-2011 Ralf Habacker <ralf.habacker@freenet.de> 
** All rights reserved.
**
** This file is part of the KDE installer for windows
**
** SPDX-License-Identifier: GPL-2.0-only
****************************************************************************/

#include "config.h"
#include "debug.h"
#include "misc.h"
#include "postprocesspage.h"
#include "selfinstaller.h"

#include <QProcess>

PostProcessPage::PostProcessPage() : InstallWizardPage(0), m_postProcessing(engine,this)
{
    ui.setupUi(this);
    setTitle(windowTitle());
    setSubTitle(statusTip());
}

void PostProcessPage::initializePage()
{
    wizard()->button(QWizard::NextButton)->setEnabled(false);
}

void PostProcessPage::addItem(const QString &label)
{
    ui.listWidget->addItem(label);
    ui.listWidget->repaint();
}

void PostProcessPage::performAction()
{
    if (engine->installedPackages() > 0 && !SelfInstaller::instance().isInstalled())
		SelfInstaller::instance().install();

    connect(&m_postProcessing,SIGNAL(numberOfCommands(int)),ui.progressBar,SLOT(setMaximum(int)));
    connect(&m_postProcessing,SIGNAL(finished()),this,SLOT(postProcessingEnd()));
    connect(&m_postProcessing,SIGNAL(commandStarted(int)),ui.progressBar,SLOT(setValue(int)));
    connect(&m_postProcessing,SIGNAL(commandStarted(const QString &)),this,SLOT(addItem(const QString &)));
    m_postProcessing.start();
}

void PostProcessPage::postProcessingEnd()
{
    disconnect(&m_postProcessing,SIGNAL(numberOfCommands(int)),ui.progressBar,SLOT(setMaximum(int)));
    disconnect(&m_postProcessing,SIGNAL(finished()),this,SLOT(postProcessingEnd()));
    disconnect(&m_postProcessing,SIGNAL(commandStarted(int)),ui.progressBar,SLOT(setValue(int)));
    disconnect(&m_postProcessing,SIGNAL(commandStarted(const QString &)),this,SLOT(addItem(const QString &)));
    wizard()->button(QWizard::NextButton)->setEnabled(true);
    if (Settings::instance().autoNextStep())
        wizard()->next();
}

bool PostProcessPage::validatePage()
{
    return true;
}

void PostProcessPage::cleanupPage()
{
}

bool PostProcessPage::isComplete()
{
    return true;
}
