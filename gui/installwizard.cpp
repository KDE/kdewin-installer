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

//#define USE_ENDUSERPAGES

#include "config.h"
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
//#include "enduserupdatepage.h"
//#include "enduserrepairpage.h"
//#include "enduserremovepage.h"
#include "mirrorsettingspage.h"
#include "packageselectorpage.h"
#include "dependenciespage.h"
#include "downloadpage.h"
#include "uninstallpage.h"
#include "installpage.h"
#include "finishpage.h"

#include <QCheckBox>
#include <QDebug>
#include <QFile>
#include <QLabel>
#include <QLineEdit>
#include <QModelIndex>
#include <QTreeWidgetItem>
#include <QListWidget>
#include <QPushButton>
#include <QVBoxLayout>
#include <QSplitter>
#include <QGridLayout>
#include <QFileDialog>
#include <QInputDialog>
#include <QApplication>
#include <QTextEdit>
#include <QTimer>

InstallerEngineGui *engine;

InstallWizard::InstallWizard(QWidget *parent) : QWizard(parent), m_lastId(0){
    engine = new InstallerEngineGui(this);
    connect(engine, SIGNAL(error(const QString &)), this, SLOT(slotEngineError(const QString &)) );

    // must be first
    setWizardStyle(QWizard::ModernStyle);
    setPixmap(QWizard::LogoPixmap,QPixmap(":/logo.png"));
    // setting a banner limit the installer width 
    // setPixmap(QWizard::BannerPixmap, QPixmap(":/images/banner.png"));
/*
    QPushButton *aboutButton = new QPushButton(tr("About"));
    setButton(QWizard::CustomButton1, aboutButton );
    setOption(QWizard::HaveCustomButton1, true);
    connect(aboutButton, SIGNAL(clicked()), this, SLOT(aboutButtonClicked()) );
*/
/*
    _settingsPage = new SettingsPage(this);
    QPushButton *settingsButton = new QPushButton(tr("Settings"));
    setButton(QWizard::CustomButton2, settingsButton);
    setOption(QWizard::HaveCustomButton2, true);
    connect(settingsButton, SIGNAL(clicked()), this, SLOT(settingsButtonClicked()) );
    settingsButton->hide();
*/
    setOption(QWizard::NoBackButtonOnStartPage,true);

#ifdef HAVE_RETRY_BUTTON
    QPushButton *retryButton = new QPushButton(tr("Retry"));
    setButton(QWizard::CustomButton3, retryButton);
    setOption(QWizard::HaveCustomButton3, true);
    retryButton->hide();
    connect(retryButton, SIGNAL(clicked()), this, SLOT(restart()) );
#endif
/*
    QList<QWizard::WizardButton> layout;
    layout << QWizard::CustomButton1 << QWizard::Stretch << QWizard::CustomButton2 << QWizard::Stretch 
            << QWizard::CancelButton << QWizard::BackButton 
//            << QWizard::CustomButton3 
            << QWizard::NextButton << QWizard::FinishButton;
    setButtonLayout(layout);
*/
    setPage(titlePage, new TitlePage()); 
    setPage(installDirectoryPage, new InstallDirectoryPage); 
    setPage(userCompilerModePage, new UserCompilerModePage); 
    setPage(downloadSettingsPage, new DownloadSettingsPage); 
    setPage(internetSettingsPage, new InternetSettingsPage); 
    setPage(endUserInstallModePage,new EndUserInstallModePage);
//    setPage(endUserUpdatePage,     new EndUserUpdatePage);     
//    setPage(endUserRepairPage,     new EndUserRepairPage);    
//    setPage(endUserRemovePage,     new EndUserRemovePage);    
    setPage(mirrorSettingsPage, new MirrorSettingsPage); 
    setPage(packageSelectorPage, new PackageSelectorPage); 
    setPage(dependenciesPage, new DependenciesPage); 
    setPage(downloadPage, new DownloadPage()); 
    setPage(uninstallPage, new UninstallPage()); 
    setPage(installPage, new InstallPage()); 
    setPage(finishPage, new FinishPage()); 

    QString windowTitle = tr("KDE Installer - Version " VERSION);
    setWindowTitle(windowTitle);
    setSizeGripEnabled(true);
    setWindowFlags(windowFlags()|Qt::WindowMaximizeButtonHint|Qt::WindowMinimizeButtonHint);

    InstallerDialogs &d = InstallerDialogs::instance();
    d.setTitle(windowTitle);
    d.setParent(this);

    setStartId(titlePage);
    connect(this, SIGNAL(currentIdChanged(int)), this, SLOT(slotCurrentIdChanged(int)) );
    readSettings();
}

InstallWizard::~InstallWizard()
{
}

