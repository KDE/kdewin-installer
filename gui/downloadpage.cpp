/****************************************************************************
**
** Copyright (C) 2008 Ralf Habacker <ralf.habacker@freenet.de> 
** All rights reserved.
**
** This file is part of the KDE installer for windows
**
** This file may be used under the terms of the GNU General Public
** License version 2.0 as published by the Free Software Foundation
** and appearing in the file LICENSE.GPL included in the packaging of
** this file.  Please review the following information to ensure GNU
** General Public Licensing requirements will be met:
** http://www.trolltech.com/products/qt/opensource.html
**
** If you are unsure which license is appropriate for your use, please
** review the following information:
** http://www.trolltech.com/products/qt/licensing.html or contact the
** sales department at sales@trolltech.com.
**
** This file is provided AS IS with NO WARRANTY OF ANY KIND, INCLUDING THE
** WARRANTY OF DESIGN, MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE.
**
****************************************************************************/

#include "downloadpage.h"
#include "downloaderprogress.h"
#include "downloader.h"

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

int DownloadPage::nextId() const
{
    return InstallWizard::uninstallPage;
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
