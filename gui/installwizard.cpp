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

extern InstallWizard *wizard;

// must be global
QTreeWidget *tree;
QTreeWidget *leftTree;

InstallerEngineGui *engine;

QListWidget *g_dependenciesList = 0;


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

InstallWizard::InstallWizard(QWidget *parent)
        : ComplexWizard(parent)
{
    engine = new InstallerEngineGui(this,progressBar,instProgressBar);
    // must be first
    settingsPage = new SettingsPage(this);

    titlePage = new TitlePage(this);
    pathSettingsPage = new PathSettingsPage(this,settingsPage->installPage());
    downloadSettingsPage = new DownloadSettingsPage(this,settingsPage->downloadPage());
    proxySettingsPage = new ProxySettingsPage(this,settingsPage->proxyPage());
    mirrorSettingsPage = new MirrorSettingsPage(this);
    packageSelectorPage = new PackageSelectorPage(this);
    downloadPage = new DownloadPage(this);
    dependenciesPage = new DependenciesPage(this);
    uninstallPage = new UninstallPage(this);
    installPage = new InstallPage(this);
    finishPage = new FinishPage(this);

    setWindowTitle(tr("KDE Installer - Version " VERSION));
    Settings &s = Settings::getInstance();

    if (s.isFirstRun() || s.showTitlePage())
    {
        setFirstPage(titlePage);
        settingsButton->hide();
    }
    else
    {
        engine->init();
        setFirstPage(packageSelectorPage);
    }
}

void InstallWizard::settingsButtonClicked()
{
    settingsPage->setGlobalConfig(engine->globalConfig());
    settingsPage->init();
    settingsPage->exec();
}


InstallWizardPage::InstallWizardPage(InstallWizard *wizard, SettingsSubPage *s)
            : WizardPage(wizard), wizard(wizard), page(s)
{
    statusLabel = new QLabel(tr(
        "<hr><br>Note: Move the mouse over one of the labels on the left side and wait some seconds to see "
        " detailed informations about this topic"
    ));
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

    //    downloadPackagesRadioButton = new QRadioButton(tr("download packages"));
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
    return wizard->pathSettingsPage;
}


PathSettingsPage::PathSettingsPage(InstallWizard *wizard,SettingsSubPage *page)
        : InstallWizardPage(wizard,page)
{
    topLabel = new QLabel(tr(
        "<h1>Basic Setup</h1>"
        "<p>Select the directory where you want to install the KDE packages, "
        "for which compiler this installation should be and which installation mode you prefer.</p>"
    ));
    QVBoxLayout *layout = new QVBoxLayout;
    layout->addWidget(topLabel,1,Qt::AlignTop);
    layout->addWidget(page->widget(),10);
    layout->addWidget(statusLabel,1,Qt::AlignBottom);
    setLayout(layout);
}

void PathSettingsPage::resetPage()
{
    page->reset();
}

bool PathSettingsPage::isComplete()
{
    return page->isComplete();
}

WizardPage *PathSettingsPage::nextPage()
{
    page->accept();
    return wizard->downloadSettingsPage;
}


DownloadSettingsPage::DownloadSettingsPage(InstallWizard *wizard,SettingsSubPage *page)
        : InstallWizardPage(wizard,page)
{
    topLabel = new QLabel(tr(
        "<h1>Download Settings</h1>"
        "<p>Select the directory where your temporay download location should be"
        " and which mirror your like to use</p>"
    ));
    QVBoxLayout *layout = new QVBoxLayout;
    layout->addWidget(topLabel,1,Qt::AlignTop);
    layout->addWidget(page->widget(),10);
    layout->addWidget(statusLabel,1,Qt::AlignBottom);
    setLayout(layout);
}

void DownloadSettingsPage::resetPage()
{
    page->reset();
}

WizardPage *DownloadSettingsPage::nextPage()
{
    page->accept();
    return wizard->proxySettingsPage;
}

bool DownloadSettingsPage::isComplete()
{
    return page->isComplete();
}


ProxySettingsPage::ProxySettingsPage(InstallWizard *wizard,SettingsSubPage *page)
        : InstallWizardPage(wizard,page)
{
    topLabel = new QLabel(tr(
                              "<h1>Select Proxy Settings</h1>"
                              "<p>Choose the proxy type and enter proxy host and port if required.</p>"
                          ));
    QVBoxLayout *layout = new QVBoxLayout;
    layout->addWidget(topLabel,1,Qt::AlignTop);
    layout->addWidget(page->widget(),10);
    layout->addWidget(statusLabel,1,Qt::AlignBottom);
    setLayout(layout);
}

void ProxySettingsPage::resetPage()
{
    page->reset();
}

WizardPage *ProxySettingsPage::nextPage()
{
    page->accept();
    return wizard->mirrorSettingsPage;
}

bool ProxySettingsPage::isComplete()
{
    return page->isComplete();
}


