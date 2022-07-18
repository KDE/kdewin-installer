/****************************************************************************
**
** Copyright (C) 2008 Ralf Habacker <ralf.habacker@freenet.de> 
** All rights reserved.
**
** This file is part of the KDE installer for windows
**
** SPDX-License-Identifier: GPL-2.0-only
****************************************************************************/

#include "uninstallpage.h"
#include "installerprogress.h"
#include "uninstaller.h"

#include <QVBoxLayout>

UninstallPage::UninstallPage()
{
    setTitle(tr("Uninstalling packages"));
    setSubTitle(tr(" "));

    InstallerProgress *progress = new InstallerProgress(this,false);
    QVBoxLayout *layout = new QVBoxLayout;
    layout->addWidget(progress);
    layout->addStretch(1);
    setLayout(layout);
    Uninstaller::instance()->setProgress(progress);
}

void UninstallPage::initializePage()
{
}

bool UninstallPage::isComplete()
{
    return true;
}

void UninstallPage::cancel()
{
    engine->stop();
}

bool UninstallPage::validatePage()
{
    return true;
}

void UninstallPage::performAction()
{       
    wizard()->button(QWizard::BackButton)->setEnabled(false);
    wizard()->button(QWizard::NextButton)->setEnabled(false);
    if (!engine->removePackages()) {
        wizard()->reject();
        return;
    }
    wizard()->button(QWizard::BackButton)->setEnabled(true);
    wizard()->button(QWizard::NextButton)->setEnabled(true);
    if (Settings::instance().autoNextStep())
        wizard()->next();
}
