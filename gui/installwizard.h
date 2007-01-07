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

#include <QObject>
#include "complexwizard.h"

class QCheckBox;
class QLabel;
class QLineEdit;
class QRadioButton;
class TitlePage;
class PathSettingsPage;
class PackageSelectorPage;
class InstallPage;
class DownloadPage;
class FinishPage;

class InstallWizard : public ComplexWizard
{
public:
    InstallWizard(QWidget *parent = 0);

private:
    TitlePage *titlePage;
    PathSettingsPage *pathSettingsPage;
    PackageSelectorPage *packageSelectorPage;
    DownloadPage *downloadPage;
    InstallPage *installPage;
    FinishPage *finishPage;

    friend class TitlePage;
    friend class PathSettingsPage;
    friend class PackageSelectorPage;
    friend class DownloadPage;
    friend class InstallPage;
    friend class FinishPage;
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

class PathSettingsPage : public InstallWizardPage
{
    Q_OBJECT

public:
    PathSettingsPage(InstallWizard *wizard);

    void resetPage();
    WizardPage *nextPage();
    bool isComplete();

public slots:
		void selectRootPath(void);

private:
    QLabel    *topLabel;
    QLabel    *rootPathLabel;
    QLineEdit *rootPathEdit;
    QPushButton *rootPathSelect;
/*
    QLabel    *tempPathLabel;
    QLineEdit *tempPathEdit;
*/
};

class QTreeWidget;
class QTreeWidgetItem;

class PackageSelectorPage : public InstallWizardPage
{
    Q_OBJECT

public:
    PackageSelectorPage(InstallWizard *wizard);

    void resetPage();
    WizardPage *nextPage();
    bool isComplete();

public slots: 
    void itemClicked(QTreeWidgetItem *item, int column);

private:
    QLabel *topLabel;
};

class DownloadPage : public InstallWizardPage
{
public:
    DownloadPage(InstallWizard *wizard);

    void resetPage();
    WizardPage *nextPage();
    bool isComplete();

private:
    QLabel *topLabel;
};

class InstallPage : public InstallWizardPage
{
public:
    InstallPage(InstallWizard *wizard);

    void resetPage();
    WizardPage *nextPage();
    bool isComplete();

private:
    QLabel *topLabel;
};

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
