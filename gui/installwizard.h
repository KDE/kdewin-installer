/****************************************************************************
**
** Copyright (C) 2005-2008 Ralf Habacker. All rights reserved.
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

#include "installwizardpage.h"
#include "settingspage.h"

#include "installerenginegui.h"
extern InstallerEngineGui *engine;

#include <QObject>
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

private:
    int nextIdEndUser() const;
    int nextIdPackageManager() const;

    SettingsPage *_settingsPage;
    int m_lastId;
    static WizardPageType m_titlePage;
};


#endif
