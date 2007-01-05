/****************************************************************************
**
** Copyright (C) 2004-2005 Trolltech AS. All rights reserved.
**
** This file is part of the example classes of the Qt Toolkit.
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

#include <QCheckBox>
#include <QDebug>
#include <QFile>
#include <QLabel>
#include <QLineEdit>
#include <QModelIndex>
#include <QTreeWidgetItem>
#include <QPushButton>
#include <QVBoxLayout>

#include "installwizard.h"
#include "downloader.h"
#include "installer.h"
#include "packagelist.h"

extern InstallWizard *wizard;

// FIXME: put this into a class!
PackageList *packageList;
Installer *installer;
Downloader *downloader;

int downloadPackageList()
{
	installer->setRoot("packages");
	if ( !packageList->hasConfig() ) {
    	QByteArray ba;
		// download package list 
		downloader->start("http://sourceforge.net/project/showfiles.php?group_id=23617", ba);

    	// load and parse 
		if (!packageList->readHTMLFromByteArray(ba))
			return 1; 

		// save into file
		if (!packageList->writeToFile())
			return 1; 

		// print list 
		packageList->listPackages("Package List");
	}
	else {
		// read list from file 
		if (!packageList->readFromFile())
			return 1; 

		// print list 
		packageList->listPackages("Package List");
	}
    return 1;
}

InstallWizard::InstallWizard(QWidget *parent)
    : ComplexWizard(parent)
{
    titlePage = new TitlePage(this);
    settingsPage = new SettingsPage(this);
//    packageSelectorPage = new PackageSelectorPage(this);
//    detailsPage = new DetailsPage(this);
    finishPage = new FinishPage(this);

    setFirstPage(titlePage);

    setWindowTitle(tr("KDE Installer"));
    resize(480, 200);

    packageList = new PackageList();
	installer = new Installer(packageList);
	downloader = new Downloader(/*blocking=*/ true,progressBar);
}

