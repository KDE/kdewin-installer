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
#include <QTextEdit>

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
    Settings &s = Settings::getInstance();

    if (s.isFirstRun() || s.showTitlePage())
    {
        titlePage = new TitlePage(this);
        setFirstPage(titlePage);
        settingsButton->hide();
    }
    else
    {
        engine->readGlobalConfig();
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
    QGroupBox* box = new QGroupBox;

    topLabel = new QLabel(tr("<h1>KDE for Windows Installer</h1>"));
    QLabel* version = new QLabel(tr("<h3>Release " VERSION "</h3>"));
    version->setAlignment(Qt::AlignRight);

    QTextEdit* description = new QTextEdit(tr(
                              "<p>This setup program is used for the initial installation of KDE for Windows application.</p>"
                              "<p>The pages that follow will guide you through the installation."
                              "<br>Please note that by default this installer will install "
                              "<br>only a basic set of applications by default. You can always "
                              "<br>run this program at any time in the future to add, remove, or "
                              "<br>upgrade packages if necessary.</p>"
                          ));
    description->setReadOnly(true);

    //    downloadPackagesRadioButton = new QRadioButton(tr("&download packages"));
    //    downloadAndInstallRadioButton = new QRadioButton(tr("&download and install packages"));
    //    setFocusProxy(downloadPackagesRadioButton);

    QVBoxLayout *layout = new QVBoxLayout;
    layout->addWidget(topLabel);
    layout->addWidget(description);
    layout->addWidget(version);
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
    Settings &s = Settings::getInstance();
    topLabel = new QLabel(tr(
                              "<h1>Select Directories</h1>"
                              "<p>Select the directory where you want to install the KDE packages and where to download packages.</p>"
                          ));

    rootPathLabel = new QLabel(tr("&RootPath:"));
    rootPathEdit = new QLineEdit;
    rootPathLabel->setBuddy(rootPathEdit);
    setFocusProxy(rootPathEdit);
    rootPathEdit->setText(s.installDir());

    rootPathSelect = new QPushButton("...", this);
    connect(rootPathSelect, SIGNAL(pressed()),this, SLOT(selectRootPath()));

    tempPathLabel = new QLabel(tr("&Temporay download path:"));
    tempPathEdit = new QLineEdit;
    tempPathLabel->setBuddy(tempPathEdit);
	tempPathEdit->setText(s.downloadDir());

	tempPathSelect = new QPushButton("...", this);
    connect(tempPathSelect, SIGNAL(pressed()),this, SLOT(selectTempPath()));


    QGridLayout *layout = new QGridLayout;
    layout->addWidget(topLabel, 0, 0, 1, 2);
    layout->setRowMinimumHeight(1, 10);
    layout->addWidget(rootPathLabel, 2, 0);
    layout->addWidget(rootPathEdit, 2, 1);
    layout->addWidget(rootPathSelect, 2, 2);
    layout->addWidget(tempPathLabel, 3, 0);
    layout->addWidget(tempPathEdit, 3, 1);
    layout->addWidget(tempPathSelect, 3, 2);
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

void PathSettingsPage::selectTempPath()
{
    QString fileName = QFileDialog::getExistingDirectory(this,
                       tr("Select Download Directory"),
                       "",
                       QFileDialog::ShowDirsOnly| QFileDialog::DontResolveSymlinks);
    tempPathEdit->setText(fileName);
}

void PathSettingsPage::resetPage()
{}

WizardPage *PathSettingsPage::nextPage()
{
    Settings &s = Settings::getInstance();
	s.setInstallDir(rootPathEdit->text());
	s.setDownloadDir(tempPathEdit->text());
	wizard->proxySettingsPage = new ProxySettingsPage(wizard);
    return wizard->proxySettingsPage;
}

bool PathSettingsPage::isComplete()
{
    return (!rootPathEdit->text().isEmpty());
}

ProxySettingsPage::ProxySettingsPage(InstallWizard *wizard)
        : InstallWizardPage(wizard)
{
    topLabel = new QLabel(tr(
                              "<h1>Select Proxy Settings</h1>"
                              "<p>Choose the proxy type and enter proxy host and port if required.</p>"
                          ));
    proxyHostLabel = new QLabel(tr("Host"));
    proxyPortLabel = new QLabel(tr("Port"));
    proxyPort = new QLineEdit();
    proxyHost = new QLineEdit();
    proxyManual = new QRadioButton();
    proxyIE = new QRadioButton();
    proxyIE->setEnabled(false);
    proxyOff = new QRadioButton();
    proxyManual->setText(QApplication::translate("SettingsDialog", "manual settings", 0, QApplication::UnicodeUTF8));
    proxyIE->setText(QApplication::translate("SettingsDialog", "IE settings", 0, QApplication::UnicodeUTF8));
    proxyOff->setText(QApplication::translate("SettingsDialog", "direct connection", 0, QApplication::UnicodeUTF8));

    Settings &s = Settings::getInstance();
	switch (s.proxyMode()) {
		case 1: proxyIE->setChecked(true); break;
		case 2: proxyManual->setChecked(true); break;
		case 0: 
		default: proxyOff->setChecked(true); break;
	}
	proxyHost->setText(s.proxyHost());
	proxyPort->setText(QString("%1").arg(s.proxyPort()));

	QGridLayout *layout = new QGridLayout;
    layout->setRowMinimumHeight(1, 10);
    layout->addWidget(topLabel, 0, 0, 1, 2);
    layout->addWidget(proxyOff, 2, 0);
    layout->addWidget(proxyIE, 3, 0);
    layout->addWidget(proxyManual, 4, 0);
    layout->addWidget(proxyHostLabel, 4, 2);
    layout->addWidget(proxyHost, 4, 3);
    layout->addWidget(proxyPortLabel, 4, 4);
    layout->addWidget(proxyPort, 4, 5);
    setLayout(layout);
}
void ProxySettingsPage::resetPage()
{}

WizardPage *ProxySettingsPage::nextPage()
{
    Settings &s = Settings::getInstance();
	s.setProxy(proxyOff->isChecked(),proxyIE->isChecked(),proxyManual->isChecked(),
		proxyHost->text(),proxyPort->text());

	engine->setRoot(s.installDir());
    engine->readGlobalConfig();
	engine->downloadPackageLists();
    wizard->packageSelectorPage = new PackageSelectorPage(wizard);
    wizard->settingsButton->show();
    Settings::getInstance().setFirstRun(false);
    return wizard->packageSelectorPage;
}

bool ProxySettingsPage::isComplete()
{
    return true;
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
