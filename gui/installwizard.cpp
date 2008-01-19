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
#include <QListWidget>
#include <QPushButton>
#include <QVBoxLayout>
#include <QSplitter>
#include <QGridLayout>
#include <QFileDialog>
#include <QInputDialog>
#include <QApplication>
#include <QTextEdit>

#include "installwizard.h"
#include "downloader.h"
#include "installer.h"
#include "installerprogress.h"
#include "package.h"
#include "packagelist.h"
#include "settings.h"
#include "installerenginegui.h"
#include "settings.h"
#include "settingspage.h"
#include "mirrors.h"
#include "installerdialogs.h"

extern InstallWizard *wizard;

// must be global
QTreeWidget *tree;
QTreeWidget *leftTree;

InstallerEngineGui *engine;

QListWidget *g_dependenciesList = 0;

/*
static
QLabel* createTopLabel(const QString& str)
{
    QLabel* label =  new QLabel(str);
    label->setObjectName("topLabel");
#ifdef ENABLE_STYLE
    label->setFrameStyle(QFrame::StyledPanel);
    label->setStyleSheet("QLabel#topLabel {font-size: 14px;}");
#else
    label->setFrameStyle(QFrame::StyledPanel);
#endif
    label->setMinimumHeight(40);
    label->setMaximumHeight(40);
    return label;
}
*/

InstallerProgress *installProgressBar;
DownloaderProgress *progressBar;

InstallWizard::InstallWizard(QWidget *parent) : QWizard(parent)
{
    progressBar = new DownloaderProgress(this);
    installProgressBar = new InstallerProgress(this);

    engine = new InstallerEngineGui(this,progressBar,installProgressBar);
    // must be first
    setWizardStyle(QWizard::ModernStyle);
    setPixmap(QWizard::LogoPixmap,QPixmap(":/images/logo.png"));
    setPixmap(QWizard::BannerPixmap, QPixmap(":/images/banner.png"));

    QPushButton *aboutButton = new QPushButton(tr("About"));
    setButton(QWizard::CustomButton1, aboutButton );
    setOption(QWizard::HaveCustomButton1, true);
    connect(aboutButton, SIGNAL(clicked()), this, SLOT(aboutButtonClicked()) );

    _settingsPage = new SettingsPage(this);
    QPushButton *settingsButton = new QPushButton(tr("Settings"));
    setButton(QWizard::CustomButton2, settingsButton);
    setOption(QWizard::HaveCustomButton2, true);
    connect(settingsButton, SIGNAL(clicked()), this, SLOT(settingsButtonClicked()) );

    QPushButton *retryButton = new QPushButton(tr("Retry"));
    setButton(QWizard::CustomButton3, retryButton);
    setOption(QWizard::HaveCustomButton3, true);
    connect(retryButton, SIGNAL(clicked()), this, SLOT(restart()) );

    setOption(QWizard::CancelButtonOnLeft,true);

    QList<QWizard::WizardButton> layout;
    layout << QWizard::CustomButton1 << QWizard::Stretch << QWizard::CustomButton2 << QWizard::Stretch 
            << QWizard::CancelButton << QWizard::BackButton 
//            << QWizard::CustomButton3 
            << QWizard::NextButton << QWizard::FinishButton;
    setButtonLayout(layout);

    setPage(titlePage, new TitlePage()); 
    setPage(pathSettingsPage, new PathSettingsPage(_settingsPage->installPage())); 
    setPage(proxySettingsPage, new ProxySettingsPage(_settingsPage->proxyPage())); 
    setPage(downloadSettingsPage, new DownloadSettingsPage(_settingsPage->downloadPage())); 
    setPage(mirrorSettingsPage, new MirrorSettingsPage(_settingsPage->mirrorPage())); 
    setPage(packageSelectorPage, new PackageSelectorPage()); 
    setPage(dependenciesPage, new DependenciesPage()); 
    setPage(downloadPage, new DownloadPage()); 
    setPage(uninstallPage, new UninstallPage()); 
    setPage(installPage, new InstallPage()); 
    setPage(finishPage, new FinishPage()); 

    QString windowTitle = tr("KDE Installer - Version " VERSION);
    setWindowTitle(windowTitle);
    setSizeGripEnabled(true);

    InstallerDialogs &d = InstallerDialogs::getInstance();
    d.setTitle(windowTitle);
    d.setParent(this);

    Settings &s = Settings::getInstance();

    if (s.isFirstRun() || s.showTitlePage())
    {
        setStartId(titlePage);
        settingsButton->hide();
    }
    else
    {
        setStartId(packageSelectorPage);
    }
    connect(this, SIGNAL(currentIdChanged(int)), this, SLOT(slotCurrentIdChanged(int)) );
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
    _settingsPage->setGlobalConfig(engine->globalConfig());
    _settingsPage->init();
    _settingsPage->exec();
}

