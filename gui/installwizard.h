/****************************************************************************
**
** Copyright (C) 2005-2008 Ralf Habacker. All rights reserved.
**
** This file is part of the KDE installer for windows
**
** SPDX-License-Identifier: GPL-2.0-only
****************************************************************************/

#ifndef InstallWizard_H
#define InstallWizard_H

#include "installwizardpage.h"
#include "settingspage.h"

#include "installerenginegui.h"
extern InstallerEngineGui *engine;

#include <QObject>
#include <QTextEdit>
#include <QWizard>

class QLabel;

class InstallWizard : public QWizard
{
    Q_OBJECT
public:
    typedef enum {
        titlePage,
        userCompilerModePage,
        endUserPackageSelectorPage,
        endUserInstallModePage,
        endUserUpdatePage,
        endUserRepairPage,
        endUserRemovePage,
        installDirectoryPage,
        downloadSettingsPage,
        internetSettingsPage,
        mirrorSettingsPage,
        releaseSelectionPage,
        packageSelectorPage,
        postProcessPage,
        dependenciesPage,
        downloadPage,
        uninstallPage,
        installPage,
        finishPage,
        settingsPage
     } WizardPageType;

    InstallWizard(QWidget *parent = 0);
    virtual ~InstallWizard();

    /// check if settings should really skipped - this depends on present install dir, downlod dir full mirror release 
    static bool skipSettings();

    /// 
    static void setTitlePage(WizardPageType pageType);

protected:
    void writeSettings();
    void readSettings();
    virtual int nextId() const;

private slots:
    virtual void reject();
    void slotCurrentIdChanged(int id);
    void slotEngineError(const QString &);
    void customButtonClicked();

private:
    int nextIdEndUser() const;
    int nextIdPackageManager() const;

    SettingsPage *_settingsPage;
    int m_lastId;
    static WizardPageType m_titlePage;
    QTextEdit *m_log;
};


#endif
