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
#include <QGridLayout>
#include <QFileDialog>
#include <QApplication>

#include "installwizard.h"
#include "downloader.h"
#include "installer.h"
#include "installerprogress.h"
#include "package.h"
#include "packagelist.h"
#include "configparser.h"

extern InstallWizard *wizard;

// must be global
QTreeWidget *tree;

class InstallerEngine {
	public:
		InstallerEngine(DownloaderProgress *progressBar,InstallerProgress *instProgressBar);
		bool downloadGlobalConfig();
		bool downloadPackageLists(); 
		void setPageSelectorWidgetData(QTreeWidget *tree);
		void itemClickedPackageSelectorPage(QTreeWidgetItem *item, int column);
		bool downloadPackages(QTreeWidget *tree, const QString &category="");
		bool installPackages(QTreeWidget *tree, const QString &category="");
		
		PackageList *packageList() { return m_packageList; }
		Installer *installer() { return m_installer; }

	private:
		PackageList *m_packageList;
		Installer *m_installer;
		Downloader *m_downloader;
		InstallerProgress *m_instProgress;
		ConfigParser *m_configParser;
		
};

InstallerEngine::InstallerEngine(DownloaderProgress *progressBar,InstallerProgress *instProgressBar)
{
	m_downloader = new Downloader(/*blocking=*/ true,progressBar);
	m_packageList = new PackageList(m_downloader);
	m_installer = new Installer(m_packageList,instProgressBar );
	m_configParser = new ConfigParser();
	downloadGlobalConfig();
}

bool InstallerEngine::downloadGlobalConfig()
{
#if 0
 	DownloaderProgress progress(0);
	Downloader download(true,&progress);
	
	qDebug() << "trying to download global configuration file";
	download.start("http://well-known-location-server/kde-installer/config.txt","config.txt");
#endif

	qDebug() << "parsing global configuration file";
	m_configParser->parseFromFile("config.txt");
}


/// download all packagelists, which are available on the configured sites
bool InstallerEngine::downloadPackageLists()
{
	if ( !m_packageList->hasConfig() ) {
    QByteArray ba;
		// download package list 
		m_downloader->start("http://sourceforge.net/project/showfiles.php?group_id=23617", ba);

    	// load and parse 
		if (!m_packageList->readHTMLFromByteArray(ba))
			return 1; 

		// save into file
		if (!m_packageList->writeToFile())
			return 1; 

		// print list 
		m_packageList->listPackages("Package List");
	}
	else {
		// read list from file 
		if (!m_packageList->readFromFile())
			return 1; 

		// print list 
		m_packageList->listPackages("Package List");
	}
    return 1;
}

void InstallerEngine::setPageSelectorWidgetData(QTreeWidget *tree)
{
 	QStringList labels;
 	QList<QTreeWidgetItem *> items;
	QTreeWidgetItem *item;

 	labels 
 	<< "Package"
	<< "Version"
	<< "all"
	<< "bin"
	<< "lib"
	<< "src"
	<< "doc"
	<< "Notes";

 	tree->setColumnCount(8);
 	tree->setHeaderLabels(labels);

	// adding top level items 
 	QList<QTreeWidgetItem *> categoryList;

	qDebug() << "adding categories";

	QList<Site*>::iterator s;
	
	qDebug() << m_configParser->sites()->size();
	for (s = m_configParser->sites()->begin(); s != m_configParser->sites()->end(); s++) {
		qDebug() << (*s)->Name();
		QTreeWidgetItem *category = new QTreeWidgetItem((QTreeWidget*)0, QStringList((*s)->Name()));
		categoryList.append(category);
	}

	tree->insertTopLevelItems(0,categoryList);
	
	// adding sub items 
	QList<Package>::iterator i;
	for (i = m_packageList->packageList()->begin(); i != m_packageList->packageList()->end(); ++i) {
		QStringList data; 
		data << i->Name()
			 << i->Version()
       << ""			 
			 << (i->isInstalled(Package::BIN) ? "-I-" : "")
			 << (i->isInstalled(Package::LIB) ? "-I-" : "")
			 << (i->isInstalled(Package::SRC) ? "-I-" : "")
			 << (i->isInstalled(Package::DOC) ? "-I-" : "")
			;
		QTreeWidgetItem *item = new QTreeWidgetItem(categoryList.at(0), data);
		bool installed = i->isInstalled(Package::BIN)
						|| i->isInstalled(Package::LIB)
						|| i->isInstalled(Package::SRC)
						|| i->isInstalled(Package::DOC);

		if (!installed) {
			item->setCheckState(2, Qt::Unchecked);
			item->setCheckState(3, Qt::Unchecked);
			item->setCheckState(4, Qt::Unchecked);
			item->setCheckState(5, Qt::Unchecked);
			item->setCheckState(6, Qt::Unchecked);
		}

 }
/*
	QStringList data; 
	data.clear(); 
	data << "kdelibs" << "4.1.2";
	item = new QTreeWidgetItem(categoryList.at(5), data);
	data.clear(); 
	data << "kdebase" << "4.1.2";
	item = new QTreeWidgetItem(categoryList.at(5), data);
	data.clear(); 
	data << "kdepim" << "4.1.2";
	item = new QTreeWidgetItem(categoryList.at(5), data);
*/
}

