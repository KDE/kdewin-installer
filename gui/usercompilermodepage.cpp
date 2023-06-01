/****************************************************************************
**
** Copyright (C) 2008-2010 Ralf Habacker <ralf.habacker@freenet.de> 
** All rights reserved.
**
** This file is part of the KDE installer for windows
**
** SPDX-License-Identifier: GPL-2.0-only
****************************************************************************/

#include "database.h"
#include "usercompilermodepage.h"

#include <QDir>
#include <QString>
#include <QFileDialog>
#include <QButtonGroup>

UserCompilerModePage::UserCompilerModePage() : InstallWizardPage(0)
{
    ui.setupUi(this);
    setTitle(windowTitle());
    setSubTitle(statusTip());

    if (!supportedCompilers.contains(CompilerTypes::MinGW4))
        ui.compilerMinGW4->setVisible(false);
    if (!supportedCompilers.contains(CompilerTypes::MinGW4_W32))
        ui.compilerMinGW4_W32->setVisible(false);
    if (!supportedCompilers.contains(CompilerTypes::MSVC9))
        ui.compilerMSVC9->setVisible(false);
    if (!supportedCompilers.contains(CompilerTypes::MSVC10))
        ui.compilerMSVC10->setVisible(false);

    if (!supportedCompilers.contains(CompilerTypes::MSVC10_X64))
        ui.compilerMSVCX64->setVisible(false);
    if (!supportedCompilers.contains(CompilerTypes::MinGW4_W64))
        ui.compilerMinGW4_W64->setVisible(false);
    
    ui.compilerMinGW4->setText(supportedCompilers.description(CompilerTypes::MinGW4));
    ui.compilerMinGW4_W32->setText(supportedCompilers.description(CompilerTypes::MinGW4_W32));
    ui.compilerMinGW4_W64->setText(supportedCompilers.description(CompilerTypes::MinGW4_W64));
    ui.compilerMSVC9->setText(supportedCompilers.description(CompilerTypes::MSVC9));
    ui.compilerMSVC10->setText(supportedCompilers.description(CompilerTypes::MSVC10));
    ui.compilerMSVCX64->setText(supportedCompilers.description(CompilerTypes::MSVC10_X64));

    // logical grouping isn't available in the designer yet :-P
    QButtonGroup *groupA = new QButtonGroup(this);
    groupA->addButton(ui.compilerMinGW4);
    groupA->addButton(ui.compilerMinGW4_W32);
    groupA->addButton(ui.compilerMSVC9);
    groupA->addButton(ui.compilerMSVC10);
    groupA->addButton(ui.compilerMinGW4_W64);
    groupA->addButton(ui.compilerMSVCX64);
    
    QButtonGroup *groupB = new QButtonGroup(this);
    groupB->addButton(ui.installModeEndUser);
    groupB->addButton(ui.installModePackageManager);
    connect( groupB,SIGNAL(buttonClicked (int)),this,SLOT(slotModeButtonClicked(int)) );

    // initial setup of packagemanager mode from previous settings, may be overriden later by the user
    engine->setPackageManagerMode(Settings::instance().isPackageManagerMode());
    engine->setCurrentCompiler(Settings::instance().compilerType());
}

void UserCompilerModePage::initializePage()
{
    Settings &s = Settings::instance();
    setCompilerMode(!s.isPackageManagerMode());

    ui.installModeEndUser->setChecked(!s.isPackageManagerMode() ? Qt::Checked : Qt::Unchecked);
    ui.installModePackageManager->setChecked(s.isPackageManagerMode() ? Qt::Checked : Qt::Unchecked);
    
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
    engine->setPackageManagerMode(s.isPackageManagerMode());

    if (ui.compilerMinGW4->isChecked())
        s.setCompilerType(CompilerTypes::MinGW4);
    else if (ui.compilerMinGW4_W32->isChecked())
        s.setCompilerType(CompilerTypes::MinGW4_W32);
    else if (ui.compilerMSVC9->isChecked())
        s.setCompilerType(CompilerTypes::MSVC9);
    else if (ui.compilerMSVC10->isChecked())
        s.setCompilerType(CompilerTypes::MSVC10);
    else if (ui.compilerMSVCX64->isChecked())
        s.setCompilerType(CompilerTypes::MSVC10_X64);
    else if (ui.compilerMinGW4_W64->isChecked())
        s.setCompilerType(CompilerTypes::MinGW4_W64);
    else
        s.setCompilerType(CompilerTypes::MSVC10);
    engine->setCurrentCompiler(s.compilerType());
    return true;
}

void UserCompilerModePage::setCompilerMode(bool EndUserMode)
{
    Settings &s = Settings::instance();
    switch (s.compilerType()) 
    {
        case CompilerTypes::MinGW4: ui.compilerMinGW4->setChecked(true); break;
        case CompilerTypes::MinGW4_W32: ui.compilerMinGW4_W32->setChecked(true); break;
        case CompilerTypes::MinGW4_W64: ui.compilerMinGW4_W64->setChecked(true); break;
        case CompilerTypes::MSVC9: ui.compilerMSVC9->setChecked(true); break;
        case CompilerTypes::MSVC10: ui.compilerMSVC10->setChecked(true); break;
        case CompilerTypes::MSVC10_X64: ui.compilerMSVCX64->setChecked(true); break;
        default: ui.compilerMSVC10->setChecked(true); break;
    }
    bool state = !Database::isAnyPackageInstalled(s.installDir());
    ui.compilerMinGW4->setEnabled(state);
    ui.compilerMinGW4_W32->setEnabled(state);
    ui.compilerMSVC9->setEnabled(state);
    ui.compilerMSVC10->setEnabled(state);
    // display is controlled by setVisible
    ui.compilerMSVCX64->setEnabled(state);
    ui.compilerMinGW4_W64->setEnabled(state);
}

void UserCompilerModePage::slotModeButtonClicked(int id)
{
    setCompilerMode(ui.installModePackageManager->isChecked() ? 0 : 1);
}