MirrorSettingsPage::MirrorSettingsPage(InstallWizard *wizard)
        : InstallWizardPage(wizard)
{
    topLabel = new QLabel(tr(
                              "<h1>Select Mirror</h1>"
                              "<p>Select the download mirror from where you want to download KDE packages.</p>"
                          ));

    mirrorLabel = new QLabel(tr("Download Mirror:"));
    mirrorEdit = new QComboBox;
    mirrorList = new QStringList;
    QGridLayout *layout = new QGridLayout;
    layout->addWidget(topLabel, 0, 0, 1, 2,Qt::AlignTop);
    layout->setRowMinimumHeight(1, 10);
    layout->addWidget(mirrorLabel, 2, 0);
    layout->addWidget(mirrorEdit, 2, 1);
    /// @TODO add stretch item 
    layout->addWidget(statusLabel,10,0,1,2,Qt::AlignBottom);
    setLayout(layout);
}

void MirrorSettingsPage::initPage()
{
    // obsolate: resetPage does this already
}

void MirrorSettingsPage::resetPage()
{
    engine->initGlobalConfig();
    QList<GlobalConfig::Mirror*>::iterator p = engine->globalConfig()->mirrors()->begin();
    for (; p != engine->globalConfig()->mirrors()->end(); p++) {
        *mirrorList << QString((*p)->url);
    }
    rebuildMirrorList(0);

    Settings &s = Settings::getInstance();
    if (!s.mirror().isEmpty())
        mirrorEdit->setEditText(s.mirror());
}

WizardPage *MirrorSettingsPage::nextPage()
{
    Settings &s = Settings::getInstance();
    s.setMirror(mirrorEdit->currentText());
    engine->initPackages();

    wizard->settingsButton->show();
    s.setFirstRun(false);
    return wizard->packageSelectorPage;
}

bool MirrorSettingsPage::isComplete()
{
    return (!mirrorEdit->currentText().isEmpty());
}

void MirrorSettingsPage::addNewMirror(int index)
{
    if(index == mirrorEdit->count() - 1) {
        bool ok;
        QString text = QInputDialog::getText(this, tr("Add a new Mirror"),
                                             tr("Mirror address:"), QLineEdit::Normal,
                                             QString("http://"), &ok);
        if (ok && !text.isEmpty() && text != "http://") 
            *mirrorList << text;
        rebuildMirrorList(mirrorEdit->count() - 1);
    }
}

void MirrorSettingsPage::rebuildMirrorList(int index)
{
    disconnect(mirrorEdit, SIGNAL(currentIndexChanged(int)), 0, 0);
    mirrorEdit->clear();
    mirrorEdit->addItems(*mirrorList);
    mirrorEdit->addItem(tr("...Add Mirror..."), 0);
    mirrorEdit->setCurrentIndex(index);
    connect(mirrorEdit, SIGNAL(currentIndexChanged(int)), this, SLOT(addNewMirror(int)));
}

PackageSelectorPage::PackageSelectorPage(InstallWizard *wizard)
        : InstallWizardPage(wizard)
{
    topLabel = createTopLabel(tr("<center><b>Please select the required packages</b></center>"));

#ifdef ENABLE_STYLE
    QSplitter *splitter = new QSplitter(wizard);
    splitter->setOrientation(Qt::Vertical);

    leftTree  = new QTreeWidget;
    engine->setLeftTreeData(leftTree);

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
    engine->setPageSelectorWidgetData(tree);

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
    layout->addWidget(topLabel);
    layout->addWidget(splitter);
    setLayout(layout);

#else

    QSplitter *splitter = new QSplitter(wizard);
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
    layout->addWidget(topLabel, 0,0,1,2);
    layout->addWidget(splitter,1,0,1,2);
    layout->setRowStretch(1,10);
    setLayout(layout);
    packageInfo->hide();
#endif

    connect(tree,SIGNAL(itemClicked(QTreeWidgetItem *, int)),this,SLOT(itemClicked(QTreeWidgetItem *, int)));
    connect(leftTree,SIGNAL(itemClicked(QTreeWidgetItem *, int)),this,SLOT(on_leftTree_itemClicked(QTreeWidgetItem *, int)));
    connect(&Settings::getInstance(),SIGNAL(installDirChanged(const QString &)),this,SLOT(installDirChanged(const QString &)));
    connect(&Settings::getInstance(),SIGNAL(compilerTypeChanged()),this,SLOT(slotCompilerTypeChanged()));
}

void PackageSelectorPage::initPage()
{
    engine->setLeftTreeData(leftTree);
    engine->setPageSelectorWidgetData(tree);
    on_leftTree_itemClicked(leftTree->currentItem(), 0);
}

void PackageSelectorPage::on_leftTree_itemClicked(QTreeWidgetItem *item, int column)
{
    engine->on_leftTree_itemClicked(item,column,categoryInfo);
    packageInfo->hide();
}