// @TODO: The nextId() methods are not called for unknown reason, 
// all id change handling is done in slotCurrentIdChanged()
void InstallWizard::slotCurrentIdChanged(int id)
{
    if (id == dependenciesPage) {
        engine->checkUpdateDependencies();
        if (g_dependenciesList->count() == 0)
            next();
    }
    else if (id == downloadPage) {
        button(QWizard::CancelButton)->setEnabled(false);
        button(QWizard::BackButton)->setEnabled(false);
        button(QWizard::NextButton)->setEnabled(false);
        engine->downloadPackages(tree);
        button(QWizard::CancelButton)->setEnabled(true);
        button(QWizard::BackButton)->setEnabled(true);
        button(QWizard::NextButton)->setEnabled(true);
        if (Settings::getInstance().autoNextStep())
            next();
    }
    else if (id == uninstallPage) {
        button(QWizard::CancelButton)->setEnabled(false);
        button(QWizard::BackButton)->setEnabled(false);
        button(QWizard::NextButton)->setEnabled(false);
        engine->removePackages(tree);
        button(QWizard::CancelButton)->setEnabled(true);
        button(QWizard::BackButton)->setEnabled(true);
        button(QWizard::NextButton)->setEnabled(true);
        if (Settings::getInstance().autoNextStep())
            next();
    }
    else if (id == installPage) {
        button(QWizard::CancelButton)->setEnabled(false);
        button(QWizard::BackButton)->setEnabled(false);
        button(QWizard::NextButton)->setEnabled(false);
        engine->installPackages(tree);
        button(QWizard::CancelButton)->setEnabled(true);
        button(QWizard::BackButton)->setEnabled(true);
        button(QWizard::NextButton)->setEnabled(true);
        if (Settings::getInstance().autoNextStep())
            next();
    }
}

InstallWizardPage::InstallWizardPage(SettingsSubPage *s) : page(s)
{
#if 1
    statusLabel = new QLabel("");
#else
    statusLabel = new QLabel(tr(
    "<hr><br>Note: Move the mouse over one of the labels on the left side and wait some seconds to see "
    " detailed informations about this topic"
    ));
#endif
}

void InstallWizardPage::initializePage()
{
}

int InstallWizardPage::nextId() 
{
    return QWizardPage::nextId();
}

bool InstallWizardPage::isComplete()
{
    return QWizardPage::isComplete();
}

TitlePage::TitlePage() : InstallWizardPage()
{
    QGroupBox* box = new QGroupBox;

    setTitle(tr("KDE for Windows Installer"));
    setSubTitle(tr("Release " VERSION));
/*
    QLabel* version = new QLabel(tr("<h3>Release " VERSION "</h3>"));
    version->setAlignment(Qt::AlignRight);
*/
    QLabel* description = new QLabel(tr(
                          "<p>This setup program is used for the initial installation of KDE for Windows application.</p>"
                          "<p>The pages that follow will guide you through the installation.</p>"
                          "<p>Please note that by default this installer will install "
                          "only a basic set of applications by default. You can always "
                          "run this program at any time in the future to add, remove, or "
                          "upgrade packages if necessary.</p>"
                          ));


    description->setWordWrap(true);
    //    downloadPackagesRadioButton = new QRadioButton(tr("download packages"));
    //    downloadAndInstallRadioButton = new QRadioButton(tr("&download and install packages"));
    //    setFocusProxy(downloadPackagesRadioButton);

    QVBoxLayout *layout = new QVBoxLayout;
    layout->addWidget(description);
  //  layout->addWidget(version);
    layout->addSpacing(10);
    //    layout->addWidget(downloadPackagesRadioButton);
    //    layout->addWidget(downloadAndInstallRadioButton);
    layout->addStretch(1);
    setLayout(layout);
}

void TitlePage::initializePage()
{
    //    downloadPackagesRadioButton->setChecked(true);
    setPixmap(QWizard::WatermarkPixmap, QPixmap(":/images/watermark.png"));
}

int TitlePage::nextId()
{
    //wizard->nextButton->setEnabled(false);
    //wizard->nextButton->setEnabled(true);
    return InstallWizard::pathSettingsPage;
}

