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
    titlePage = new TitlePage(this);
    pathSettingsPage = new PathSettingsPage(this);
    proxySettingsPage = new ProxySettingsPage(this);
    mirrorSettingsPage = new MirrorSettingsPage(this);
    settingsPage = new SettingsPage(this);
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

    tempPathLabel = new QLabel(tr("&Temporary download path:"));
    tempPathEdit = new QLineEdit;
    tempPathLabel->setBuddy(tempPathEdit);
    tempPathEdit->setText(s.downloadDir());

    tempPathSelect = new QPushButton("...", this);
    connect(tempPathSelect, SIGNAL(pressed()),this, SLOT(selectTempPath()));

    compilerLabel = new QLabel(tr("Compiler type:"));
    compilerMinGW = new QRadioButton(tr("MinGW"));
    compilerMSVC = new QRadioButton(tr("MSVC"));
    compilerUnspecified = new QRadioButton(tr("&unspecified"));

    switch (s.compilerType()) {
        case Settings::unspecified: compilerUnspecified->setChecked(true); break;
        case Settings::MinGW: compilerMinGW->setChecked(true); break;
        case Settings::MSVC: compilerMSVC->setChecked(true); break;
        default: compilerUnspecified->setChecked(true); break;
    }

    QGridLayout *layout = new QGridLayout;
    layout->addWidget(topLabel, 0, 0, 1, 2);
    layout->setRowMinimumHeight(1, 10);
    layout->addWidget(rootPathLabel, 2, 0);
    layout->addWidget(rootPathEdit, 2, 1);
    layout->addWidget(rootPathSelect, 2, 2);
    layout->addWidget(tempPathLabel, 3, 0);
    layout->addWidget(tempPathEdit, 3, 1);
    layout->addWidget(tempPathSelect, 3, 2);
    layout->addWidget(compilerLabel, 4, 0);

    QHBoxLayout *layout2 = new QHBoxLayout;
    layout2->addWidget(compilerMinGW, 0);
    layout2->addWidget(compilerMSVC, 1);
    layout2->addWidget(compilerUnspecified, 2);
    layout2->addWidget(compilerUnspecified, 2);
    layout2->addStretch(10);
    layout->addLayout(layout2, 4,1,1,3);

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

    if (compilerUnspecified->isChecked())
        s.setCompilerType(Settings::unspecified);
    if (compilerMinGW->isChecked())
        s.setCompilerType(Settings::MinGW);
    if (compilerMSVC->isChecked())
        s.setCompilerType(Settings::MSVC);

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
    proxyUserNameLabel = new QLabel(tr("Username"));
    proxyPasswordLabel = new QLabel(tr("Password"));
    proxyUserName = new QLineEdit();
    proxyPassword = new QLineEdit();
    proxyManual = new QRadioButton();
    proxyIE = new QRadioButton();
    proxyOff = new QRadioButton();
    proxyFireFox = new QRadioButton();
    proxyOff->setText(QApplication::translate("SettingsDialog", "direct connection", 0, QApplication::UnicodeUTF8));
#ifdef Q_WS_WIN
    proxyIE->setText(QApplication::translate("SettingsDialog", "IE settings", 0, QApplication::UnicodeUTF8));
#else
    proxyIE->setText(QApplication::translate("SettingsDialog", "Environment settings", 0, QApplication::UnicodeUTF8));
#endif
    proxyFireFox->setText(QApplication::translate("SettingsDialog", "FireFox settings", 0, QApplication::UnicodeUTF8));
    proxyManual->setText(QApplication::translate("SettingsDialog", "manual settings", 0, QApplication::UnicodeUTF8));

    connect( proxyOff,SIGNAL(clicked(bool)),this,SLOT(switchProxyFields(bool)) );
    connect( proxyIE,SIGNAL(clicked(bool)),this,SLOT(switchProxyFields(bool)) );
    connect( proxyFireFox,SIGNAL(clicked(bool)),this,SLOT(switchProxyFields(bool)) );
    connect( proxyManual,SIGNAL(clicked(bool)),this,SLOT(switchProxyFields(bool)) );

    Settings &s = Settings::getInstance();
    switch (s.proxyMode()) {
#ifdef Q_WS_WIN
        case Settings::InternetExplorer: proxyIE->setChecked(true); break;
#else
        case Settings::Environment: proxyIE->setChecked(true); break;
#endif
        case Settings::Manual: proxyManual->setChecked(true); break;
        case Settings::FireFox: proxyFireFox->setChecked(true); break;
        case Settings::None: 
        default: proxyOff->setChecked(true); break;
    }
    switchProxyFields(true);

    QNetworkProxy proxy;
    s.proxy("",proxy);
    proxyHost->setText(proxy.hostName());
    proxyPort->setText(QString("%1").arg(proxy.port()));
    proxyUserName->setText(proxy.user());
    proxyPassword->setText(proxy.password());

    QGridLayout *layout = new QGridLayout;
    layout->setRowMinimumHeight(1, 10);
    layout->addWidget(topLabel, 0, 0, 1, 2);
    layout->addWidget(proxyOff, 2, 0);
    layout->addWidget(proxyIE, 3, 0);
    layout->addWidget(proxyFireFox, 4, 0);
    layout->addWidget(proxyManual, 5, 0);
    layout->addWidget(proxyHostLabel, 5, 2);
    layout->addWidget(proxyHost, 5, 3);
    layout->addWidget(proxyPortLabel, 5, 4);
    layout->addWidget(proxyPort, 5, 5);
    layout->addWidget(proxyUserNameLabel, 6, 2);
    layout->addWidget(proxyUserName, 6, 3);
    layout->addWidget(proxyPasswordLabel, 6, 4);
    layout->addWidget(proxyPassword, 6, 5);
    setLayout(layout);
}

void ProxySettingsPage::switchProxyFields(bool checked)
{
    proxyHost->setEnabled(proxyManual->isChecked());
    proxyPort->setEnabled(proxyManual->isChecked());
}

void ProxySettingsPage::resetPage()
{}

WizardPage *ProxySettingsPage::nextPage()
{
    Settings &s = Settings::getInstance();

    Settings::ProxyMode m = Settings::None;
    if(proxyIE->isChecked())
        m = Settings::InternetExplorer;
    if(proxyFireFox->isChecked())
        m = Settings::FireFox;
    if(proxyManual->isChecked())
        m = Settings::Manual;
    s.setProxyMode(m);
    if (proxyManual->isChecked()) 
    {
        QNetworkProxy proxy(QNetworkProxy::DefaultProxy,proxyHost->text(),proxyPort->text().toInt(),proxyUserName->text(),proxyPassword->text());
        s.setProxy(proxy);
    }
    return wizard->mirrorSettingsPage;
}

bool ProxySettingsPage::isComplete()
{
    return true;
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
    layout->addWidget(topLabel, 0, 0, 1, 2);
    layout->setRowMinimumHeight(1, 10);
    layout->addWidget(mirrorLabel, 2, 0);
    layout->addWidget(mirrorEdit, 2, 1);
    setLayout(layout);
}

void MirrorSettingsPage::initPage()
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

void MirrorSettingsPage::resetPage()
{}

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
    // wizard->backButton->setEnabled(false);
}

void FinishPage::resetPage()
{}

bool FinishPage::isComplete()
{
    return true;
}

#include "installwizard.moc"
