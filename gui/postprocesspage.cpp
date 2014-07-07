/****************************************************************************
**
** Copyright (C) 2008-2011 Ralf Habacker <ralf.habacker@freenet.de> 
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
}

void PostProcessPage::performAction()
{
    if (engine->installedPackages() > 0 && !SelfInstaller::instance().isInstalled())
		SelfInstaller::instance().install();

    Q_ASSERT(connect(&m_postProcessing,SIGNAL(numberOfCommands(int)),ui.progressBar,SLOT(setMaximum(int))));
    Q_ASSERT(connect(&m_postProcessing,SIGNAL(finished()),this,SLOT(postProcessingEnd())));
    Q_ASSERT(connect(&m_postProcessing,SIGNAL(commandStarted(int)),ui.progressBar,SLOT(setValue(int))));
    Q_ASSERT(connect(&m_postProcessing,SIGNAL(commandStarted(const QString &)),this,SLOT(addItem(const QString &))));
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