PathSettingsPage::PathSettingsPage(SettingsSubPage *s) : InstallWizardPage(s)
{
    setTitle(tr("Basic Setup"));
    setSubTitle(tr("Select the directory where you want to install the KDE packages, "
       "for which compiler this installation should be and which installation mode you prefer."
    ));
    QVBoxLayout *layout = new QVBoxLayout;
    // @TODO: remove top level line from widget
    layout->addWidget(page->widget(),10);
    layout->addWidget(statusLabel,1,Qt::AlignBottom);
    setLayout(layout);
}

void PathSettingsPage::initializePage()
{
    page->reset();
    setPixmap(QWizard::WatermarkPixmap, QPixmap());
}

bool PathSettingsPage::isComplete()
{
    return page->isComplete();
}

int PathSettingsPage::nextId()
{
    page->accept();
    return InstallWizard::proxySettingsPage;
}

ProxySettingsPage::ProxySettingsPage(SettingsSubPage *s) : InstallWizardPage(s)
{
    setTitle(tr("Proxy Settings"));
    setSubTitle(tr("Choose the proxy type and enter proxy host and port if required."));
    QVBoxLayout *layout = new QVBoxLayout;
    layout->addWidget(page->widget(),10);
    layout->addWidget(statusLabel,1,Qt::AlignBottom);
    setLayout(layout);
}

void ProxySettingsPage::initializePage()
{
    page->reset();
}

int ProxySettingsPage::nextId()
{
    page->accept();
    return InstallWizard::downloadSettingsPage;
}

bool ProxySettingsPage::isComplete()
{
    return page->isComplete();
}

DownloadSettingsPage::DownloadSettingsPage(SettingsSubPage *s) : InstallWizardPage(s)
{
    setTitle(tr("Download Settings"));
    setSubTitle(tr("Select the directory where downloaded files are saved into."));

    QVBoxLayout *layout = new QVBoxLayout;
    layout->addWidget(page->widget(),10);
    layout->addWidget(statusLabel,1,Qt::AlignBottom);
    setLayout(layout);
}

void DownloadSettingsPage::initializePage()
{
    page->reset();
}

int DownloadSettingsPage::nextId()
{
    page->accept();
    return InstallWizard::mirrorSettingsPage;
}

bool DownloadSettingsPage::isComplete()
{
    return page->isComplete();
}

MirrorSettingsPage::MirrorSettingsPage(SettingsSubPage *s) : InstallWizardPage(s)
{
    setTitle(tr("Mirror settings"));
    setSubTitle(tr("Select the download mirror from where you want to download KDE packages."));

    QVBoxLayout *layout = new QVBoxLayout;
    layout->addWidget(page->widget(),10);
    layout->addWidget(statusLabel,1,Qt::AlignBottom);
    setLayout(layout);
}

void MirrorSettingsPage::initializePage()
{
    page->reset();
}

int MirrorSettingsPage::nextId()
{
    page->accept();
    wizard()->button(QWizard::CustomButton2)->show();

    Settings &s = Settings::getInstance();
    s.setFirstRun(false);
    return InstallWizard::packageSelectorPage;
}

bool MirrorSettingsPage::isComplete()
{
    return page->isComplete();
}