void PackageSelectorPage::itemClicked(QTreeWidgetItem *item, int column)
{
    packageInfo->show();
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

void PackageSelectorPage::resetPage()
{}

WizardPage *PackageSelectorPage::nextPage()
{
    engine->checkUpdateDependencies();
    wizard->nextButton->setEnabled(true);
    if (wizard->dependenciesPage->dependenciesList->count() > 0)
        return wizard->dependenciesPage;
    else
       return wizard->downloadPage;
}

bool PackageSelectorPage::isComplete()
{
    return true;
}

DependenciesPage::DependenciesPage(InstallWizard *wizard)
        : InstallWizardPage(wizard)
{
    topLabel = createTopLabel(tr("<center><b>Additional Packages</b></center>"));
    dependenciesList = new QListWidget;

    QVBoxLayout *layout = new QVBoxLayout;
    layout->addWidget(topLabel);
    layout->addWidget(dependenciesList);
    g_dependenciesList = dependenciesList;
    setLayout(layout);
}

void DependenciesPage::resetPage()
{
}

WizardPage *DependenciesPage::nextPage()
{
    if (Settings::getInstance().autoNextStep())
    {
        wizard->nextButton->setVisible(false);
        wizard->backButton->setVisible(false);
    }
    g_dependenciesList = 0;
    return wizard->downloadPage;
}

DownloadPage::DownloadPage(InstallWizard *wizard)
        : InstallWizardPage(wizard)
{
    topLabel = createTopLabel(tr("<center><b>Downloading packages</b></center>"));

    QVBoxLayout *layout = new QVBoxLayout;
    layout->addWidget(topLabel);
    setLayout(layout);
}

void DownloadPage::resetPage()
{}

WizardPage *DownloadPage::nextPage()
{
    return wizard->uninstallPage;
}

bool DownloadPage::isComplete()
{
    if (!Settings::getInstance().autoNextStep())
        wizard->nextButton->setEnabled(false);
    QApplication::instance()->processEvents();
    if (!engine->downloadPackages(tree))
        return false;
    if (!Settings::getInstance().autoNextStep())
        wizard->nextButton->setEnabled(true);
    else
        emit wizard->nextButtonClicked();
    return true;
}

void DownloadPage::reject()
{
    engine->stop();
}

UninstallPage::UninstallPage(InstallWizard *wizard)
        : InstallWizardPage(wizard)
{
    topLabel = createTopLabel(tr("<center><b>Removing packages</b></center>"));

    fileList = wizard->instProgressBar;
   
    QVBoxLayout *layout = new QVBoxLayout;
    layout->addWidget(topLabel);
    layout->addWidget(fileList);
    setLayout(layout);
}

void UninstallPage::resetPage()
{}

WizardPage *UninstallPage::nextPage()
{
    return wizard->installPage;
}

bool UninstallPage::isComplete()
{
    if (!Settings::getInstance().autoNextStep())
        wizard->nextButton->setEnabled(false);
    QApplication::instance()->processEvents();
    // FIXME: add remove progressbar 
    engine->removePackages(tree);
    if (!Settings::getInstance().autoNextStep())
        wizard->nextButton->setEnabled(true);
    else
        emit wizard->nextButtonClicked();
    return true;
}

InstallPage::InstallPage(InstallWizard *wizard)
        : InstallWizardPage(wizard)
{
    topLabel = createTopLabel(tr("<center><b>Installing packages</b></center>"));

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
    return wizard->finishPage;
}

bool InstallPage::isComplete()
{
    if (!Settings::getInstance().autoNextStep())
        wizard->nextButton->setEnabled(false);
    QApplication::instance()->processEvents();
    engine->installPackages(tree);
    if (!Settings::getInstance().autoNextStep())
        wizard->nextButton->setEnabled(true);
    else
        emit wizard->nextButtonClicked();
    return true;
}

FinishPage::FinishPage(InstallWizard *wizard)
        : InstallWizardPage(wizard)
{
    topLabel = createTopLabel(tr("<center><b>Installation finished</b></center>"));
    QLabel* label = new QLabel(tr(
                             "<p>If you have questions or problems using this installer "
                             "please ask on the <a href=\"http://mail.kde.org/mailman/listinfo/kde-windows\">"
                             "kde-windows@kde.org</a> mailing list. </p>"
                             "<p>If you would like to join this famous project, please contact also the "
                             "<a href=\"http://mail.kde.org/mailman/listinfo/kde-windows\">kde-windows@kde.org</a> mailing list.</p>"
                             "<p></p>"
                             "<p>Have fun using KDE on windows.</p>" 
                             "<p></p>"
                             "<p></p>"
                             "<p>The KDE on Windows team</p>"
                             ));

    QVBoxLayout *layout = new QVBoxLayout;
    layout->addWidget(topLabel);
    layout->addWidget(label);
    layout->addStretch(1);
    setLayout(layout);
}

void FinishPage::initPage()
{
    /// @TODO back button should go to package selector page 
    // does not work yet 
   wizard->backButton->setVisible(false);
}

void FinishPage::resetPage()
{}

bool FinishPage::isComplete()
{
    return true;
}

#include "installwizard.moc"
