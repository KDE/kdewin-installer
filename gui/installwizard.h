/****************************************************************************
**
** Copyright (C) 2004-2005 Trolltech AS. All rights reserved.
** Copyright (C) 2006 Ralf Habacker. All rights reserved.
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

#ifndef InstallWizard_H
#define InstallWizard_H

#include "complexwizard.h"

class QCheckBox;
class QLabel;
class QLineEdit;
class QRadioButton;
class SettingsPage;
class FinishPage;
class PackageSelectorPage;
//class DetailsPage;
class TitlePage;

class InstallWizard : public ComplexWizard
{
public:
    InstallWizard(QWidget *parent = 0);

private:
    TitlePage *titlePage;
    SettingsPage *settingsPage;
    PackageSelectorPage *packageSelectorPage;
//    DetailsPage *detailsPage;
    FinishPage *finishPage;

//    friend class DetailsPage;
    friend class SettingsPage;
    friend class FinishPage;
    friend class PackageSelectorPage;
    friend class TitlePage;
};

class InstallWizardPage : public WizardPage
{
public:
    InstallWizardPage(InstallWizard *wizard)
        : WizardPage(wizard), wizard(wizard) {}

protected:
    InstallWizard *wizard;
};

class TitlePage : public InstallWizardPage
{
public:
    TitlePage(InstallWizard *wizard);

    void resetPage();
    WizardPage *nextPage();

private:
    QLabel *topLabel;
//    QRadioButton *downloadPackagesRadioButton;
//    QRadioButton *downloadAndInstallRadioButton;
};

class SettingsPage : public InstallWizardPage
{
public:
    SettingsPage(InstallWizard *wizard);

    void resetPage();
    WizardPage *nextPage();
    bool isComplete();

private:
    QLabel *topLabel;
    QLabel *nameLabel;
    QLabel *emailLabel;
    QLabel *bottomLabel;
    QLineEdit *nameLineEdit;
    QLineEdit *emailLineEdit;
};


//class QDirModel;
class QStandardItemModel;
class QTreeView;
class QTreeWidgetItem;

class PackageSelectorPage : public InstallWizardPage
{
public:
    PackageSelectorPage(InstallWizard *wizard);

    void resetPage();
    WizardPage *nextPage();
    bool isComplete();

private slots: 
	void itemClicked(QTreeWidgetItem *item, int column);

private:
    QLabel *topLabel;
    QStandardItemModel *model;
//    QDirModel *model;
    QTreeView *tree;

};
/*
class DetailsPage : public InstallWizardPage
{
public:
    DetailsPage(InstallWizard *wizard);

    void resetPage();
    WizardPage *nextPage();
    bool isComplete();

private:
    QLabel *topLabel;
    QLabel *companyLabel;
    QLabel *emailLabel;
    QLabel *postalLabel;
    QLineEdit *companyLineEdit;
    QLineEdit *emailLineEdit;
    QLineEdit *postalLineEdit;
};
*/

class FinishPage : public InstallWizardPage
{
public:
    FinishPage(InstallWizard *wizard);

    void resetPage();
    bool isLastPage() { return true; }
    bool isComplete();

private:
    QLabel *topLabel;
    QLabel *bottomLabel;
    QCheckBox *agreeCheckBox;
};

#endif