void InstallWizard::aboutButtonClicked()
{
    QMessageBox::information(this,
        tr("KDE-Installer"),
        tr("The KDEWIN Installer is an open source application, "
           "which makes it able to install KDE applications on windows.\n"
           "\nAuthors: \n\n\tRalf Habacker\n\tChristian Ehrlicher\n\tPatrick Spendrin\n"
           "\n\n\nbuild with Qt version " QTVERSION
          ),
          QMessageBox::Ok
    );
}

void InstallWizard::settingsButtonClicked()
{
    _settingsPage->init();
    _settingsPage->exec();
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

// @TODO: The nextId() methods are not called for unknown reason, 
// all id change handling is done in slotCurrentIdChanged()
void InstallWizard::slotCurrentIdChanged(int id)
{
    if (id == downloadPage) {
        button(QWizard::BackButton)->setEnabled(false);
        button(QWizard::NextButton)->setEnabled(false);
        if (!engine->downloadPackages(tree)) {
            reject();
            return;
        }
        button(QWizard::BackButton)->setEnabled(true);
        button(QWizard::NextButton)->setEnabled(true);
        if (Settings::instance().autoNextStep())
            next();
    }
    else if (id == uninstallPage) {
        button(QWizard::BackButton)->setEnabled(false);
        button(QWizard::NextButton)->setEnabled(false);
        if (!engine->removePackages(tree)) {
            reject();
            return;
        }
        button(QWizard::BackButton)->setEnabled(true);
        button(QWizard::NextButton)->setEnabled(true);
        if (Settings::instance().autoNextStep())
            next();
    }
    else if (id == installPage) {
        button(QWizard::BackButton)->setEnabled(false);
        button(QWizard::NextButton)->setEnabled(false);
        if (!engine->installPackages(tree)) {
            reject();
            return;
        }
        button(QWizard::BackButton)->setEnabled(true);
        button(QWizard::NextButton)->setEnabled(true);
        if (Settings::instance().autoNextStep())
            next();
    }
    m_lastId = id;
}

void InstallWizard::slotEngineError(const QString &msg)
{
    QMessageBox::StandardButton result = QMessageBox::critical(
        this,
        tr("Error"),
        msg,
        QMessageBox::Cancel
    );
}

int InstallWizard::nextId() const
{
    switch (currentId()) {
    case titlePage:
        if (Settings::instance().isSkipBasicSettings())
        {
            if (Settings::instance().isDeveloperMode()) 
            {
                if (GlobalConfig::isRemoteConfigAvailable())
                    return packageSelectorPage;
                else
                    return mirrorSettingsPage;
            }
#ifdef USE_ENDUSERPAGES
            else if (Database::isAnyPackageInstalled(Settings::instance().installDir()))
                return endUserInstallModePage;
            else
#endif
                return mirrorSettingsPage;
        }
        else
            return installDirectoryPage;

    case installDirectoryPage: return userCompilerModePage;
    case userCompilerModePage: return downloadSettingsPage;
    case downloadSettingsPage: return internetSettingsPage;
    case internetSettingsPage: 
#ifdef USE_ENDUSERPAGES
        if (!Settings::instance().isDeveloperMode())
            return endUserInstallModePage;
        else
#endif
            return mirrorSettingsPage;
        
    case endUserInstallModePage:
    { 
        EndUserInstallModePage *_page = static_cast<EndUserInstallModePage*>(page(endUserInstallModePage));
        return _page->nextId();
    }
    case mirrorSettingsPage: 
        return packageSelectorPage;
        
//    case endUserUpdatePage: 
//    case endUserRepairPage: 
//    case endUserRemovePage: 
    case packageSelectorPage: 
        return dependenciesPage;
    case dependenciesPage: 
        return downloadPage;
    case downloadPage: 
        return uninstallPage;
    case uninstallPage: 
        return installPage;
    case installPage: 
        return finishPage;
    case finishPage: 
    default:
     return -1;
    }
}

InstallWizardPage::InstallWizardPage(QWidget *parent) : QWizardPage(parent)
{
    statusLabel = new QLabel("", this);
}

void InstallWizardPage::initializePage()
{
}

int InstallWizardPage::nextId() const
{
    return QWizardPage::nextId();
}

bool InstallWizardPage::isComplete()
{
    return QWizardPage::isComplete();
}

void InstallWizardPage::cancel()
{
}

void InstallWizardPage::setStatus(const QString &text)
{
    statusLabel->setText(text);
    QTimer::singleShot(5000, this, SLOT(slotClearStatus()));
}

void InstallWizardPage::slotClearStatus()
{
    statusLabel->setText(QString());
}

void InstallWizardPage::setSettingsButtonVisible(bool mode)
{
#if 0
    if (mode)
        wizard()->button(QWizard::CustomButton2)->show();
    else
        wizard()->button(QWizard::CustomButton2)->hide();
#endif
}


#include "installwizard.moc"
