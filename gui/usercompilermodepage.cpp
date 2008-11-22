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

#include "database.h"
#include "usercompilermodepage.h"

#include <QDir>
#include <QString>
#include <QFileDialog>

UserCompilerModePage::UserCompilerModePage() : InstallWizardPage(0)
{
    ui.setupUi(this);
    setTitle(windowTitle());
    setSubTitle(statusTip());

    // logical grouping isn't available in the designer yet :-P
    QButtonGroup *groupA = new QButtonGroup(this);
    groupA->addButton(ui.compilerMinGW);
    groupA->addButton(ui.compilerMSVC);

    QButtonGroup *groupB = new QButtonGroup(this);
    groupB->addButton(ui.installModeEndUser);
    groupB->addButton(ui.installModePackageManager);
    connect( groupB,SIGNAL(buttonClicked (int)),this,SLOT(slotModeButtonClicked(int)) );
}

void UserCompilerModePage::initializePage()
{
    Settings &s = Settings::instance();
    setCompilerMode(!s.isPackageManagerMode());

    ui.installModeEndUser->setChecked(!s.isPackageManagerMode() ? Qt::Checked : Qt::Unchecked);
    ui.installModePackageManager->setChecked(s.isPackageManagerMode() ? Qt::Checked : Qt::Unchecked);
    
    if (Database::isAnyPackageInstalled(s.installDir()))
    {
        ui.installModePackageManager->setEnabled(false);
        ui.installModeEndUser->setEnabled(false);
    }
    else
    {
        ui.installModePackageManager->setEnabled(true);
        ui.installModeEndUser->setEnabled(true);
    }

    if (s.isPackageManagerMode())
        ui.installModePackageManager->setChecked(true);
    else
        ui.installModeEndUser->setChecked(true);

    setPixmap(QWizard::WatermarkPixmap, QPixmap());
}

bool UserCompilerModePage::isComplete()
{
    return true;
}

bool UserCompilerModePage::validatePage()
{
    Settings &s = Settings::instance();
    s.setPackageManagerMode(ui.installModePackageManager->isChecked());

    if (ui.compilerMinGW->isChecked())
        s.setCompilerType(Settings::MinGW);
    if (ui.compilerMSVC->isChecked())
        s.setCompilerType(Settings::MSVC);
    return true;
}

void UserCompilerModePage::setCompilerMode(bool EndUserMode)
{
    Settings &s = Settings::instance();
    if (EndUserMode)
    {
        ui.compilerMSVC->setChecked(true);        
        ui.compilerMinGW->setEnabled(false);
        ui.compilerMSVC->setEnabled(false);
    }
    else 
    {
        switch (s.compilerType()) 
        {
            case Settings::MinGW: ui.compilerMinGW->setChecked(true); break;
            case Settings::MSVC: ui.compilerMSVC->setChecked(true); break;
            default: ui.compilerMinGW->setChecked(true); break;
        }
        if (Database::isAnyPackageInstalled(s.installDir()))
        {
            ui.compilerMinGW->setEnabled(false);
            ui.compilerMSVC->setEnabled(false);
        }
        else
        {
            ui.compilerMinGW->setEnabled(true);
            ui.compilerMSVC->setEnabled(true);
        }
    }
}

void UserCompilerModePage::slotModeButtonClicked(int id)
{
    setCompilerMode(ui.installModePackageManager->isChecked() ? 0 : 1);
}
