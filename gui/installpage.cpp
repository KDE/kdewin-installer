/****************************************************************************
**
** Copyright (C) 2008 Ralf Habacker <ralf.habacker@freenet.de> 
** All rights reserved.
**
** This file is part of the KDE installer for windows
**
** SPDX-License-Identifier: GPL-2.0-only
****************************************************************************/

#include "installpage.h"
#include "installerprogress.h"
#include "unpacker.h"

#include <QVBoxLayout>

InstallPage::InstallPage() : InstallWizardPage(0)
{
    setTitle(tr("Installing packages"));
    setSubTitle(tr(" "));

    InstallerProgress *progress = new InstallerProgress(this);
    QVBoxLayout *layout = new QVBoxLayout;
    layout->addWidget(progress);
    layout->addStretch(1);
    setLayout(layout);
    Unpacker::instance()->setProgress(progress);
}

void InstallPage::initializePage()
{
}

bool InstallPage::isComplete()
{
    return true;
}

void InstallPage::cancel()
{
    engine->stop();
}

bool InstallPage::validatePage()
{
    return true;
}

void InstallPage::performAction()
{
    wizard()->button(QWizard::BackButton)->setEnabled(false);
    wizard()->button(QWizard::NextButton)->setEnabled(false);
    if (!engine->installPackages()) {
        wizard()->reject();
        return;
    }
    wizard()->button(QWizard::BackButton)->setEnabled(true);
    wizard()->button(QWizard::NextButton)->setEnabled(true);
    if (Settings::instance().autoNextStep())
        wizard()->next();
}
