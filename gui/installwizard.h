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

#include <QWizard>
#include "settingspage.h"
#include "installerprogress.h"

class QCheckBox;
class QLabel;
class QLineEdit;
class QRadioButton;
class QListWidget;
class QTextEdit;
class QTreeWidget;
class QTreeWidgetItem;
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

class InstallWizard : public QWizard
{
    Q_OBJECT
public:
    enum {
        titlePage,
        pathSettingsPage,
        downloadSettingsPage,
        proxySettingsPage,
        mirrorSettingsPage,
        packageSelectorPage,
        dependenciesPage,
        downloadPage,
        uninstallPage,
        installPage,
        finishPage,
        settingsPage
     };

    InstallWizard(QWidget *parent = 0);
    ~InstallWizard();
protected:
    void writeSettings();
    void readSettings();

private slots:
    void aboutButtonClicked();
    void settingsButtonClicked();
    void cancelButtonClicked();
    void slotCurrentIdChanged(int id);

private:
    SettingsPage *_settingsPage;
    int m_lastId;
};

class InstallWizardPage : public QWizardPage
{
public:
    InstallWizardPage(SettingsSubPage *s=0);

    virtual void initializePage();
    virtual int nextId() const;
    virtual bool isComplete();
    virtual void cancel();

protected:
    SettingsSubPage *page;
    QLabel *topLabel;
    QLabel *statusLabel;
};

class TitlePage : public InstallWizardPage
{
    Q_OBJECT

public:
    TitlePage();

    void initializePage();
    int nextId() const;
};

class DownloadSettingsPage : public InstallWizardPage
{
    Q_OBJECT

public:
    DownloadSettingsPage(SettingsSubPage *s);

    void initializePage();
    bool isComplete();
    int nextId() const;
    bool validatePage();
};

class PathSettingsPage : public InstallWizardPage
{
    Q_OBJECT

public:
    PathSettingsPage(SettingsSubPage *s);

    void initializePage();
    bool isComplete();
    int nextId() const;
    bool validatePage();
};

class ProxySettingsPage : public InstallWizardPage
{
    Q_OBJECT

public:
    ProxySettingsPage(SettingsSubPage *s);

    void initializePage();
    bool isComplete();
    int nextId() const;
    bool validatePage();
};

class MirrorSettingsPage : public InstallWizardPage
{
    Q_OBJECT

public:
    MirrorSettingsPage(SettingsSubPage *s);

    void initializePage();
    bool isComplete();
    int nextId() const;
    bool validatePage();
};

class PackageSelectorPage : public InstallWizardPage
{
    Q_OBJECT

public:
    PackageSelectorPage();

    void initializePage();
    bool isComplete();
    int nextId() const;
    bool validatePage();

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
    Q_OBJECT

public:
    DependenciesPage();

    void initializePage();
    int nextId() const;
    bool validatePage();

private:
    QListWidget *dependenciesList;
    friend class PackageSelectorPage;
};

class DownloadPage : public InstallWizardPage
{
    Q_OBJECT

public:
    DownloadPage();

    void cancel();
    void initializePage();
    bool isComplete();
    int nextId() const;
    bool validatePage();
};

class UninstallPage : public InstallWizardPage
{
    Q_OBJECT

public:
    UninstallPage();

    void cancel();
    void initializePage();
    bool isComplete();
    int nextId() const;
    bool validatePage();
};

class InstallPage : public InstallWizardPage
{
    Q_OBJECT

public:
    InstallPage();

    void cancel();
    void initializePage();
    bool isComplete();
    int nextId() const;
    bool validatePage();
};

class FinishPage : public InstallWizardPage
{
    Q_OBJECT

public:
    FinishPage();

    void initializePage();
    bool isComplete();

private:
    QLabel *bottomLabel;
    QCheckBox *agreeCheckBox;
};

#endif
