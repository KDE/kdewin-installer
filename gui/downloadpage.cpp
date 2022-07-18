/****************************************************************************
**
** Copyright (C) 2008 Ralf Habacker <ralf.habacker@freenet.de> 
** All rights reserved.
**
** This file is part of the KDE installer for windows
**
** SPDX-License-Identifier: GPL-2.0-only
****************************************************************************/

#include "downloadpage.h"
#include "downloaderprogress.h"
#include "downloader.h"

#include <QVBoxLayout>

DownloadPage::DownloadPage() : InstallWizardPage(0)
{
    setTitle(tr("Downloading Packages"));
    setSubTitle(tr(" "));

    DownloaderProgress *progress = new DownloaderProgress(this);
    QVBoxLayout *layout = new QVBoxLayout;
    layout->addWidget(progress);
    layout->addStretch(1);
    setLayout(layout);

    Downloader::instance()->setProgress(progress);
}

void DownloadPage::initializePage()
{
    setSettingsButtonVisible(false);
}

bool DownloadPage::isComplete()
{
    return true;
}

void DownloadPage::cancel()
{
    //@ TODO 
    engine->stop();
}

bool DownloadPage::validatePage()
{
    return true;
}

void DownloadPage::performAction()
{
    wizard()->button(QWizard::BackButton)->setEnabled(false);
    wizard()->button(QWizard::NextButton)->setEnabled(false);
    if (!engine->downloadPackages()) {
        wizard()->reject();
        return;
    }
    wizard()->button(QWizard::BackButton)->setEnabled(true);
    wizard()->button(QWizard::NextButton)->setEnabled(true);
    if (Settings::instance().autoNextStep())
        wizard()->next();
}
