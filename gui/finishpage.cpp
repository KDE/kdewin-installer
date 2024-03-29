/****************************************************************************
**
** Copyright (C) 2008 Ralf Habacker <ralf.habacker@freenet.de> 
** All rights reserved.
**
** This file is part of the KDE installer for windows
**
** SPDX-License-Identifier: GPL-2.0-only
****************************************************************************/

#include "finishpage.h"

#include <QProcess>
#include <QLabel>
#include <QVBoxLayout>

FinishPage::FinishPage()
{
    setTitle(tr("Installation/Update Finished"));
    setSubTitle(tr(" "));
    
    label = new QLabel;
    label2 = new QLabel;

    label->setOpenExternalLinks (true);
    label->setWordWrap(true);
    label2->setOpenExternalLinks (true);
    label2->setWordWrap(true);
    
    QLabel *label3 = new QLabel("");
    runSystemSettingsBox = new QCheckBox("run system settings after exit");
    QVBoxLayout *layout = new QVBoxLayout;
    layout->addWidget(label);
    layout->addWidget(label2);
    layout->addWidget(label3);
    layout->addWidget(runSystemSettingsBox);
    layout->addStretch(1);
    setLayout(layout);
}

void FinishPage::initializePage()
{
    QString funText;
    if (engine->installedPackages() > 0)
    {
        setTitle(tr("Installation/Update Finished"));

        label->setText(tr(
             "<p>Now you should be able to run KDE applications. In the start menu you will find entries for the installed KDE applications.</p>"
            ));

        funText = tr("<p>Have fun using KDE on Windows.</p>");
    }
    else if (engine->removedPackages() > 0)
    {
        setTitle(tr("Package removal Finished"));
        label->setVisible(false);
    }
    else if (engine->installedPackages() == 0)
    {
        label->setText(tr(
             "<p>Thank you for using KDE on Windows.</p>"
            ));
    }

    label2->setText(tr(
         "<p>If you would:<ul>"
         "<li>like to see the KDE on Windows project web site see <a href=\"http://windows.kde.org\">http://windows.kde.org</a></li>"
         "<li>like to get community support for this installer and/or running KDE applications please contact "
            "the <a href=\"http://mail.kde.org/mailman/listinfo/kde-windows\">kde-windows@kde.org</a> mailing list.</li>"
         "<li>like to contribute time and/or money to this project contact the above mentioned list."
            "There are always interesting projects where you can join.</li>"
         "</ul></p>"
         ) + funText + tr(
         "<p> </p>"
         "<p><b>The KDE on Windows team</b></p>"
         "<p>&nbsp;</p>"
         "<p>&nbsp;</p>"
         "<p>Contributors: Ralf Habacker, Christian Ehrlicher, Patrick Spendrin, Hannah von Reth</p>"
         ));

    setFinalPage(true);
    //wizard()->setOption(QWizard::NoCancelButton,true);
    wizard()->button(QWizard::CancelButton)->setEnabled(false);
#ifdef HAVE_RETRY_BUTTON
    wizard()->setOption(QWizard::HaveCustomButton3, false);
    wizard()->button(QWizard::CustomButton3)->show();
#endif
    wizard()->setOption(QWizard::NoBackButtonOnLastPage,true);
    Settings::instance().setFirstRun(false);
    runSystemSettingsBox->setVisible(QFile::exists(engine->root() + "/bin/systemsettings.exe") && engine->canRunHelperApplications());
}

bool FinishPage::isComplete()
{   
    return true;
}

bool FinishPage::validatePage()
{    
    if (runSystemSettingsBox->isChecked())
    {
        engine->runProcessDetached(engine->root() + "/bin/systemsettings.exe");
    }
    return true;
}

