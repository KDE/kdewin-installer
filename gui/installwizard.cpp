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
#include "configparser.h"
#include "settings.h"

extern InstallWizard *wizard;

// must be global
QTreeWidget *tree;

#if 1
// FIXME: move into shared directory
class InstallerEngine
{
public:
    InstallerEngine(DownloaderProgress *progressBar,InstallerProgress *instProgressBar);
    bool downloadGlobalConfig();
    bool downloadPackageLists();
    PackageList *getPackageListByName(const QString &name);
    void setPageSelectorWidgetData(QTreeWidget *tree);
    void itemClickedPackageSelectorPage(QTreeWidgetItem *item, int column);
    bool downloadPackages(QTreeWidget *tree, const QString &category="");
    bool installPackages(QTreeWidget *tree, const QString &category="");
    
    void setRoot(const QString &root);
    QString root() const;

    PackageList *packageList()
    {
        return m_packageList;
    }

    Installer *installer()
    {
        return m_installer;
    }
    const Settings &settings() const
    {
        return m_settings;
    }

private:
    QList <PackageList*> m_packageListList;
    QList <Installer*> m_installerList;
    PackageList *m_packageList;  // currenty used packagelist
    Installer *m_installer;    // currenty used installer
    Downloader *m_downloader;
    InstallerProgress *m_instProgress;
    ConfigParser *m_configParser;
    InstallerProgress *m_instProgressBar;
    Settings m_settings;
};

InstallerEngine::InstallerEngine(DownloaderProgress *progressBar,InstallerProgress *instProgressBar)
{
    m_downloader = new Downloader(/*blocking=*/ true,progressBar);
    m_instProgressBar = instProgressBar;
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
    return m_configParser->parseFromFile("config.txt");
}

PackageList *InstallerEngine::getPackageListByName(const QString &name)
{
    QList <PackageList *>::iterator k;
    for (k = m_packageListList.begin(); k != m_packageListList.end(); ++k)
    {
        if ((*k)->Name() == name)
            return (*k);
    }
    return 0;
}

/// download all packagelists, which are available on the configured sites
bool InstallerEngine::downloadPackageLists()
{
    // FIXME: m_packageListList has to be synced with sites, may be packageList contains a name used in view
    QList<Site*>::iterator s;
    for (s = m_configParser->sites()->begin(); s != m_configParser->sites()->end(); s++)
    {
        qDebug() << "download package file list for site: " << (*s)->Name();
        PackageList *packageList = new PackageList(m_downloader);
        packageList->setConfigFileName("packages-" + (*s)->Name() + ".txt");
        packageList->setName((*s)->Name());
        m_packageList = packageList;
        m_packageListList.append(packageList);
        Installer *installer = new Installer(packageList,m_instProgressBar );

        // FIXME:: hardcoded name, better to use an option in the config file ?
        if ((*s)->Name() == "gnuwin32")
        {
            installer->setType(Installer::GNUWIN32);
            // FIXME: add additional option in config.txt for mirrors
            packageList->setBaseURL("http://heanet.dl.sourceforge.net/sourceforge/gnuwin32/");
        }
        else
            packageList->setBaseURL((*s)->URL());
        installer->setRoot(m_settings.value("rootdir").toString());
        m_installerList.append(installer);
        m_installer = installer;

        if ( !packageList->hasConfig() )
        {
            // download package list
            qDebug() << (*s)->URL();
#ifdef DEBUG
            QFileInfo tmpFile(installer->Root() + "/packages-"+(*s)->Name()+".html");
            if (!tmpFile.exists())
                m_downloader->start((*s)->URL(), ba));

            // load and parse
            if (!packageList->readHTMLFromFile(tmpFile.absoluteFilePath(),(*s)->Type() == Site::ApacheModIndex ? PackageList::ApacheModIndex : PackageList::SourceForge ))
#else            
            QByteArray ba;
            m_downloader->start((*s)->URL(), ba);
            if (!packageList->readHTMLFromByteArray(ba,(*s)->Type() == Site::ApacheModIndex ? PackageList::ApacheModIndex : PackageList::SourceForge ))
#endif
            {
                qDebug() << "error reading package list from download html file";
                continue;
            }

            // save into file
            if (!packageList->writeToFile())
            {
                qDebug() << "error writing package list to file";
                continue;
            }

        }
        else
        {
            // read list from file
            if (!packageList->readFromFile())
            {
                qDebug() << "error reading package list from file";
                continue;
            }
        }
    }
    return true;
}

