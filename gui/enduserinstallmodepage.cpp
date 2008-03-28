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
#include "enduserinstallmodepage.h"

EndUserInstallModePage::EndUserInstallModePage() : InstallWizardPage(0)
{
    ui.setupUi(this);
    setTitle(windowTitle());
    setSubTitle(statusTip());
    // logical grouping isn't available in the designer yet :-P
    QButtonGroup *groupA = new QButtonGroup(this);
    groupA->addButton(ui.updateInstallButton);
    groupA->addButton(ui.repairInstallButton);
    groupA->addButton(ui.removeInstallButton);
}

void EndUserInstallModePage::initializePage()
{
}

int EndUserInstallModePage::nextId() const
{
    if (ui.updateInstallButton->isChecked())
        return InstallWizard::endUserUpdatePage;
    else if (ui.repairInstallButton->isChecked())
        return InstallWizard::endUserRepairPage;
    else if (ui.removeInstallButton->isChecked())
        return InstallWizard::endUserRemovePage;
    else 
        return InstallWizard::endUserInstallModePage;
}

bool EndUserInstallModePage::validatePage()
{
    return true;
}


#include "enduserinstallmodepage.moc"

