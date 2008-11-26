/****************************************************************************
**
** Copyright (C) 2008 Ralf Habacker <ralf.habacker@freenet.de> 
** All rights reserved.
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
#include "debug.h"
#include "downloader.h"
#include "installer.h"
#include "installerprogress.h"
#include "installerenginegui.h"
#include "installerdialogs.h"
#include "package.h"
#include "packagelist.h"
#include "mirrors.h"
#include "settings.h"
#include "uninstaller.h"
#include "unpacker.h"
#include "packageselectorpage.h"

#include <QListWidget>
#include <QSplitter>
#include <QTextEdit>
#include <QTreeWidget>

const int NameColumn = 0;
const int availableVersionColumn = 1;
const int installedVersionColumn = 2;
const int VersionColumn = 2;

static int BINColumn = 3;
static int SRCColumn = 4;
static int NotesColumn = 5;
static int LIBColumn = 6;
static int DOCColumn = 7;
static int ColumnCount = 8;

Package::Type columnToType ( int column )
{
    if (column == BINColumn)
        return Package::BIN;
    else if (column == LIBColumn)
        return Package::LIB;
    else if (column == DOCColumn)
        return Package::DOC;
    else if (column == SRCColumn)
        return Package::SRC;
    else
        return Package::NONE;
}

int typeToColumn ( Package::Type type )
{
    switch ( type ) {
    case Package::BIN :
        return BINColumn;
    case Package::LIB :
        return LIBColumn;
    case Package::DOC :
        return DOCColumn;
    case Package::SRC :
        return SRCColumn;
    default:
        return 0;
    }
}

// must be global
QTreeWidget *tree;
QTreeWidget *leftTree;

extern InstallerEngineGui *engine;

PackageSelectorPage::PackageSelectorPage()  : InstallWizardPage(0)
{
    setTitle(tr("Package Selection"));
    setSubTitle(tr("Please select the packages you would like to install."));

    QSplitter *splitter = new QSplitter(wizard());
    splitter->setOrientation(Qt::Horizontal);

    // left side of splitter 
    leftTree  = new QTreeWidget(splitter);

    categoryInfo = new QLabel();
    //categoryInfo->setReadOnly(true);
    categoryInfo->setWordWrap(true);
    
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
}

void PackageSelectorPage::setLeftTreeStatus()
{
    QTreeWidgetItem *headerItem = leftTree->headerItem();
    QString config;
    QString date = tr("configuration timestamp: %1").arg(engine->globalConfig()->timeStamp().toString(tr("dd.MM.yyyy hh:mm")));
    if (GlobalConfig::isRemoteConfigAvailable()) 
    {
        config = tr("note: no configuration downloaded because local file overrides it\nconfiguration read from %1\n").arg(GlobalConfig::remoteConfigFile().absoluteFilePath());
    }
    else 
    {
        config = tr("configuration downloaded from %1\n").arg(engine->usedDownloadSource().toString());
    }

    headerItem->setToolTip(0,config + date);
}

void PackageSelectorPage::setLeftTreeData()
{
    leftTree->clear();
    leftTree->setColumnCount ( 1 );
    // header
    QTreeWidgetItem *headerItem = new QTreeWidgetItem();
    headerItem->setText(0,tr ( "Package Categories" ));
    leftTree->setHeaderItem(headerItem);
    setLeftTreeStatus();
    QList<QTreeWidgetItem *> categoryList;
    QList<QTreeWidgetItem *> items;

    qDebug() << categoryCache.categories();
    Settings &s = Settings::instance();
    Q_FOREACH (const QString &category,categoryCache.categories())
    {
        const QStringList names = category.split(':');
        if ( (s.compilerType() == Settings::MinGW ||s.compilerType() == Settings::MSVC)
            && (names[0] == QLatin1String("msvc") || names[0] == QLatin1String("mingw")) )
            continue;

        QTreeWidgetItem *categoryItem = new QTreeWidgetItem ( ( QTreeWidget* ) 0, names );
        categoryItem->setToolTip ( 0, names[1] );
        categoryList.append ( categoryItem );
    }
    leftTree->insertTopLevelItems ( 0,categoryList );
    leftTree->expandAll();
    for ( int i = 0; i < tree->columnCount(); i++ )
        leftTree->resizeColumnToContents ( i );

    if (categoryList.size() > 0)
    {
        leftTree->setCurrentItem ( categoryList.first() );
        categoryList.first()->setSelected ( true );
    }
    /// @TODO initial set displayed package list category
}

void PackageSelectorPage::setWidgetData( QString categoryName )
{
    InstallerEngineGui::Type installMode = engine->installMode();
    if (installMode == InstallerEngineGui::Single)
    {
        BINColumn = 3;
        LIBColumn = 4;
        DOCColumn = 5;
        SRCColumn = 6;
        NotesColumn = 7;
        ColumnCount = 8;
    }
    else if (installMode == InstallerEngineGui::Developer)
    {
        BINColumn = 3;
        SRCColumn = 4;
        NotesColumn = 5;
        ColumnCount = 6;
        LIBColumn = 0;
        DOCColumn = 0;
    }
    tree->clear();
    QStringList labels;
    QList<QTreeWidgetItem *> items;
    // QTreeWidgetItem *item;
    QString binToolTip;
    QString libToolTip;
    QString docToolTip;
    QString srcToolTip;

    labels
    << tr ( "Package" )
    << tr ( "Available" )
    << tr ( "Installed" );
    if (installMode == InstallerEngineGui::Single)
    {
        labels 
        << tr ( "Bin" ) 
        << tr ( "Devel" ) 
        << tr ( "Doc" );
    }
    else if (installMode == InstallerEngineGui::Developer)
    {
        labels
        << tr ( "Bin/Devel/Doc" );
    }
    labels 
    << tr ( "Src" )
    << tr ( "Package notes" )
    ;
    binToolTip = "select this checkbox to install/remove/update the binary, development and doc part of this package";
    srcToolTip = "select this checkbox to install/remove/update the source of this package";

    tree->setColumnCount ( ColumnCount );
    tree->setHeaderLabels ( labels );
    // see http://lists.trolltech.com/qt-interest/2006-06/thread00441-0.html
    // and Task Tracker Entry 106731
    //tree->setAlignment(Center);

    // adding top level items
    QList<QTreeWidgetItem *> categoryList;
    QList <Package*> packageList;

    // add packages which are installed but for which no config entry is there 
    Q_FOREACH(Package *instPackage, categoryCache.packages(categoryName,*engine->database())) 
    {
        Package *p = engine->packageResources()->getPackage(instPackage->name());
        if (!p)
            packageList << instPackage;
    }

    Settings &s = Settings::instance();
    Q_FOREACH(Package *availablePackage,categoryCache.packages(categoryName,*engine->packageResources()))
    {
        QString name = availablePackage->name();
        if ( ( categoryName == "mingw"  || s.compilerType() == Settings::MinGW )
                && ( name.endsWith ( QLatin1String( "-msvc" ) ) ) )
            continue;
        else if ( ( categoryName == "msvc"  || s.compilerType() == Settings::MSVC )
                  && ( name.endsWith ( QLatin1String ( "-mingw" ) ) ) )
            continue;
        packageList << availablePackage;
    }

    Q_FOREACH(Package *availablePackage,packageList)
    {
        QString name = availablePackage->name();
        QStringList data;
        Package *installedPackage = engine->database()->getPackage(availablePackage->name());
        Package::PackageVersion installedVersion = installedPackage ? installedPackage->installedVersion() : Package::PackageVersion();
        Package::PackageVersion availableVersion = availablePackage->version();
        availablePackage->setInstalledVersion(installedVersion);

        /// @TODO add version format check to be sure available package is really newer
        data << availablePackage->name()
            << (availableVersion != installedVersion ? availablePackage->version().toString() : "")
            << installedVersion.toString()
            << QString();
        QTreeWidgetItem *item = new QTreeWidgetItem ( ( QTreeWidgetItem* ) 0, data );
        engine->setInitialState ( *item,availablePackage,installedPackage,0);

        /// @TODO add printing notes from ver file
        item->setText ( NotesColumn, availablePackage->notes() );
        item->setToolTip ( BINColumn, binToolTip );

        item->setToolTip ( SRCColumn, srcToolTip );
        categoryList.append(item);
    }
    tree->addTopLevelItems ( categoryList );
    tree->expandAll();
    tree->sortItems ( 0,Qt::AscendingOrder );
    for ( int i = 0; i < tree->columnCount(); i++ )
        tree->resizeColumnToContents ( i );
}

void PackageSelectorPage::updatePackageInfo(const Package *availablePackage, const Package *installedPackage)
{
    if (!packageInfo)
        return;
    if ( !availablePackage && !installedPackage  ) {
        packageInfo->setEnabled ( false );
        return;
    }
    packageInfo->setEnabled ( true );
    QTextEdit *e;
    if (availablePackage) {
        e = ( QTextEdit* ) packageInfo->widget ( 0 );
        if ( !availablePackage->longNotes().isEmpty() ) {
            packageInfo->setTabEnabled ( 0,true );
            e->setText ( availablePackage->longNotes() );
        } else {
            packageInfo->setTabEnabled ( 0,false );
            e->setText ( "" );
        }
        e = ( QTextEdit* ) packageInfo->widget ( 1 );
        QString deps = availablePackage->deps().join ( "\n" );
        if ( !deps.isEmpty() ) {
            packageInfo->setTabEnabled ( 1,true );
            e->setText ( availablePackage->deps().join ( "\n" ) );
        } else {
            packageInfo->setTabEnabled ( 1,false );
            e->setText ( "" );
        }
    }

    e = ( QTextEdit* ) packageInfo->widget ( 2 );
    if ( e && installedPackage) {
        QString list;
        if ( installedPackage->isInstalled ( Package::BIN ) )
            list += tr ( "---- BIN package ----" ) + "\n" + engine->database()->getPackageFiles ( installedPackage->name(),Package::BIN ).join ( "\n" ) + "\n";
        if ( installedPackage->isInstalled ( Package::LIB ) )
            list += tr ( "---- LIB package ----" ) + "\n" + engine->database()->getPackageFiles ( installedPackage->name(),Package::LIB ).join ( "\n" ) + "\n";
        if ( installedPackage->isInstalled ( Package::DOC ) )
            list += tr ( "---- DOC package ----" ) + "\n" + engine->database()->getPackageFiles ( installedPackage->name(),Package::DOC ).join ( "\n" ) + "\n";
        if ( installedPackage->isInstalled ( Package::SRC ) )
            list += tr ( "---- SRC package ----" ) + "\n" + engine->database()->getPackageFiles ( installedPackage->name(),Package::SRC ).join ( "\n" ) + "\n";
        if ( list.isEmpty() )
            packageInfo->setTabEnabled ( 2,false );
        else {
            e->setText ( list );
            packageInfo->setTabEnabled ( 2,true );
        }
    } else
        packageInfo->setTabEnabled ( 2,false );
}

void PackageSelectorPage::initializePage()
{
    Settings::instance().setFirstRun(false);
    Settings::instance().setSkipBasicSettings(true);
    setSettingsButtonVisible(true);
    /// @TODO display separate window
    InstallerDialogs::instance().downloadProgressDialog(this,true,tr("Downloading Package Lists"));
    engine->init();
    InstallerDialogs::instance().downloadProgressDialog(this,false);
    connect(tree,SIGNAL(itemClicked(QTreeWidgetItem *, int)),this,SLOT(itemClicked(QTreeWidgetItem *, int)));
    connect(leftTree,SIGNAL(itemClicked(QTreeWidgetItem *, int)),this,SLOT(on_leftTree_itemClicked(QTreeWidgetItem *, int)));
    connect(&Settings::instance(),SIGNAL(installDirChanged(const QString &)),this,SLOT(installDirChanged(const QString &)));
    connect(&Settings::instance(),SIGNAL(compilerTypeChanged()),this,SLOT(slotCompilerTypeChanged()));
    setLeftTreeData();
    setWidgetData("");
    //engine->setPageSelectorWidgetData(tree);
    on_leftTree_itemClicked(leftTree->currentItem(), 0);
}

void PackageSelectorPage::on_leftTree_itemClicked(QTreeWidgetItem *item, int column)
{
    if (!item || !categoryInfo)
        return;
    categoryInfo->setText ( item->text ( 1 ) );
    QString category = item->text ( 0 );
    setWidgetData( category );
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

    QString name = item->text ( NameColumn );
    QString installedVersion = item->text ( installedVersionColumn );
    QString availableVersion = item->text ( availableVersionColumn );

    Package *installedPackage = engine->database()->getPackage( name,installedVersion.toAscii() );
    Package *availablePackage = engine->getPackageByName ( name,availableVersion  );
    updatePackageInfo(availablePackage, installedPackage);
    if ( !availablePackage && !installedPackage ) {
        qWarning() << __FUNCTION__ << "neither available or installed package present for package" << name;
        return;
    }

    // end Package Info display
    if ( column < BINColumn )
        return;

    if ( column == BINColumn || column == LIBColumn || column == DOCColumn || column == SRCColumn )
    {
#if 0
        if (!checkRemoveDependencies(installedPackage))
            if (QMessageBox::warning(this,
                    "Remove failure",
                    "This package is selected for removal but used by other packages. Should this package really be removed ?",
                    QMessageBox::Cancel | QMessageBox::Ignore,QMessageBox::Cancel
                    ) == QMessageBox::Cancel)
                return;
#endif
        engine->setNextState ( *item, availablePackage, installedPackage, columnToType(column), column);
    }
    // dependencies are selected later
}

void PackageSelectorPage::installDirChanged(const QString &dir)
{
    engine->reload();
    setLeftTreeData();
    setWidgetData();
}

void PackageSelectorPage::slotCompilerTypeChanged()
{
    setLeftTreeData();
    setWidgetData();
}

bool PackageSelectorPage::validatePage()
{
    setSettingsButtonVisible(false);
    return true;
}

void PackageSelectorPage::cleanupPage()
{
    disconnect(tree,SIGNAL(itemClicked(QTreeWidgetItem *, int)),this,SLOT(itemClicked(QTreeWidgetItem *, int)));
    disconnect(leftTree,SIGNAL(itemClicked(QTreeWidgetItem *, int)),this,SLOT(on_leftTree_itemClicked(QTreeWidgetItem *, int)));
    disconnect(&Settings::instance(),SIGNAL(installDirChanged(const QString &)),this,SLOT(installDirChanged(const QString &)));
    disconnect(&Settings::instance(),SIGNAL(compilerTypeChanged()),this,SLOT(slotCompilerTypeChanged()));
    engine->unselectAllPackages();
}

bool PackageSelectorPage::isComplete()
{
    return true;
}
