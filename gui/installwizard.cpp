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

#include "config.h"
#include <QCheckBox>
#include <QDebug>
#include <QFile>
#include <QLabel>
#include <QLineEdit>
#include <QModelIndex>
#include <QTreeWidgetItem>
#include <QPushButton>
#include <QVBoxLayout>
#include <QGridLayout>
#include <QFileDialog>
#include <QApplication>

#include "installwizard.h"
#include "downloader.h"
#include "installer.h"
#include "installerprogress.h"
#include "package.h"
#include "packagelist.h"
#include "settings.h"
#include "installerengine.h"
#include "settings.h"
#include "settingspage.h"

extern InstallWizard *wizard;

// must be global
QTreeWidget *tree;

InstallerEngine *engine;

InstallWizard::InstallWizard(QWidget *parent)
        : ComplexWizard(parent)
{
    engine = new InstallerEngine(progressBar,instProgressBar);
    settingsPage = new SettingsPage(this);

    setWindowTitle(tr("KDE Installer"));
    resize(480, 200);
    Settings &s = Settings::getInstance();

    if (s.isFirstRun() || s.showTitlePage())
    {
        titlePage = new TitlePage(this);
        setFirstPage(titlePage);
        settingsButton->hide();
    }
    else
    {
        engine->downloadPackageLists();
        packageSelectorPage = new PackageSelectorPage(this);
        setFirstPage(packageSelectorPage);
    }        
}

void InstallWizard::settingsButtonClicked()
{
    settingsPage->exec();
}


TitlePage::TitlePage(InstallWizard *wizard)
        : InstallWizardPage(wizard)
{
    topLabel = new QLabel(tr(
                              "<h1>KDE for Windows Installer</h1>"
                              "<h3>Release " VERSION "</h3>"
                              "<p>This setup program is used for the initial installation of KDE for Windows application.</p>"
                              "<p>The pages that follow will guide you through the installation."
                              "<br>Please note that by default this installer will install "
                              "<br>only a basic set of applications by default. You can always "
                              "<br>run this program at any time in the future to add, remove, or "
                              "<br>upgrade packages if necessary.</p>"
                          ));

    //    downloadPackagesRadioButton = new QRadioButton(tr("&download packages"));
    //    downloadAndInstallRadioButton = new QRadioButton(tr("&download and install packages"));
    //    setFocusProxy(downloadPackagesRadioButton);

    QVBoxLayout *layout = new QVBoxLayout;
    layout->addWidget(topLabel);
    layout->addSpacing(10);
    //    layout->addWidget(downloadPackagesRadioButton);
    //    layout->addWidget(downloadAndInstallRadioButton);
    layout->addStretch(1);
    setLayout(layout);
}

void TitlePage::resetPage()
{
    //    downloadPackagesRadioButton->setChecked(true);
}

WizardPage *TitlePage::nextPage()
{
    wizard->nextButton->setEnabled(false);
    wizard->nextButton->setEnabled(true);
    wizard->pathSettingsPage = new PathSettingsPage(wizard);
    return wizard->pathSettingsPage;
}

PathSettingsPage::PathSettingsPage(InstallWizard *wizard)
        : InstallWizardPage(wizard)
{
    topLabel = new QLabel(tr(
                              "<h1>Select Root Install Directory</h1>"
                              "<p>Select the directory where you want to install the KDE packages.</p>"
                          ));

    rootPathLabel = new QLabel(tr("&RootPath:"));
    rootPathEdit = new QLineEdit;
    rootPathLabel->setBuddy(rootPathEdit);
    setFocusProxy(rootPathEdit);
    rootPathEdit->setText(engine->root());

    rootPathSelect = new QPushButton("...", this);
    connect(rootPathSelect, SIGNAL(pressed()),this, SLOT(selectRootPath()));

    /*
        tempPathLabel = new QLabel(tr("&Temporay download path:"));
        tempPathEdit = new QLineEdit;
        tempPathLabel->setBuddy(tempPathEdit);
    */
    QGridLayout *layout = new QGridLayout;
    layout->addWidget(topLabel, 0, 0, 1, 2);
    layout->setRowMinimumHeight(1, 10);
    layout->addWidget(rootPathLabel, 2, 0);
    layout->addWidget(rootPathEdit, 2, 1);
    layout->addWidget(rootPathSelect, 2, 2);
    /*
        layout->addWidget(tempPathLabel, 3, 0);
        layout->addWidget(tempPathEdit, 3, 1);
    */
    layout->setRowMinimumHeight(4, 10);
    layout->setRowStretch(6, 1);
    setLayout(layout);
}

