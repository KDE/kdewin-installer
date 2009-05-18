/****************************************************************************
**
** Copyright (C) 2006-2008 Ralf Habacker <ralf.habacker@freenet.de>
**  All rights reserved.
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

#define ENABLE_ENDUSER_PAGES

#include "config.h"
#include "debug.h"
#include "installwizard.h"
#include "installerenginegui.h"
#include "installerdialogs.h"
#include "database.h"
#include "downloader.h"
#include "downloaderprogress.h"
#include "uninstaller.h"
#include "unpacker.h"
#include "installerprogress.h"
#include "settings.h"
#include "settingspage.h"
#include "titlepage.h"
#include "usercompilermodepage.h"
#include "installdirectorypage.h"
#include "internetsettingspage.h"
#include "downloadsettingspage.h"
#include "enduserinstallmodepage.h"
#include "enduserpackageselectorpage.h"
//#include "enduserupdatepage.h"
//#include "enduserrepairpage.h"
//#include "enduserremovepage.h"
#include "mirrorsettingspage.h"
#include "releaseselectionpage.h"
#include "packageselectorpage.h"
#include "postprocesspage.h"
#include "dependenciespage.h"
#include "downloadpage.h"
#include "uninstallpage.h"
#include "installpage.h"
#include "finishpage.h"

#include <QtCore/QTimer>

InstallerEngineGui *engine;

InstallWizard::WizardPageType InstallWizard::m_titlePage = WizardPageType::titlePage;

InstallWizard::InstallWizard(QWidget *parent) : QWizard(parent), m_lastId(0){
    engine = new InstallerEngineGui(this);
    // default settings from stored values, they may be overrided by the  wizard pages 
    engine->setRoot(Settings::instance().installDir());
    engine->setConfigURL(Settings::instance().mirrorWithReleasePath());
    connect(engine, SIGNAL(error(const QString &)), this, SLOT(slotEngineError(const QString &)) );

    // must be first
    setWizardStyle(QWizard::ModernStyle);
    setPixmap(QWizard::LogoPixmap,QPixmap(":/logo.png"));

    setOption(QWizard::NoBackButtonOnStartPage,true);

#ifdef HAVE_RETRY_BUTTON
    QPushButton *retryButton = new QPushButton(tr("Retry"));
    setButton(QWizard::CustomButton3, retryButton);
    setOption(QWizard::HaveCustomButton3, true);
    retryButton->hide();
    connect(retryButton, SIGNAL(clicked()), this, SLOT(restart()) );
#endif
    TitlePage *titlePageP = new TitlePage();
    
    QString windowTitle = titlePageP->windowTitle() + QLatin1String(" - Version " VERSION_PATCH);
    setWindowTitle(windowTitle);

    setPage(titlePage, titlePageP); 
    setPage(installDirectoryPage, new InstallDirectoryPage); 
    setPage(userCompilerModePage, new UserCompilerModePage); 
    setPage(downloadSettingsPage, new DownloadSettingsPage); 
    setPage(internetSettingsPage, new InternetSettingsPage); 
    setPage(endUserInstallModePage,new EndUserInstallModePage);
    setPage(endUserPackageSelectorPage, new EndUserPackageSelectorPage); 
//    setPage(endUserUpdatePage,     new EndUserUpdatePage);     
//    setPage(endUserRepairPage,     new EndUserRepairPage);    
//    setPage(endUserRemovePage,     new EndUserRemovePage);    
    setPage(mirrorSettingsPage, new MirrorSettingsPage); 
    setPage(releaseSelectionPage, new ReleaseSelectionPage); 
    setPage(packageSelectorPage, new PackageSelectorPage); 
    setPage(dependenciesPage, new DependenciesPage); 
    setPage(downloadPage, new DownloadPage()); 
    setPage(uninstallPage, new UninstallPage()); 
    setPage(installPage, new InstallPage()); 
    setPage(postProcessPage, new PostProcessPage); 
    setPage(finishPage, new FinishPage()); 

    setSizeGripEnabled(true);
    setWindowFlags(windowFlags()|Qt::WindowMaximizeButtonHint|Qt::WindowMinimizeButtonHint);

    InstallerDialogs &d = InstallerDialogs::instance();
    d.setTitle(windowTitle);
    d.setParent(this);

    setStartId(m_titlePage);
    connect(this, SIGNAL(currentIdChanged(int)), this, SLOT(slotCurrentIdChanged(int)) );
    readSettings();
}

InstallWizard::~InstallWizard()
{
}

void InstallWizard::setTitlePage(WizardPageType pageType)
{
    m_titlePage = pageType;
}

void InstallWizard::reject()
{
    //there is no page set for this signal 
    InstallWizardPage *aPage = static_cast<InstallWizardPage*>(currentPage());
    if (aPage)
        aPage->cancel();
    else 
        qDebug() << "no page for cancel";
    // let installer engine handle this case 

    engine->stop();
    writeSettings();
    QWizard::reject();
}

void InstallWizard::writeSettings()
{
    Settings &settings = Settings::instance();

    settings.beginGroup("Geometry");
    settings.setValue("normalGeometry", normalGeometry());
    settings.setValue("maximized", isMaximized());
    settings.endGroup();
}

void InstallWizard::readSettings()
{
    Settings &settings = Settings::instance();

    settings.beginGroup("Geometry");
    setGeometry(settings.value("normalGeometry", QRect(200,200, 800, 500)).toRect());
    if (settings.value("maximized", false).toBool()) {
      setWindowState(Qt::WindowMaximized);
    }

    settings.endGroup();
}

// this method is called after a page changed
// the new page is visible
void InstallWizard::slotCurrentIdChanged(int id)
{
    InstallWizardPage *currentPage = static_cast<InstallWizardPage*>(page(id));
    if (currentPage) {
        currentPage->performAction(); 
        m_lastId = id;
    }
}

void InstallWizard::slotEngineError(const QString &msg)
{
    QMessageBox::StandardButton result = QMessageBox::critical(
        this,
        tr("Error"),
        msg,
        QMessageBox::Cancel
    );
//    engine->setErrorAction(result = QMessageBox::Cancel ?  Cancel : Retry);
}

bool InstallWizard::skipSettings()
{
    return Settings::instance().isSkipBasicSettings()
        && !Settings::instance().installDir().isEmpty()
        && !Settings::instance().downloadDir().isEmpty()
        && (InstallerEngine::installMode() == InstallerEngine::localInstall || Settings::instance().mirrorWithReleasePath().isValid());
}

int InstallWizard::nextIdEndUser() const
{
    switch (currentId()) {
    case titlePage:
        if (InstallerEngine::installMode() == InstallerEngine::localInstall 
            ||  InstallerEngine::installMode() == InstallerEngine::downloadOnly)
        {
            if (skipSettings())
                return downloadSettingsPage;
            else 
                return installDirectoryPage;
        }
    
        if (skipSettings())
        {
            if (Database::isAnyPackageInstalled(Settings::instance().installDir()))
#if 1
                return endUserInstallModePage;
#else
                return endUserPackageSelectorPage;
#endif
            else
                return mirrorSettingsPage;
        }
        else
            return installDirectoryPage;

    case installDirectoryPage: return userCompilerModePage;
    case userCompilerModePage: return downloadSettingsPage;
    case downloadSettingsPage: 
        if (InstallerEngine::installMode() == InstallerEngine::downloadOnly)
            return endUserPackageSelectorPage;
        if (InstallerEngine::installMode() == InstallerEngine::localInstall)
            return endUserInstallModePage;
        else
            return internetSettingsPage;

    case internetSettingsPage: 
        return mirrorSettingsPage;
        
    case mirrorSettingsPage: 
        if (InstallerEngine::installMode() == InstallerEngine::localInstall)
            return endUserPackageSelectorPage;
        else
            return releaseSelectionPage;
    case releaseSelectionPage: return endUserPackageSelectorPage;
//    case endUserUpdatePage:        return dependenciesPage;
//    case endUserRepairPage:        return uninstallPage;
//    case endUserRemovePage:        return uninstallPage;

    case endUserInstallModePage:
    { 
        EndUserInstallModePage *_page = static_cast<EndUserInstallModePage*>(page(endUserInstallModePage));
        switch(_page->selectedInstallMode()) {
            case EndUserInstallModePage::Update:
                if (skipSettings() || InstallerEngine::installMode() == InstallerEngine::localInstall)
                    return endUserPackageSelectorPage;
                else
                    return mirrorSettingsPage;
            case EndUserInstallModePage::Remove:
                return uninstallPage;
            case EndUserInstallModePage::Repair:
                return endUserRepairPage;
            default: 
                return endUserInstallModePage;
        }
    }

    case endUserPackageSelectorPage: return dependenciesPage;
    case dependenciesPage:           return downloadPage;
    case downloadPage:               
        if (InstallerEngine::installMode() == InstallerEngine::downloadOnly)
            return finishPage;
        else
            return uninstallPage;
    case uninstallPage:              return installPage;
    case installPage:                
        if (engine->installedPackages() > 0 || engine->removedPackages() > 0)
            return postProcessPage;
        else
            return finishPage;
            
    case postProcessPage:            return finishPage;
    case finishPage: 
    default:
     return -1;
    }
}


int InstallWizard::nextIdPackageManager() const
{
    switch (currentId()) {
    case titlePage:
        if (InstallerEngine::installMode() == InstallerEngine::localInstall 
            ||  InstallerEngine::installMode() == InstallerEngine::downloadOnly)
        {
            if (skipSettings())
                return downloadSettingsPage;
            else 
                return installDirectoryPage;
        }
        if (skipSettings())
        {
            return packageSelectorPage;
        }
        else
            return installDirectoryPage;

    case installDirectoryPage: return userCompilerModePage;
    case userCompilerModePage: 
        if (InstallerEngine::installMode() == InstallerEngine::localInstall)
            return packageSelectorPage;
        else
            return downloadSettingsPage;
    case downloadSettingsPage: 
        if (skipSettings() 
                && (InstallerEngine::installMode() == InstallerEngine::downloadOnly 
                    || InstallerEngine::installMode() == InstallerEngine::localInstall))
            return packageSelectorPage;
        return internetSettingsPage;
        
    case internetSettingsPage: return mirrorSettingsPage;
    case mirrorSettingsPage:           
        if (InstallerEngine::installMode() == InstallerEngine::localInstall)
            return endUserPackageSelectorPage;
        else
            return releaseSelectionPage;
    case releaseSelectionPage: return packageSelectorPage;
    case packageSelectorPage:  return dependenciesPage;
    case dependenciesPage:     return downloadPage;
    case downloadPage:
        if (InstallerEngine::installMode() == InstallerEngine::downloadOnly)
            return finishPage;
        else
            return uninstallPage;
    case uninstallPage:        return installPage;
    case installPage:        
        if (engine->installedPackages() > 0 || engine->removedPackages() > 0)
            return postProcessPage;
        else
            return finishPage;

    case postProcessPage:      return finishPage;
    case finishPage: 
    default:
     return -1;
    }
}

int InstallWizard::nextId() const
{
#ifdef ENABLE_ENDUSER_PAGES
    if (!Settings::instance().isPackageManagerMode())
        return nextIdEndUser();
    else
#endif
        return nextIdPackageManager();
}