void InstallerEngine::itemClickedPackageSelectorPage(QTreeWidgetItem *item, int column)
{
	if (column < 2)
		return;
	if (column == 2) {
		item->setCheckState(3,item->checkState(column));
		item->setCheckState(4,item->checkState(column));
		item->setCheckState(5,item->checkState(column));
		item->setCheckState(6,item->checkState(column));
	}
}   

bool InstallerEngine::downloadPackages(QTreeWidget *tree, const QString &category)
{
	for (int i = 0; i < tree->topLevelItemCount(); i++) {
		QTreeWidgetItem *item = tree->topLevelItem(i);
		if (category.isEmpty() || item->text(0) == category) {
			for (int j = 0; j < item->childCount(); j++) {
				QTreeWidgetItem *child = item->child(j);
				qDebug("%s %s %d",child->text(0).toAscii().data(),child->text(1).toAscii().data(),child->checkState(2));
				if (child->checkState(2) == Qt::Checked) {
					if (!m_packageList->downloadPackage(child->text(0)))
						qDebug() << "could not download package";
				}
			}
		}
	}
	return true;
}

bool InstallerEngine::installPackages(QTreeWidget *tree,const QString &category)
{
	for (int i = 0; i < tree->topLevelItemCount(); i++) {
		QTreeWidgetItem *item = tree->topLevelItem(i);
		if (category.isEmpty() || item->text(0) == category) {
			for (int j = 0; j < item->childCount(); j++) {
				QTreeWidgetItem *child = item->child(j);
				qDebug("%s %s %d",child->text(0).toAscii().data(),child->text(1).toAscii().data(),child->checkState(2));
				if (child->checkState(2) == Qt::Checked) {
					if (!m_packageList->installPackage(child->text(0)))
						qDebug() << "could not download package";
				}
			}
		}
	}
	return true;
}



InstallerEngine *engine;



InstallWizard::InstallWizard(QWidget *parent)
    : ComplexWizard(parent)
{
    titlePage = new TitlePage(this);
//    packageSelectorPage = new PackageSelectorPage(this);
    setFirstPage(titlePage);

    setWindowTitle(tr("KDE Installer"));
    resize(480, 200);

 		engine = new InstallerEngine(progressBar,instProgressBar);
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
    // FIXME: read rootPath from config
    rootPathEdit->setText(QDir::currentPath());

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
{
}

WizardPage *PathSettingsPage::nextPage()
{
	engine->installer()->setRoot(rootPathEdit->text());
	engine->downloadPackageLists();
  wizard->packageSelectorPage = new PackageSelectorPage(wizard);
	return wizard->packageSelectorPage;
}

bool PathSettingsPage::isComplete()
{
    return 1;//!rootPathEdit->text().isEmpty();
}

PackageSelectorPage::PackageSelectorPage(InstallWizard *wizard)
    : InstallWizardPage(wizard)
{
    topLabel = new QLabel(tr("<center><b>Please select the required packages</b></center>"));

    tree = new QTreeWidget();
    engine->setPageSelectorWidgetData(tree);
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
{
}

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
{
}

WizardPage *DownloadPage::nextPage()
{
/*
    wizard->installPage = new InstallPage(wizard);
    return wizard->installPage;
*/
    wizard->finishPage = new FinishPage(wizard);
    return wizard->finishPage;
}

bool DownloadPage::isComplete()
{
    wizard->nextButton->setEnabled(false);
    engine->downloadPackages(tree);
    topLabel->setText(tr("<center><b>Installing packages</b></center>"));
    QApplication::instance()->processEvents();
    engine->installPackages(tree);
    wizard->nextButton->setEnabled(true);
    // here the finish page should be called directly 
		return 1;
}

InstallPage::InstallPage(InstallWizard *wizard)
    : InstallWizardPage(wizard)
{
    topLabel = new QLabel(tr("<center><b>Installing packages</b></center>"));

    QVBoxLayout *layout = new QVBoxLayout;
    layout->addWidget(topLabel);
    setLayout(layout);
}

void InstallPage::resetPage()
{
}

WizardPage *InstallPage::nextPage()
{
    wizard->finishPage = new FinishPage(wizard);
    return wizard->finishPage;
}

bool InstallPage::isComplete()
{
		return 1;
}

FinishPage::FinishPage(InstallWizard *wizard)
    : InstallWizardPage(wizard)
{
    topLabel = new QLabel(tr("<center><b>Installation finished"
                             "</b></center>"));

    QVBoxLayout *layout = new QVBoxLayout;
    layout->addWidget(topLabel);
    layout->addStretch(1);
    setLayout(layout);
}

void FinishPage::resetPage()
{
}

bool FinishPage::isComplete()
{
    return 1;
}

#include "installwizard.moc"
