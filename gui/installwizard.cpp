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
#include "uninstaller.h"
#include "unpacker.h"

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

InstallWizard::InstallWizard(QWidget *parent) : QWizard(parent), m_lastId(0){
    engine = new InstallerEngineGui(this);
    connect(engine, SIGNAL(error(const QString &)), this, SLOT(slotEngineError(const QString &)) );

    // must be first
    setWizardStyle(QWizard::ModernStyle);
    setPixmap(QWizard::LogoPixmap,QPixmap(":/images/logo.png"));
    // setting a banner limit the installer width 
    // setPixmap(QWizard::BannerPixmap, QPixmap(":/images/banner.png"));
/*
    QPushButton *aboutButton = new QPushButton(tr("About"));
    setButton(QWizard::CustomButton1, aboutButton );
    setOption(QWizard::HaveCustomButton1, true);
    connect(aboutButton, SIGNAL(clicked()), this, SLOT(aboutButtonClicked()) );
*/
    _settingsPage = new SettingsPage(this);
    QPushButton *settingsButton = new QPushButton(tr("Settings"));
    setButton(QWizard::CustomButton2, settingsButton);
    setOption(QWizard::HaveCustomButton2, true);
    connect(settingsButton, SIGNAL(clicked()), this, SLOT(settingsButtonClicked()) );
    settingsButton->hide();

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
    setWindowFlags(windowFlags()|Qt::WindowMaximizeButtonHint);

    InstallerDialogs &d = InstallerDialogs::getInstance();
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
    _settingsPage->setGlobalConfig(engine->globalConfig());
    _settingsPage->init();
    _settingsPage->exec();
}

void InstallWizard::reject()
{
    //there is no page set for this signal 
    int i = currentId();
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
    Settings &settings = Settings::getInstance();

    settings.beginGroup("Geometry");
    settings.setValue("normalGeometry", normalGeometry());
    settings.setValue("maximized", isMaximized());
    settings.endGroup();
}