void PathSettingsPage::selectRootPath()
{
    QString fileName = QFileDialog::getExistingDirectory(this,
                       tr("Select Root Installation Directory"),
                       "",
                       QFileDialog::ShowDirsOnly| QFileDialog::DontResolveSymlinks);
    rootPathEdit->setText(fileName);
}

void PathSettingsPage::resetPage()
{}

WizardPage *PathSettingsPage::nextPage()
{
    engine->setRoot(rootPathEdit->text());
    engine->downloadPackageLists();
    wizard->packageSelectorPage = new PackageSelectorPage(wizard);
    wizard->settingsButton->show();
    return wizard->packageSelectorPage;
}

bool PathSettingsPage::isComplete()
{
    return (!rootPathEdit->text().isEmpty());
}

PackageSelectorPage::PackageSelectorPage(InstallWizard *wizard)
        : InstallWizardPage(wizard)
{
    topLabel = new QLabel(tr("<center><b>Please select the required packages</b></center>"));

    tree = new QTreeWidget();
    engine->setPageSelectorWidgetData(tree);
    for (int i = 0; i < tree->columnCount(); i++)
        tree->resizeColumnToContents(i);

    connect(tree,SIGNAL(itemClicked(QTreeWidgetItem *, int)),this,SLOT(itemClicked(QTreeWidgetItem *, int)));

    QVBoxLayout *layout = new QVBoxLayout;
    layout->addWidget(topLabel);
    layout->addWidget(tree);
    setLayout(layout);
}

void PackageSelectorPage::itemClicked(QTreeWidgetItem *item, int column)
{
    engine->itemClickedPackageSelectorPage(item,column);
}

void PackageSelectorPage::resetPage()
{}

WizardPage *PackageSelectorPage::nextPage()
{
    wizard->downloadPage = new DownloadPage(wizard);
    return wizard->downloadPage;
}

bool PackageSelectorPage::isComplete()
{
    return true;
}

DownloadPage::DownloadPage(InstallWizard *wizard)
        : InstallWizardPage(wizard)
{
    topLabel = new QLabel(tr("<center><b>Downloading packages</b></center>"));

    QVBoxLayout *layout = new QVBoxLayout;
    layout->addWidget(topLabel);
    setLayout(layout);
}

void DownloadPage::resetPage()
{}

WizardPage *DownloadPage::nextPage()
{
    wizard->installPage = new InstallPage(wizard);
    return wizard->installPage;
/*
    wizard->finishPage = new FinishPage(wizard);
    return wizard->finishPage;
*/
}

bool DownloadPage::isComplete()
{
    wizard->nextButton->setEnabled(false);
    engine->downloadPackages(tree);
    QApplication::instance()->processEvents();
    wizard->nextButton->setEnabled(true);
    // here the download page should be called directly
    return true;
}

void DownloadPage::reject()
{
    engine->stop();
}

InstallPage::InstallPage(InstallWizard *wizard)
        : InstallWizardPage(wizard)
{
    topLabel = new QLabel(tr("<center><b>Installing packages</b></center>"));

    fileList = wizard->instProgressBar;
   
    QVBoxLayout *layout = new QVBoxLayout;
    layout->addWidget(topLabel);
    layout->addWidget(fileList);
    setLayout(layout);
}

void InstallPage::resetPage()
{}

WizardPage *InstallPage::nextPage()
{
    wizard->finishPage = new FinishPage(wizard);
    return wizard->finishPage;
}

bool InstallPage::isComplete()
{
    QApplication::instance()->processEvents();
    engine->installPackages(tree);
    return true;
}

FinishPage::FinishPage(InstallWizard *wizard)
        : InstallWizardPage(wizard)
{
    topLabel = new QLabel(tr("<center><b>Installation finished</b></center>"
                             "<p></p>"
                             "<p>Now you are able to run the installed kde application by .... </p>"
                             "<p>If you have questions or problems using this installer or KDE applications, "
                             "please ask on the <a href=\"http://mail.kde.org/mailman/listinfo/kde-windows\">"
                             "kde-windows@kde.org</a> mailing list. </p>"
                             "<p>If you would like to join this famous project, please contact also the "
                             "<a href=\"http://mail.kde.org/mailman/listinfo/kde-windows\">kde-windows@kde.org</a> mailing list.</p>"
                             "<p></p>"
                             "<p>Have fun using KDE on windows.</p>" 
                             "<p></p>"
                             "<p></p>"
                             "<p>The KDE on windows team</p>"
                             ));

    QVBoxLayout *layout = new QVBoxLayout;
    layout->addWidget(topLabel);
    layout->addStretch(1);
    setLayout(layout);
}

void FinishPage::resetPage()
{}

bool FinishPage::isComplete()
{
    return 1;
}

#include "installwizard.moc"
