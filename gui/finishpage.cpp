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

#include "finishpage.h"

FinishPage::FinishPage()
{
    setTitle(tr("Installation finished"));
    setSubTitle(tr(" "));

    QLabel* label = new QLabel(tr(
         "<p>Now you should be able to run kde applications. "
         "Please open an explorer window and navigate to the bin folder of the kde installation root. "
         "There you will find several applications which can be started by a simple click on the executable.</p>"
         "<p>In further versions of this installer it will be also possible "
         "to start kde applications from the windows start menu.</p>"
         "<p>If you <ul>"
         "<li>like to see the KDE on windows project web site see <a href=\"http://windows.kde.org\">http://windows.kde.org</a></li>"
         "<li>like to get community support for this installer and/or running kde applications please contact "
            "the <a href=\"http://mail.kde.org/mailman/listinfo/kde-windows\">kde-windows@kde.org</a> mailing list.</li>"
         "<li>like to contribute time and/or money to this project contact also the above mentioned list."
            "There are always interesting projects where you can join.</li>"
         "</ul></p>"
         "<p>Have fun using KDE on windows.</p>" 
         "<p> </p>"
         "<p><b>The KDE on Windows team</b></p>"
         "<p>&nbsp;</p>"
         "<p>&nbsp;</p>"
         "<p>This software is designed to provide powerfull and flexible network based install systems using a modern gui toolkit. "
         "If you need assistance or have specific needs for such a services, please contact <a href=\"mailto:ralf@habacker.de\">Ralf Habacker</a></p>"
         ));

    label->setOpenExternalLinks (true);
    label->setWordWrap(true);
    
    QVBoxLayout *layout = new QVBoxLayout;
    layout->addWidget(label);
    layout->addStretch(1);
    setLayout(layout);
}

void FinishPage::initializePage()
{
    setFinalPage(true);
    //wizard()->setOption(QWizard::NoCancelButton,true);
    wizard()->button(QWizard::CancelButton)->setEnabled(false);
#ifdef HAVE_RETRY_BUTTON
    wizard()->setOption(QWizard::HaveCustomButton3, false);
    wizard()->button(QWizard::CustomButton3)->show();
#endif
    wizard()->setOption(QWizard::NoBackButtonOnLastPage,true);
}

bool FinishPage::isComplete()
{
    return true;
}

#include "finishpage.moc"