PackageSelectorPage::PackageSelectorPage()  : InstallWizardPage(0)
{
    setTitle(tr("Package selection"));
    setSubTitle(tr("Please select the required packages"));

#ifdef ENABLE_STYLE
    QSplitter *splitter = new QSplitter(wizard);
    splitter->setOrientation(Qt::Vertical);

    leftTree  = new QTreeWidget;
    //engine->setLeftTreeData(leftTree);

    QHBoxLayout* hl = new QHBoxLayout;
    hl->addWidget(leftTree);
    hl->addStretch(2);
    hl->setMargin(0);
    leftTree->setMinimumWidth(300);
    leftTree->setMinimumHeight(100);
    leftTree->setSizePolicy(QSizePolicy::Minimum, QSizePolicy::Preferred);

    QWidget* top = new QWidget;
    top->setLayout(hl);

    tree = new QTreeWidget;
    //engine->setPageSelectorWidgetData(tree);

    splitter->addWidget(top);
    splitter->addWidget(tree);

    QWidget *widget = splitter->widget(0);
    QSizePolicy policy = widget->sizePolicy();
    policy.setVerticalStretch(2);
    widget->setSizePolicy(policy);

    widget = splitter->widget(1);
    policy = widget->sizePolicy();
    policy.setVerticalStretch(5);
    widget->setSizePolicy(policy);


    QVBoxLayout *layout = new QVBoxLayout;
    layout->addWidget(splitter);
    setLayout(layout);

#else

    QSplitter *splitter = new QSplitter(wizard());
    splitter->setOrientation(Qt::Horizontal);

    // left side of splitter 
    leftTree  = new QTreeWidget(splitter);

    categoryInfo = new QTextEdit();
    categoryInfo->setReadOnly(true);

    QWidget *gridLayoutLeft = new QWidget(splitter);
    gridLayoutLeft->setContentsMargins(0, 0, 0, 0);
    QVBoxLayout *vboxLayoutLeft = new QVBoxLayout(gridLayoutLeft);
    vboxLayoutLeft->addWidget(leftTree,4);
    vboxLayoutLeft->addWidget(categoryInfo,1);
    vboxLayoutLeft->setContentsMargins(0, 0, 0, 0);

    // right side of splitter 
    tree = new QTreeWidget(splitter);

    QTextEdit *tab1 = new QTextEdit();
    tab1->setReadOnly(true);
    QTextEdit *tab2 = new QTextEdit();
    tab2->setReadOnly(true);
    QTextEdit *tab3 = new QTextEdit();
    tab3->setReadOnly(true);

    packageInfo = new QTabWidget();
    packageInfo->addTab(tab1,tr("Description"));
    packageInfo->addTab(tab2,tr("Dependencies"));
    packageInfo->addTab(tab3,tr("Files"));

    QWidget *gridLayout = new QWidget(splitter);
    gridLayout->setContentsMargins(0, 0, 0, 0);
    QVBoxLayout *vboxLayout = new QVBoxLayout(gridLayout);
    vboxLayout->addWidget(tree,3);
    vboxLayout->addWidget(packageInfo,1);
    vboxLayout->setContentsMargins(0, 0, 0, 0);

    splitter->addWidget(gridLayoutLeft);
    splitter->addWidget(gridLayout);
    
    // setup widget initial width 
    QWidget *widget = splitter->widget(0);
    QSizePolicy policy = widget->sizePolicy();
    policy.setHorizontalStretch(2);
    widget->setSizePolicy(policy);

    widget = splitter->widget(1);
    policy = widget->sizePolicy();
    policy.setHorizontalStretch(7);
    widget->setSizePolicy(policy);
 
    QGridLayout *layout = new QGridLayout;
    layout->addWidget(splitter,1,0,1,2);
    layout->setRowStretch(1,10);
    setLayout(layout);
    packageInfo->hide();
#endif
}

void PackageSelectorPage::initializePage()
{
    /// @TODO display separate window
    engine->init();
    connect(tree,SIGNAL(itemClicked(QTreeWidgetItem *, int)),this,SLOT(itemClicked(QTreeWidgetItem *, int)));
    connect(leftTree,SIGNAL(itemClicked(QTreeWidgetItem *, int)),this,SLOT(on_leftTree_itemClicked(QTreeWidgetItem *, int)));
    connect(&Settings::getInstance(),SIGNAL(installDirChanged(const QString &)),this,SLOT(installDirChanged(const QString &)));
    connect(&Settings::getInstance(),SIGNAL(compilerTypeChanged()),this,SLOT(slotCompilerTypeChanged()));
    engine->setLeftTreeData(leftTree);
    engine->setPageSelectorWidgetData(tree);
    on_leftTree_itemClicked(leftTree->currentItem(), 0);
    wizard()->button(QWizard::CustomButton2)->show();
}

void PackageSelectorPage::on_leftTree_itemClicked(QTreeWidgetItem *item, int column)
{
    engine->on_leftTree_itemClicked(item,column,categoryInfo);
    packageInfo->hide();
}

void PackageSelectorPage::itemClicked(QTreeWidgetItem *item, int column)
{
    if (column == 0) 
    {
        static QTreeWidgetItem *lastItem = 0;
        if (lastItem == item)
            packageInfo->isVisible() ? packageInfo->hide() : packageInfo->show();
        else
            packageInfo->show();
        lastItem = item;
    }
    else 
        packageInfo->hide();
    engine->itemClickedPackageSelectorPage(item,column,packageInfo);
}

void PackageSelectorPage::installDirChanged(const QString &dir)
{
    engine->reload();
    engine->setLeftTreeData(leftTree);
    engine->setPageSelectorWidgetData(tree);
}

void PackageSelectorPage::slotCompilerTypeChanged()
{
    engine->setLeftTreeData(leftTree);
    engine->setPageSelectorWidgetData(tree);
}