void InstallWizard::readSettings()
{
    Settings &settings = Settings::getInstance();

    settings.beginGroup("Geometry");
    setGeometry(settings.value("normalGeometry", QRect(200,200, 400, 400)).toRect());
    if (settings.value("maximized", false).toBool()) {
      setWindowState(Qt::WindowMaximized);
    }

    settings.endGroup();
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
        button(QWizard::BackButton)->setEnabled(false);
        button(QWizard::NextButton)->setEnabled(false);
        if (!engine->downloadPackages(tree)) {
            reject();
            return;
        }
        button(QWizard::BackButton)->setEnabled(true);
        button(QWizard::NextButton)->setEnabled(true);
        if (Settings::getInstance().autoNextStep())
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
        if (Settings::getInstance().autoNextStep())
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
        if (Settings::getInstance().autoNextStep())
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

InstallWizardPage::InstallWizardPage(SettingsSubPage *s) : page(s)
{
#if 1
    statusLabel = new QLabel("", this);
#else
    statusLabel = new QLabel(tr(
    "<hr><br>Note: Move the mouse over one of the labels on the left side and wait some seconds to see "
    " detailed informations about this topic"
    ), this);
#endif
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

TitlePage::TitlePage() : InstallWizardPage()
{
    setTitle(tr("KDE for Windows Installer"));
    setSubTitle(tr("Release " VERSION));
/*
    QLabel* version = new QLabel(tr("<h3>Release " VERSION "</h3>"));
    version->setAlignment(Qt::AlignRight);
*/
    QLabel* description = new QLabel(tr(
                          "<p>This setup program is used for the installation of KDE for Windows application.</p>"
                          "<p>The pages that follow will guide you through the installation.</p>"
                          "<p>Please note that by default this installer will install "
                          "only a basic set of applications by default. You can always "
                          "run this program at any time in the future to add, remove or "
                          "upgrade packages if necessary.</p>"
                          "<p>This software is licensed under an open source license and build with Qt<sup>1</sup> version " QTVERSION "</p>"
                          "<p>Contributors: <ul>"
                          "<li>Ralf Habacker - main application, design, maintainer"
                          "<li>Christian Ehrlicher - curl and uncompress support, bug fixing"
                          "<li>Patrick Spendrin - start menu support"
                          "</ul></p>"
                          "<p> </p>"
                          "<p> </p>"
                          "<p> </p>"
                          "<p> </p>"
                          "<p> </p>"
                          "<p> </p>"
                          "<p> </p>"
                          "<small><sup>1</sup> Qt and the Qt logo are trademarks of Trolltech in Norway, the United States and other countries.</small>"
                          ));


    description->setWordWrap(true);
    //    downloadPackagesRadioButton = new QRadioButton(tr("download packages"));
    //    downloadAndInstallRadioButton = new QRadioButton(tr("&download and install packages"));
    //    setFocusProxy(downloadPackagesRadioButton);

    // @TODO add select box for experts mode 

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

int TitlePage::nextId() const
{
    Settings &s = Settings::getInstance();
    if (s.isFirstRun() || s.showTitlePage())
        return InstallWizard::pathSettingsPage;
    else
        return InstallWizard::packageSelectorPage;
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

int PathSettingsPage::nextId() const
{
    return InstallWizard::proxySettingsPage;
}

bool PathSettingsPage::validatePage()
{
    page->accept();
    return true;
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

int ProxySettingsPage::nextId() const
{
    page->accept();
    return InstallWizard::downloadSettingsPage;
}

bool ProxySettingsPage::validatePage()
{
    page->accept();
    return true;
}

bool ProxySettingsPage::isComplete()
{
    return page->isComplete();
}

DownloadSettingsPage::DownloadSettingsPage(SettingsSubPage *s) : InstallWizardPage(s)
{
    setTitle(tr("Download Settings"));
    setSubTitle(tr("Select the directory where downloaded files are finishPaged into."));

    QVBoxLayout *layout = new QVBoxLayout;
    layout->addWidget(page->widget(),10);
    layout->addWidget(statusLabel,1,Qt::AlignBottom);
    setLayout(layout);
}

void DownloadSettingsPage::initializePage()
{
    page->reset();
}

int DownloadSettingsPage::nextId() const
{
    page->accept();
    return InstallWizard::mirrorSettingsPage;
}

bool DownloadSettingsPage::validatePage()
{
    page->accept();
    return true;
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

int MirrorSettingsPage::nextId() const
{
    return InstallWizard::packageSelectorPage;
}

bool MirrorSettingsPage::validatePage()
{
    page->accept();
    Settings &s = Settings::getInstance();
    s.setFirstRun(false);
    wizard()->button(QWizard::CustomButton2)->show();
    return true;
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
    wizard()->button(QWizard::CustomButton3)->hide();
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

int PackageSelectorPage::nextId() const
{
    if (g_dependenciesList->count() > 0)
        return InstallWizard::dependenciesPage;
    else
       return InstallWizard::downloadPage;
}

bool PackageSelectorPage::validatePage()
{
    disconnect(tree,SIGNAL(itemClicked(QTreeWidgetItem *, int)),0,0);
    disconnect(leftTree,SIGNAL(itemClicked(QTreeWidgetItem *, int)),0,0);
    disconnect(&Settings::getInstance(),SIGNAL(installDirChanged(const QString &)),0,0);
    disconnect(&Settings::getInstance(),SIGNAL(compilerTypeChanged()),0,0);
    engine->checkUpdateDependencies();
    wizard()->button(QWizard::CustomButton2)->hide();
    return true;
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

int DependenciesPage::nextId() const
{
    return InstallWizard::downloadPage;
}

bool DependenciesPage::validatePage()
{
    return true;
}

DownloadPage::DownloadPage() : InstallWizardPage(0)
{
    setTitle(tr("Downloading packages"));
    setSubTitle(tr(" "));

    DownloaderProgress *progress = new DownloaderProgress(this);
    QVBoxLayout *layout = new QVBoxLayout;
    layout->addWidget(progress);
    layout->addStretch(1);
    setLayout(layout);

    Downloader::instance()->setProgress(progress);
}

void DownloadPage::initializePage()
{
    wizard()->setOption(QWizard::HaveCustomButton2,false);
}

int DownloadPage::nextId() const
{
    return InstallWizard::uninstallPage;
}

bool DownloadPage::isComplete()
{
    return true;
}

void DownloadPage::cancel()
{
    //@ TODO 
    engine->stop();
}

bool DownloadPage::validatePage()
{
    return true;
}

UninstallPage::UninstallPage() : InstallWizardPage(0)
{
    setTitle(tr("Uninstalling packages"));
    setSubTitle(tr(" "));

    InstallerProgress *progress = new InstallerProgress(this);
    QVBoxLayout *layout = new QVBoxLayout;
    layout->addWidget(progress);
    layout->addStretch(1);
    setLayout(layout);
    Uninstaller::instance()->setProgress(progress);
}

void UninstallPage::initializePage()
{
}

int UninstallPage::nextId() const
{
    return InstallWizard::installPage;
}

bool UninstallPage::isComplete()
{
    return true;
}

void UninstallPage::cancel()
{
    engine->stop();
}

bool UninstallPage::validatePage()
{
    return true;
}

InstallPage::InstallPage() : InstallWizardPage(0)
{
    setTitle(tr("Installing packages"));
    setSubTitle(tr(" "));

    InstallerProgress *progress = new InstallerProgress(this);
    QVBoxLayout *layout = new QVBoxLayout;
    layout->addWidget(progress);
    layout->addStretch(1);
    setLayout(layout);
    Unpacker::instance()->setProgress(progress);
}

void InstallPage::initializePage()
{
}

int InstallPage::nextId() const
{
    return InstallWizard::finishPage;
}

bool InstallPage::isComplete()
{
    return true;
}

void InstallPage::cancel()
{
    engine->stop();
}

bool InstallPage::validatePage()
{
    return true;
}

FinishPage::FinishPage() : InstallWizardPage(0)
{
    setTitle(tr("Installation finished"));
    setSubTitle(tr(" "));

    QLabel* label = new QLabel(tr(
         "<p>Now you should be able to run kde applications. "
         "Please open an explorer window and navigate to the bin folder of the kde installation root. "
         "There you will find several applications which can be started by a simple click on the executable.</p>"
         "<p>In further versions of this installer it will be also possible "
         "to start kde applications from the windows start menu.</p>"
         "<p>If you <ul>"
         "<li>like to see the KDE on windows project web site see <a href=\"http://windows.kde.org\">http://windows.kde.org</a></li>"
         "<li>like to get community support for this installer and/or running kde applications please contact "
            "the <a href=\"http://mail.kde.org/mailman/listinfo/kde-windows\">kde-windows@kde.org</a> mailing list.</li>"
         "<li>like to contribute time and/or money to this project contact also the above mentioned list."
            "There are always interesting projects where you can join.</li>"
         "</ul></p>"
         "<p>Have fun using KDE on windows.</p>" 
         "<p> </p>"
         "<p><b>The KDE on Windows team</b></p>"
         "<p>&nbsp;</p>"
         "<p>&nbsp;</p>"
         "<p>This software is designed to provide powerfull and flexible network based install systems using a modern gui toolkit. "
         "If you need assistance or have specific needs for such a services, please contact <a href=\"mailto:ralf@habacker.de\">Ralf Habacker</a></p>"
         ));

    label->setOpenExternalLinks (true);
    label->setWordWrap(true);
    
    QVBoxLayout *layout = new QVBoxLayout;
    layout->addWidget(label);
    layout->addStretch(1);
    setLayout(layout);
}

void FinishPage::initializePage()
{
    setFinalPage(true);
    //wizard()->setOption(QWizard::NoCancelButton,true);
    wizard()->button(QWizard::CancelButton)->setEnabled(false);
#ifdef HAVE_RETRY_BUTTON
    wizard()->setOption(QWizard::HaveCustomButton3, false);
    wizard()->button(QWizard::CustomButton3)->show();
#endif
    wizard()->setOption(QWizard::NoBackButtonOnLastPage,true);
}

bool FinishPage::isComplete()
{
    return true;
}

#include "installwizard.moc"
