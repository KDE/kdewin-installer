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

#include <QtGui>

#include "installwizard.h"

#include "downloader.h"
#include "installer.h"
#include "packagelist.h"

#include <QModelIndex>

extern InstallWizard *wizard;


PackageList *packageList;
Installer *installer;
Downloader *downloader;

int downloadPackageList()
{
	if (!QFile::exists("packages.txt")) {
		// download package list 
		downloader->start("http://sourceforge.net/project/showfiles.php?group_id=23617","packages.html");
	
		// load and parse 
		if (!packageList->readFromHTMLFile("packages.html"))
			return 1; 

		// save into file
		if (!packageList->writeToFile("packages.txt"))
			return 1; 

		// print list 
		packageList->listPackages("Package List");

		// remove temporay files 
		QFile::remove("packages.html");
	}
	else {
		// read list from file 
		if (!packageList->readFromFile("packages.txt"))
			return 1; 

		if ( !QFile::exists("bin\\unzip.exe") ) {
			QStringList files = packageList->getFilesForDownload("unzip");
			files = filterPackageFiles(files,"URL");
		  for (int j = 0; j < files.size(); ++j)
				downloader->start(files.at(j));
		}
		
		// print list 
		packageList->listPackages("Package List");

		if ( !QFile::exists("bin\\unzip.exe") ) {
			qDebug() 	<< "Please unpack " 
							<< packageList->getPackage("unzip")->getFileName(Package::BIN) 
							<< " into the current dir"
			<< "\n then restart installer to download and install additional packages."
			<< "\n\n" <<" ..." << "<package-name> <package-name>";
			return 0;
		}
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

#if 0
#include "spinboxdelegate.h"

SpinBoxDelegate::SpinBoxDelegate(QObject *parent)
    : QItemDelegate(parent)
{
}

QWidget *SpinBoxDelegate::createEditor(QWidget *parent,
    const QStyleOptionViewItem &/* option */,
    const QModelIndex &/* index */) const
{
    QSpinBox *editor = new QSpinBox(parent);
    editor->setMinimum(0);
    editor->setMaximum(100);
    editor->installEventFilter(const_cast<SpinBoxDelegate*>(this));

    return editor;
}

void SpinBoxDelegate::setEditorData(QWidget *editor,
                                        const QModelIndex &index) const
{
    int value = index.model()->data(index, Qt::DisplayRole).toInt();

    QSpinBox *spinBox = static_cast<QSpinBox*>(editor);
    spinBox->setValue(value);
}

void SpinBoxDelegate::setModelData(QWidget *editor, QAbstractItemModel *model,
                                   const QModelIndex &index) const
{
    QSpinBox *spinBox = static_cast<QSpinBox*>(editor);
    spinBox->interpretText();
    int value = spinBox->value();

    model->setData(index, value);
}

void SpinBoxDelegate::updateEditorGeometry(QWidget *editor,
    const QStyleOptionViewItem &option, const QModelIndex &/* index */) const
{
    editor->setGeometry(option.rect);
}
#endif

PackageSelectorPage::PackageSelectorPage(InstallWizard *wizard)
    : InstallWizardPage(wizard)
{
    topLabel = new QLabel(tr("<center><b>Please select the required packages</b></center>"));

    tree  = new QTreeView();
    tree->setWindowTitle(QObject::tr("Dir View"));
		packageList->setModelData(tree);
    //connect(tree,SIGNAL(itemClicked(QTreeWidgetItem *, int)),this,SLOT(itemClicked(QTreeWidgetItem *, int)));
    connect(tree,SIGNAL(clicked(QModelIndex &)),this,SLOT(clicked(QModelIndex &)));



//    SpinBoxDelegate *delegate = new SpinBoxDelegate();
//    tree->setItemDelegate(delegate);

    QVBoxLayout *layout = new QVBoxLayout;
    layout->addWidget(topLabel);
    layout->addWidget(tree);
    setLayout(layout);
}

void PackageSelectorPage::itemClicked(QTreeWidgetItem *item, int column)
{
	qDebug("itemClicked %d",column);
	item->setCheckState(column,item->checkState(0) == Qt::Checked ? Qt::Unchecked : Qt::Checked);
	item->setText(column,"sdsdsd");
	item->setText(column,"sdsdsd");
}

void PackageSelectorPage::clicked(const QModelIndex &index)
{
	qDebug("clicked %d",index);
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