TitlePage::TitlePage(InstallWizard *wizard)
    : InstallWizardPage(wizard)
{
    topLabel = new QLabel(tr(
    	"<h1>KDE for Windows Installer</h1>"
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
	downloadPackageList();
	wizard->nextButton->setEnabled(true);
  wizard->packageSelectorPage = new PackageSelectorPage(wizard);
	return wizard->packageSelectorPage;
//	return wizard->settingsPage;
}

SettingsPage::SettingsPage(InstallWizard *wizard)
    : InstallWizardPage(wizard)
{
    topLabel = new QLabel(tr("<center><b>Evaluate Super Product One"
                             "</b></center>"));

    nameLabel = new QLabel(tr("&Name:"));
    nameLineEdit = new QLineEdit;
    nameLabel->setBuddy(nameLineEdit);
    setFocusProxy(nameLineEdit);

    emailLabel = new QLabel(tr("&Email address:"));
    emailLineEdit = new QLineEdit;
    emailLabel->setBuddy(emailLineEdit);

    bottomLabel = new QLabel(tr("Please fill in both fields.\nThis will "
                                "entitle you to a 30-day evaluation."));

    connect(nameLineEdit, SIGNAL(textChanged(QString)),
            this, SIGNAL(completeStateChanged()));
    connect(emailLineEdit, SIGNAL(textChanged(QString)),
            this, SIGNAL(completeStateChanged()));

    QGridLayout *layout = new QGridLayout;
    layout->addWidget(topLabel, 0, 0, 1, 2);
    layout->setRowMinimumHeight(1, 10);
    layout->addWidget(nameLabel, 2, 0);
    layout->addWidget(nameLineEdit, 2, 1);
    layout->addWidget(emailLabel, 3, 0);
    layout->addWidget(emailLineEdit, 3, 1);
    layout->setRowMinimumHeight(4, 10);
    layout->addWidget(bottomLabel, 5, 0, 1, 2);
    layout->setRowStretch(6, 1);
    setLayout(layout);
}

void SettingsPage::resetPage()
{
    nameLineEdit->clear();
    emailLineEdit->clear();
}

WizardPage *SettingsPage::nextPage()
{
    return wizard->packageSelectorPage;
}

bool SettingsPage::isComplete()
{
    return !nameLineEdit->text().isEmpty() && !emailLineEdit->text().isEmpty();
}


PackageSelectorPage::PackageSelectorPage(InstallWizard *wizard)
    : InstallWizardPage(wizard)
{
    topLabel = new QLabel(tr("<center><b>Please select the required packages</b></center>"));

    QTreeWidget *tree = new QTreeWidget();
    packageList->setWidgetData(tree);
    connect(tree,SIGNAL(itemClicked(QTreeWidgetItem *, int)),this,SLOT(itemClicked(QTreeWidgetItem *, int)));

    QVBoxLayout *layout = new QVBoxLayout;
    layout->addWidget(topLabel);
    layout->addWidget(tree);
    setLayout(layout);
}

void WizardPage::itemClicked(QTreeWidgetItem *item, int column)
{
    packageList->itemClicked(item,column);
}

void PackageSelectorPage::resetPage()
{
}

WizardPage *PackageSelectorPage::nextPage()
{
	return wizard->finishPage;
}

bool PackageSelectorPage::isComplete()
{
    return true;
}

/*
DetailsPage::DetailsPage(InstallWizard *wizard)
    : InstallWizardPage(wizard)
{
    topLabel = new QLabel(tr("<center><b>Fill in your details</b></center>"));

    companyLabel = new QLabel(tr("&Company name:"));
    companyLineEdit = new QLineEdit;
    companyLabel->setBuddy(companyLineEdit);
    setFocusProxy(companyLineEdit);

    emailLabel = new QLabel(tr("&Email address:"));
    emailLineEdit = new QLineEdit;
    emailLabel->setBuddy(emailLineEdit);

    postalLabel = new QLabel(tr("&Postal address:"));
    postalLineEdit = new QLineEdit;
    postalLabel->setBuddy(postalLineEdit);

    connect(companyLineEdit, SIGNAL(textChanged(QString)),
            this, SIGNAL(completeStateChanged()));
    connect(emailLineEdit, SIGNAL(textChanged(QString)),
            this, SIGNAL(completeStateChanged()));
    connect(postalLineEdit, SIGNAL(textChanged(QString)),
            this, SIGNAL(completeStateChanged()));

    QGridLayout *layout = new QGridLayout;
    layout->addWidget(topLabel, 0, 0, 1, 2);
    layout->setRowMinimumHeight(1, 10);
    layout->addWidget(companyLabel, 2, 0);
    layout->addWidget(companyLineEdit, 2, 1);
    layout->addWidget(emailLabel, 3, 0);
    layout->addWidget(emailLineEdit, 3, 1);
    layout->addWidget(postalLabel, 4, 0);
    layout->addWidget(postalLineEdit, 4, 1);
    layout->setRowStretch(5, 1);
    setLayout(layout);
}

void DetailsPage::resetPage()
{
    companyLineEdit->clear();
    emailLineEdit->clear();
    postalLineEdit->clear();
}

WizardPage *DetailsPage::nextPage()
{
    return wizard->finishPage;
}

bool DetailsPage::isComplete()
{
    return !companyLineEdit->text().isEmpty()
           && !emailLineEdit->text().isEmpty()
           && !postalLineEdit->text().isEmpty();
}
*/

FinishPage::FinishPage(InstallWizard *wizard)
    : InstallWizardPage(wizard)
{
    topLabel = new QLabel(tr("<center><b>Complete your registration"
                             "</b></center>"));

    bottomLabel = new QLabel;
    bottomLabel->setWordWrap(true);

    agreeCheckBox = new QCheckBox(tr("I agree to the terms and conditions of "
                                     "the license"));
    setFocusProxy(agreeCheckBox);

    connect(agreeCheckBox, SIGNAL(toggled(bool)),
            this, SIGNAL(completeStateChanged()));

    QVBoxLayout *layout = new QVBoxLayout;
    layout->addWidget(topLabel);
    layout->addSpacing(10);
    layout->addWidget(bottomLabel);
    layout->addWidget(agreeCheckBox);
    layout->addStretch(1);
    setLayout(layout);
}

void FinishPage::resetPage()
{
    QString licenseText;

    if (wizard->historyPages().contains(wizard->settingsPage)) {
        licenseText = tr("Evaluation License Agreement: "
                         "You can use this software for 30 days and make one "
                         "back up, but you are not allowed to distribute it.");
    } 
/*
    else if (wizard->historyPages().contains(wizard->detailsPage)) {
        licenseText = tr("First-Time License Agreement: "
                         "You can use this software subject to the license "
                         "you will receive by email.");
    } 
*/
    else {
        licenseText = tr("Upgrade License Agreement: "
                         "This software is licensed under the terms of your "
                         "current license.");
    }
    bottomLabel->setText(licenseText);
    agreeCheckBox->setChecked(false);
}

bool FinishPage::isComplete()
{
    return agreeCheckBox->isChecked();
}
