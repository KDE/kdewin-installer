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

#include "config.h"
#include "debug.h"
#include "postprocesspage.h"

#include <QProcess>

bool PostProcessPage::runCommand(const QString &msg, const QString &app, const QStringList &params)
{
    QFileInfo f(Settings::instance().installDir()+"/bin/" + app + ".exe");
    qDebug() << "checking for app " << app << " - "  <<(f.exists() ? "found" : "not found"); 
    if (!f.exists())
        return false;
    
    ui.listWidget->addItem(msg);
    QCoreApplication::processEvents();
    qDebug() << "running " << app << params; 
    return QProcess::execute( f.absoluteFilePath(), params) == 0 ? true : false;
}

PostProcessPage::PostProcessPage() : InstallWizardPage(0)
{
    ui.setupUi(this);
    setTitle(windowTitle());
    setSubTitle(statusTip());
}

void PostProcessPage::initializePage()
{
}

void PostProcessPage::performAction()
{
    ui.progressBar->setMaximum(3);
    ui.progressBar->setValue(0);
    runCommand("updating mime database","update-mime-database",QStringList() << Settings::instance().installDir()+ "/share/mime");
    ui.progressBar->setValue(1);
    runCommand("updating system configuration database","kbuildsycoca4");
    ui.progressBar->setValue(2);
    runCommand("updating windows start menu entries","kwinstartmenu");
    ui.progressBar->setValue(3);

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

#include "postprocesspage.moc"