int PackageSelectorPage::nextId()
{
/*
    disconnect(tree,SIGNAL(itemClicked(QTreeWidgetItem *, int)),0,0);
    disconnect(leftTree,SIGNAL(itemClicked(QTreeWidgetItem *, int)),0,0);
    disconnect(&Settings::getInstance(),SIGNAL(installDirChanged(const QString &)),0,0);
    disconnect(&Settings::getInstance(),SIGNAL(compilerTypeChanged()),0,0);
    engine->checkUpdateDependencies();
    wizard()->button(QWizard::CustomButton2)->hide();
    if (g_dependenciesList->count() > 0)
        return InstallWizard::dependenciesPage;
    else
       return InstallWizard::downloadPage;
*/
    return InstallWizard::dependenciesPage;
}

bool PackageSelectorPage::isComplete()
{
    return true;
}

DependenciesPage::DependenciesPage() : InstallWizardPage(0)
{
    setTitle(tr("Additional Packages"));
    setSubTitle(tr("The following packages are selected for installing too because selected packages depends on them"));
    dependenciesList = new QListWidget;

    QVBoxLayout *layout = new QVBoxLayout;
    layout->addWidget(dependenciesList);
    g_dependenciesList = dependenciesList;
    layout->addStretch(1);
    setLayout(layout);
}

void DependenciesPage::initializePage()
{
    wizard()->setOption(QWizard::HaveCustomButton2,false);
}

int DependenciesPage::nextId()
{
    return InstallWizard::downloadPage;
}

DownloadPage::DownloadPage() : InstallWizardPage(0)
{
    setTitle(tr("Downloading packages"));
    setSubTitle(tr(" "));

    QVBoxLayout *layout = new QVBoxLayout;
    layout->addWidget(progressBar);
    layout->addStretch(1);
    setLayout(layout);
}

void DownloadPage::initializePage()
{
    wizard()->setOption(QWizard::HaveCustomButton2,false);
}

int DownloadPage::nextId()
{
    return InstallWizard::uninstallPage;
}

bool DownloadPage::isComplete()
{
    return true;
}

void DownloadPage::reject()
{
    //@ TODO 
    engine->stop();
}

UninstallPage::UninstallPage() : InstallWizardPage(0)
{
    setTitle(tr("Uninstalling packages"));
    setSubTitle(tr(" "));

    QVBoxLayout *layout = new QVBoxLayout;
    layout->addWidget(installProgressBar);
    layout->addStretch(1);
    setLayout(layout);
}

void UninstallPage::initializePage()
{
}

int UninstallPage::nextId()
{
    return InstallWizard::installPage;
}

bool UninstallPage::isComplete()
{
    return true;
}

InstallPage::InstallPage() : InstallWizardPage(0)
{
    setTitle(tr("Installing packages"));
    setSubTitle(tr(" "));

    QVBoxLayout *layout = new QVBoxLayout;
    layout->addWidget(installProgressBar);
    layout->addStretch(1);
    setLayout(layout);
}

void InstallPage::initializePage()
{
}

int InstallPage::nextId()
{
    return InstallWizard::finishPage;
}

bool InstallPage::isComplete()
{
    return true;
}

FinishPage::FinishPage() : InstallWizardPage(0)
{
    setTitle(tr("Installation finished"));
    setSubTitle(tr(" "));

    QLabel* label = new QLabel(tr(
         "Now you should be able to run kde applications."
         "Please open an explorer window and navigate to the bin folder of the kde installation root."
         "There you will find several applications which can be started by a simple click on the executable."
         "In further versions of this installer it will be also possible to start kde applications from<br>"
         "the windows start menu."
         "If you <ul>"
         "<li>have questions about the KDE on windows project see <a href=\"http://windows.kde.org\">http://windows.kde.org</a></li>"
         "<li>like to get a technical overview about this project see <br><a href=\"http://techbase.kde.org/index.php?title=Projects/KDE_on_Windows\">Techbase - KDE on Windows</a></li>"
         "<li>have problems using this installer or with running kde applications<br>"
         "please contact the <a href=\"http://mail.kde.org/mailman/listinfo/kde-windows\">"
         "kde-windows@kde.org</a> mailing list."
         "</li>"
         "<li>like to contribute time and/or money to this project contact us also on the above mentioned list.</li>"
         "</ul>"
         ""
         "Have fun using KDE on windows." 
         ""
         ""
         "The KDE on Windows team"
    ));

    label->setOpenExternalLinks (true);
    
    QVBoxLayout *layout = new QVBoxLayout;
    layout->addWidget(label);
    layout->addStretch(1);
    setLayout(layout);
}

void FinishPage::initializePage()
{
    setFinalPage(true);
    wizard()->setOption(QWizard::NoCancelButton,true);
    wizard()->button(QWizard::CustomButton3)->show();
    wizard()->setOption(QWizard::NoBackButtonOnLastPage,true);
}

bool FinishPage::isComplete()
{
    return true;
}

#include "installwizard.moc"