void InstallerEngine::setPageSelectorWidgetData(QTreeWidget *tree)
{
    QStringList labels;
    QList<QTreeWidgetItem *> items;
    // QTreeWidgetItem *item;

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
    // see http://lists.trolltech.com/qt-interest/2006-06/thread00441-0.html
    // and Task Tracker Entry 106731
    //tree->setAlignment(Center);

    // adding top level items
    QList<QTreeWidgetItem *> categoryList;

    qDebug() << "adding categories size:" << m_configParser->sites()->size();

    QList<Site*>::iterator s;
    for (s = m_configParser->sites()->begin(); s != m_configParser->sites()->end(); s++)
    {
        qDebug() << (*s)->Name();
        QTreeWidgetItem *category = new QTreeWidgetItem((QTreeWidget*)0, QStringList((*s)->Name()));
        categoryList.append(category);
    }

    tree->insertTopLevelItems(0,categoryList);

    QList <PackageList *>::iterator k;
    // FIXME: m_packageListList has to be synced with sites
    int t=0;
    for (k = m_packageListList.begin(); k != m_packageListList.end(); ++k)
    {
        // adding sub items
        QList<Package>::iterator i;
        for (i = (*k)->packageList()->begin(); i != (*k)->packageList()->end(); ++i)
        {
            QStringList data;
            data << i->name()
            << i->version()
            << ""
            << (i->isInstalled(Package::BIN) ? "-I-" : "")
            << (i->isInstalled(Package::LIB) ? "-I-" : "")
            << (i->isInstalled(Package::SRC) ? "-I-" : "")
            << (i->isInstalled(Package::DOC) ? "-I-" : "")
            ;
            QTreeWidgetItem *item = new QTreeWidgetItem(categoryList.at(t), data);
            bool installed = i->isInstalled(Package::BIN)
                             || i->isInstalled(Package::LIB)
                             || i->isInstalled(Package::SRC)
                             || i->isInstalled(Package::DOC);

            if (!installed)
            {
                item->setCheckState(2, Qt::Unchecked);
                item->setCheckState(3, Qt::Unchecked);
                item->setCheckState(4, Qt::Unchecked);
                item->setCheckState(5, Qt::Unchecked);
                item->setCheckState(6, Qt::Unchecked);
            }
        }
        t++;
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
    if (column == 2)
    {
        item->setCheckState(3,item->checkState(column));
        item->setCheckState(4,item->checkState(column));
        item->setCheckState(5,item->checkState(column));
        item->setCheckState(6,item->checkState(column));
    }
}

bool InstallerEngine::downloadPackages(QTreeWidget *tree, const QString &category)
{
    for (int i = 0; i < tree->topLevelItemCount(); i++)
    {
        QTreeWidgetItem *item = tree->topLevelItem(i);
        qDebug() << __FUNCTION__ << " " << item->text(0);
        if (category.isEmpty() || item->text(0) == category)
        {
            PackageList *packageList = getPackageListByName(item->text(0));
            if (!packageList)
            {
                qDebug() << __FUNCTION__ << " packagelist for " << item->text(0) << " not found";
                continue;
            }
            for (int j = 0; j < item->childCount(); j++)
            {
                QTreeWidgetItem *child = item->child(j);
                qDebug("%s %s %d",child->text(0).toAscii().data(),child->text(1).toAscii().data(),child->checkState(2));
                if (child->checkState(2) == Qt::Checked)
                {
                    if (!packageList->downloadPackage(child->text(0)))
                        qDebug() << "could not download package";
                }
            }
        }
    }
    return true;
}

bool InstallerEngine::installPackages(QTreeWidget *tree,const QString &category)
{
    for (int i = 0; i < tree->topLevelItemCount(); i++)
    {
        QTreeWidgetItem *item = tree->topLevelItem(i);
        qDebug() << __FUNCTION__ << " " << item->text(0);
        if (category.isEmpty() || item->text(0) == category)
        {
            PackageList *packageList = getPackageListByName(item->text(0));
            if (!packageList)
            {
                qDebug() << __FUNCTION__ << " packagelist for " << item->text(0) << " not found";
                continue;
            }
            for (int j = 0; j < item->childCount(); j++)
            {
                QTreeWidgetItem *child = item->child(j);
                qDebug("%s %s %d",child->text(0).toAscii().data(),child->text(1).toAscii().data(),child->checkState(2));
                if (child->checkState(2) == Qt::Checked)
                {
                    if (!packageList->installPackage(child->text(0)))
                        qDebug() << "could not install package";
                }
            }
        		packageList->writeToFile();
        }
    }
    return true;
}

void InstallerEngine::setRoot(const QString &root)
{
    m_settings.setValue("rootdir", QDir::convertSeparators(root));
}

QString InstallerEngine::root() const
{
    QString root = m_settings.value("rootdir").toString();
    if (root.isEmpty())
        return QDir::convertSeparators(QDir::currentPath());
    return root;
}

#else
#include "installerengine.h"
#endif

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
                              "<h2>Release " VERSION "</h2>"
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
    return 1;
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
{}

bool FinishPage::isComplete()
{
    return 1;
}

#include "installwizard.moc"
