/****************************************************************************
**
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

// define to enable different style
//#define ENABLE_STYLE


#include <QObject>
#include "complexwizard.h"
#include "settingspage.h"

class QCheckBox;
class QLabel;
class QLineEdit;
class QRadioButton;
class QTextEdit;
class QListWidget;
class TitlePage;
class DownloadSettingsPage;
class PathSettingsPage;
class ProxySettingsPage;
class MirrorSettingsPage;
class PackageSelectorPage;
class DependenciesPage;
class InstallPage;
class UninstallPage;
class DownloadPage;
class FinishPage;

extern QListWidget *g_dependenciesList;

class InstallWizard : public ComplexWizard
{
    Q_OBJECT
public:
    InstallWizard(QWidget *parent = 0);

private slots:
    virtual void settingsButtonClicked();
    
private:
    TitlePage *titlePage;
    PathSettingsPage *pathSettingsPage;
    DownloadSettingsPage *downloadSettingsPage;
    ProxySettingsPage *proxySettingsPage;
    MirrorSettingsPage *mirrorSettingsPage;
    PackageSelectorPage *packageSelectorPage;
    DependenciesPage *dependenciesPage;
    DownloadPage *downloadPage;
    InstallPage *installPage;
    UninstallPage *uninstallPage;
    FinishPage *finishPage;
    SettingsPage *settingsPage;

    friend class SettingsPage;
    friend class TitlePage;
    friend class PathSettingsPage;
    friend class DownloadSettingsPage;
    friend class ProxySettingsPage;
    friend class MirrorSettingsPage;
    friend class PackageSelectorPage;
    friend class DependenciesPage;
    friend class DownloadPage;
    friend class InstallPage;
    friend class UninstallPage;
    friend class FinishPage;
};


class InstallWizardPage : public WizardPage
{

public:
    InstallWizardPage(InstallWizard *wizard, SettingsSubPage *s=0);

protected:
    InstallWizard *wizard;
    SettingsSubPage *page;
    QLabel *topLabel;
    QLabel *statusLabel;
};

class TitlePage : public InstallWizardPage
{
public:
    TitlePage(InstallWizard *wizard);

    void resetPage();
    WizardPage *nextPage();
};

class DownloadSettingsPage : public InstallWizardPage
{
    Q_OBJECT

public:
    DownloadSettingsPage(InstallWizard *wizard, SettingsSubPage *s);

    void resetPage();
    WizardPage *nextPage();
    bool isComplete();
};

class PathSettingsPage : public InstallWizardPage
{
    Q_OBJECT

public:
    PathSettingsPage(InstallWizard *wizard, SettingsSubPage *s);

    void resetPage();
    WizardPage *nextPage();
    bool isComplete();
};

class ProxySettingsPage : public InstallWizardPage
{
    Q_OBJECT

public:
    ProxySettingsPage(InstallWizard *wizard, SettingsSubPage *s);

    void resetPage();
    WizardPage *nextPage();
    bool isComplete();
};

class MirrorSettingsPage : public InstallWizardPage
{
    Q_OBJECT

public:
    MirrorSettingsPage(InstallWizard *wizard);

    void resetPage();
    WizardPage *nextPage();
    bool isComplete();
    void initPage();

private:
    QLabel    *mirrorLabel;
    QComboBox *mirrorEdit;
    QStringList *mirrorList;
    void rebuildMirrorList(int index);
private slots:
    void addNewMirror(int index);
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
    virtual void initPage();

public slots:
    void itemClicked(QTreeWidgetItem *item, int column);
    void on_leftTree_itemClicked(QTreeWidgetItem *item, int column);

    void installDirChanged(const QString &dir);
    void slotCompilerTypeChanged(void);

private:
	QTabWidget *packageInfo;
	QTextEdit *categoryInfo;
};

class DependenciesPage: public InstallWizardPage
{
public:
    DependenciesPage(InstallWizard *wizard);

    void resetPage();
    WizardPage *nextPage();

private:
    QListWidget *dependenciesList;
    friend class PackageSelectorPage;
};

class DownloadPage : public InstallWizardPage
{
public:
    DownloadPage(InstallWizard *wizard);

    void resetPage();
    virtual WizardPage *nextPage();
    virtual bool isComplete();
    virtual void reject();
};

class UninstallPage : public InstallWizardPage
{
public:
    UninstallPage(InstallWizard *wizard);

    void resetPage();
    WizardPage *nextPage();
    bool isComplete();

private:
    InstallerProgress *fileList;
};

class InstallPage : public InstallWizardPage
{
public:
    InstallPage(InstallWizard *wizard);

    void resetPage();
    WizardPage *nextPage();
    bool isComplete();

private:
    InstallerProgress *fileList;
};

class FinishPage : public InstallWizardPage
{
public:
    FinishPage(InstallWizard *wizard);

    virtual WizardPage *nextPage() { return NULL; }
    virtual void resetPage();
    virtual bool isLastPage() { return true; }
    virtual bool isComplete();
    virtual void initPage();

private:
    QLabel *bottomLabel;
    QCheckBox *agreeCheckBox;
};

#endif
