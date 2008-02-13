/****************************************************************************
**
** Copyright (C) 2005-2007 Ralf Habacker. All rights reserved.
**
** This file is part of the KDE installer for windows
**
** This library is free software; you can redistribute it and/or
** modify it under the terms of the GNU Library General Public
** License version 2 as published by the Free Software Foundation.
**
** This library is distributed in the hope that it will be useful,
** but WITHOUT ANY WARRANTY; without even the implied warranty of
** MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU
** Library General Public License for more details.
**
** You should have received a copy of the GNU Library General Public License
** along with this library; see the file COPYING.LIB.  If not, write to
** the Free Software Foundation, Inc., 51 Franklin Street, Fifth Floor,
** Boston, MA 02110-1301, USA.
**
****************************************************************************/

// uncomment to display text instead of icons
//#define DISABLE_ICONS
#include "config.h"

#include <QtDebug>
#include <QDir>
#include <QTreeWidget>
#include <QListWidget>
#include <QTextEdit>
#include <QFlags>
#include <QMessageBox>

#include "installerenginegui.h"
#include "downloader.h"
#include "installwizard.h"
#ifdef ENABLE_STYLE
#define DISABLE_ICONS
#endif

//#include "downloader.h"
#include "installer.h"
#include "downloaderprogress.h"
#include "package.h"
#include "packagelist.h"
#include "globalconfig.h"
#include "database.h"

#include "packagestates.h"
#include "installerdialogs.h"

/// holds the package selection and icon states
PackageStates packageStates;

/// holds the package dependency state
PackageStates dependencyStates;

// Column definitions in package list tree widget
const int NameColumn = 0;
const int availableVersionColumn = 1;
const int installedVersionColumn = 2;
const int VersionColumn = 2;

int BINColumn = 3;
int SRCColumn = 4;
int NotesColumn = 5;
int LIBColumn = 6;
int DOCColumn = 7;
int ColumnCount = 8;

static int typeToColumn ( Package::Type type )
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

enum iconType {_install, _autoinstall,_keepinstalled, _update, _remove, _nothing, _disable};

static void setIcon ( QTreeWidgetItem &item, Package::Type type, iconType action )
{
#ifndef DISABLE_ICONS
    static QIcon ai;
    static QIcon ii;
    static QIcon ki;
    static QIcon ni;
    static QIcon id;
    static QIcon dl;
    static QIcon up;

    if ( ii.isNull() ) {
        ai = QIcon ( ":/images/autoinstall.xpm" );
        ii = QIcon ( ":/images/install.xpm" );
        ki = QIcon ( ":/images/keepinstalled.xpm" );
        ni = QIcon ( ":/images/noinst.xpm" );
        id = QIcon ( ":/images/install_disabled.xpm" );
        dl = QIcon ( ":/images/del.xpm" );
        up = QIcon ( ":/images/update.xpm" );
    }
#endif
    int column = typeToColumn ( type );

#ifndef DISABLE_ICONS
    switch ( action ) {
    case _autoinstall:
        item.setIcon ( column, ai );
        return;
    case _install:
        item.setIcon ( column, ii );
        return;
    case _keepinstalled:
        item.setIcon ( column, ki );
        return;
    case _remove:
        item.setIcon ( column, dl );
        return;
    case _update:
        item.setIcon ( column, up );
        return;
    case _nothing:
        item.setIcon ( column, ni );
        return;
    case _disable:
        item.setIcon ( column, id );
        return;
    }
#else
    switch ( action ) {
    case _autoinstall:
        item.setText ( column,"-I-" );
        return;
    case _install:
        item.setText ( column,"-I-" );
        return;
    case _keepinstalled:
        item.setText ( column,"-i-" );
        return;
    case _remove:
        item.setText ( column,"-R-" );
        return;
    case _update:
        item.setText ( column,"-U-" );
        return;
    case _nothing:
        item.setText ( column,"---" );
        return;
    case _disable:
        item.setText ( column,"" );
        return;
    }
#endif
    // FIXME: does not work, don't know how to set the icon size
    // item.icon(column).setIconSize(QSize(22,22));
}

static void setIcon ( QTreeWidgetItem &item, Package::Type type, stateType state, iconType defType )
{
  iconType t = defType;
  switch( state ) {
    case _Install:
      t = _install;
      break;
    case _Update:
      t = _update;
      break;
    case _Remove:
      t = _remove;
      break;
    default:
      break;
  }
  setIcon( item, type, t );
}

void InstallerEngineGui::setInitialState ( QTreeWidgetItem &item, Package *available, Package *installed, int column )
{
    if (available)
    {
        if (available->hasType(Package::BIN))
            setIcon(item,Package::BIN,packageStates.getState(available,Package::BIN),_nothing);
        if (m_installMode == Developer)
        {
            if (available->hasType(Package::LIB))
                setIcon(item,Package::BIN,packageStates.getState(available,Package::LIB),_nothing);
            if (available->hasType(Package::DOC))
                setIcon(item,Package::BIN,packageStates.getState(available,Package::DOC),_nothing);
            if (available->hasType(Package::SRC))
                setIcon(item,Package::SRC,packageStates.getState(available,Package::SRC),_nothing);
        }
        else if (m_installMode == Single)
        {
            if (available->hasType(Package::LIB))
                setIcon(item,Package::LIB,packageStates.getState(available,Package::LIB),_nothing);
            if (available->hasType(Package::DOC))
                setIcon(item,Package::DOC,packageStates.getState(available,Package::DOC),_nothing);
            if (available->hasType(Package::SRC))
                setIcon(item,Package::SRC,packageStates.getState(available,Package::SRC),_nothing);
        }
    }
    if (installed)
    {
        if (installed->isInstalled(Package::BIN))
            setIcon(item,Package::BIN,packageStates.getState(installed,Package::BIN),_keepinstalled);
        if (m_installMode == Developer)
        {
            if (installed->isInstalled(Package::LIB))
                setIcon(item,Package::BIN,packageStates.getState(installed,Package::BIN),_keepinstalled);
            if (installed->isInstalled(Package::DOC))
                setIcon(item,Package::BIN,packageStates.getState(installed,Package::BIN),_keepinstalled);
            if (installed->isInstalled(Package::SRC))
                setIcon(item,Package::SRC,packageStates.getState(installed,Package::BIN),_keepinstalled);
        }
        else if(m_installMode == Single)
        {
            if (installed->isInstalled(Package::LIB))
                setIcon(item,Package::LIB,_keepinstalled);
            if (installed->isInstalled(Package::DOC))
                setIcon(item,Package::DOC,_keepinstalled);
            if (installed->isInstalled(Package::SRC))
                setIcon(item,Package::SRC,_keepinstalled);
        }
    }
}

void InstallerEngineGui::setNextState ( QTreeWidgetItem &item, Package *available, Package *installed, int column )
{
    Package::Type type = columnToType(column);
    if (type == Package::NONE)
        return;

    bool isAvailable;
    bool isInstalled;

    if (type == Package::BIN && m_installMode == Developer)
    {
        isAvailable = available && (available->hasType(type) || available->hasType(Package::LIB) || available->hasType(Package::DOC));
        isInstalled = installed && (installed->isInstalled(type) || installed->hasType(Package::LIB) || installed->hasType(Package::DOC));
    }
    else if (type == Package::BIN && m_installMode == EndUser)
    {
        isAvailable = available && (available->hasType(type) /*|| available->hasType(Package::DOC)*/);
        isInstalled = installed && (installed->isInstalled(type) /*|| installed->hasType(Package::DOC)*/);
    }
    else
    {
        isAvailable = available && available->hasType(type);
        isInstalled = installed && installed->isInstalled(type);
    }
    bool sameVersion = available && installed && available->version() == installed->version();

    stateType currentState = packageStates.getState(available,type);
    stateType newState = _Nothing;
    iconType iconState = _nothing;

    if (currentState == _Nothing)
    {
        if (isAvailable && isInstalled && !sameVersion)
        {
            iconState = _update;
            newState = _Update;
        }
        else if (isAvailable && isInstalled && sameVersion)
        {
            iconState = _remove;
            newState = _Remove;
        }
        else if (!isAvailable && isInstalled)
        {
            iconState = _remove;
            newState = _Remove;
        }
        else if (isAvailable && !isInstalled)
        {
            iconState = _install;
            newState = _Install;
        }
        else if (!isAvailable && !isInstalled)
        {
            iconState = _disable;
            newState = _Nothing;
        }
    }
    else if (currentState == _Update)
    {
        if (isAvailable && isInstalled && !sameVersion)
        {
            iconState = _remove;
            newState = _Remove;
        }
    }
    else if (currentState == _Install)
    {
        if (isAvailable && !isInstalled)
        {
            iconState = _nothing;
            newState = _Nothing;
        }
    }
    else if (currentState == _Remove)
    {
        if (isInstalled)
        {
            iconState =_keepinstalled;
            newState = _Nothing;
        }
    }
    setIcon(item,type,iconState);

    if (available->hasType(type))
        packageStates.setState(available,type,newState);

    // set additional package types for download/install/remove
    if (type == Package::BIN && m_installMode == Developer)
    {
        if (available->hasType(Package::LIB))
            packageStates.setState(available,Package::LIB,newState);
        if (available->hasType(Package::DOC))
            packageStates.setState(available,Package::DOC,newState);
    }
    else if (type == Package::BIN && m_installMode == EndUser)
    {
        ;//if (available->hasType(Package::DOC))
         //   packageStates.setState(available,Package::DOC,newState);
    }
}

bool InstallerEngineGui::checkRemoveDependencies(QTreeWidget *uilist)
{
    return true;
}

void InstallerEngineGui::checkUpdateDependencies(QTreeWidget *uilist)
{
    dependencyStates.clear();
    if (uilist)
        uilist->clear();
    
    uilist->setHeaderLabels(QStringList() << "Package" << "Description");
    Q_FOREACH(Package *pkg, packageStates.packages(m_packageResources)) {
        if (!setDependencyState(pkg,uilist))
            break;
    }
    if (uilist)
        uilist->sortItems(0,Qt::AscendingOrder);
    qDebug() << packageStates;    
    qDebug() << dependencyStates;    
}


bool InstallerEngineGui::setDependencyState(Package *_package, QTreeWidget *list)
{
    stateType state = packageStates.getState(_package,Package::BIN);
    stateType depState = dependencyStates.getState(_package,Package::BIN);
    // @TODO check reverse dependency when deleting
    if ((state == _Nothing || state == _Remove) && (depState == _Nothing || depState == _Remove))
        return true;

    foreach(const QString &dep, _package->deps())
    {
        Package *package = m_packageResources->getPackage(dep);
        if (!package)
            continue;

        // if package is already installed, don't install it again
        if (m_database->getPackage(dep))
            continue;

        stateType state = packageStates.getState(package,Package::BIN);
        stateType depState = dependencyStates.getState(package,Package::BIN);
        // only add package if is neither selected in main states or dep states
        if ((state == _Nothing || state == _Remove) && (depState == _Nothing || depState == _Remove))
        {
            qDebug() << __FUNCTION__ << "selected package" << package->name() << "in previous state" << state << "for installation";
            if (list) 
            {   
                QTreeWidgetItem * item = new QTreeWidgetItem(QStringList() << package->name() << package->notes());
                list->addTopLevelItem(item);
            }
            dependencyStates.setState(package,Package::BIN,_Install);

            // set additional package types for download/install/remove
            if (m_installMode == Developer)
            {
                if (package->hasType(Package::LIB))
                    dependencyStates.setState(package,Package::LIB,_Install);
                if (package->hasType(Package::DOC))
                    dependencyStates.setState(package,Package::DOC,_Install);
            }
            else if (m_installMode == EndUser)
            {
                ;//if (package->hasType(Package::DOC))
                 //   dependenciesStates.setState(package,Package::DOC,_Install);
            }
        }
        setDependencyState(package, list);
    }
    return true;
}

bool isMarkedForDownload ( Package *pkg,Package::Type type )
{
    stateType state = packageStates.getState ( pkg, type );
    stateType depState = dependencyStates.getState ( pkg, type );
    bool result = state == _Install || state == _Update || depState == _Install || depState == _Update;
    if (Settings::hasDebug ( "InstallerEngineGui" ) && result)
        qDebug() << __FUNCTION__ << "select package for download" << pkg->name() << type;
    return result;
}

bool isMarkedForInstall ( Package *pkg,Package::Type type )
{
    stateType state = packageStates.getState ( pkg, type );
    stateType depState = dependencyStates.getState ( pkg, type );
    bool result = state == _Install || state == _Update || depState == _Install || depState == _Update;
    if (Settings::hasDebug ( "InstallerEngineGui" ) && result)
        qDebug() << __FUNCTION__ << "select package for installation" << pkg->name() << type;
    return result;
}

bool isMarkedForRemoval ( Package *pkg,Package::Type type )
{
    stateType state = packageStates.getState ( pkg, type );
    stateType depState = dependencyStates.getState ( pkg, type );
    bool result = state == _Remove || state == _Update || depState == _Remove || depState == _Update;;
    if (Settings::hasDebug ( "InstallerEngineGui" ) && result)
        qDebug() << __FUNCTION__ << "select package for removal" << pkg->name() << type;
    return result;
}

InstallerEngineGui::InstallerEngineGui (QWidget *parent)
        : InstallerEngine ( parent ), m_parent(parent)
{
}

bool InstallerEngineGui::init()
{
    m_installMode = Settings::instance().isDeveloperMode() ? Developer : EndUser;
    if (m_installMode == Single)
    {
        BINColumn = 3;
        LIBColumn = 4;
        DOCColumn = 5;
        SRCColumn = 6;
        NotesColumn = 7;
        ColumnCount = 8;
    }
    else if (m_installMode == Developer)
    {
        BINColumn = 3;
        SRCColumn = 4;
        NotesColumn = 5;
        ColumnCount = 6;
        LIBColumn = 0;
        DOCColumn = 0;
    }
    else if (m_installMode == EndUser)
    {
        BINColumn = 3;
        NotesColumn = 4;
        ColumnCount = 5;
        LIBColumn = 0;
        DOCColumn = 0;
        SRCColumn = 0;
    }

    initGlobalConfig();
    if (isInstallerVersionOutdated())
        InstallerDialogs::instance().installerOutdated();
    return initPackages();

    /// @TODO add updates to category cache
}

void InstallerEngineGui::reload()
{
    m_installMode = Settings::instance().isDeveloperMode() ? Developer : EndUser;
    packageStates.clear();
    dependencyStates.clear();
    InstallerEngine::reload();
}

void InstallerEngineGui::setLeftTreeData ( QTreeWidget *tree )
{
    tree->clear();
    tree->setColumnCount ( 1 );
    // header
    QTreeWidgetItem *headerItem = new QTreeWidgetItem();
    headerItem->setText(0,tr ( "Package Categories" ));
    headerItem->setToolTip(0,
        "Url:  "+ m_usedDownloadSource.toString() + 
        "\nDate: " + m_globalConfig->timeStamp().toString("dd.MM.yyyy hh:mm"));
    tree->setHeaderItem(headerItem);

    QList<QTreeWidgetItem *> categoryList;
    QList<QTreeWidgetItem *> items;

    qDebug() << categoryCache.categories();
    Settings &s = Settings::instance();
    foreach (const QString &category,categoryCache.categories())
    {
        QStringList names = category.split(":");
        if ( (s.compilerType() == Settings::MinGW ||s.compilerType() == Settings::MSVC)
            && (names[0] == "msvc" || names[0] == "mingw") )
            continue;

        if (m_installMode == EndUser && !names[0].contains("KDE"))
            continue;

        QTreeWidgetItem *categoryItem = new QTreeWidgetItem ( ( QTreeWidget* ) 0, names );
        categoryItem->setToolTip ( 0, names[1] );
        categoryList.append ( categoryItem );
    }
    tree->insertTopLevelItems ( 0,categoryList );
    tree->expandAll();
    for ( int i = 0; i < tree->columnCount(); i++ )
        tree->resizeColumnToContents ( i );

    if (categoryList.size() > 0)
    {
        tree->setCurrentItem ( categoryList.first() );
        categoryList.first()->setSelected ( true );
    }
}
// @TODO
extern QTreeWidget *tree;

void InstallerEngineGui::on_leftTree_itemClicked ( QTreeWidgetItem *item, int column, QLabel *info )
{
    if (!item || !info)
        return;
    info->setText ( item->text ( 1 ) );
    QString category = item->text ( 0 );
    setPageSelectorWidgetData ( tree,category );
}

void InstallerEngineGui::setPageSelectorWidgetData ( QTreeWidget *tree, QString categoryName )
{
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
    switch ( m_installMode ) {
    case Developer:
        labels << tr ( "Bin/Devel/Doc" ) << tr ( "Src" );
        binToolTip = "select this checkbox to install/remove/update the binary, development and doc part of this package";
        srcToolTip = "select this checkbox to install/remove/update the source of this package";
        break;

    case EndUser:
        labels << tr ( "Bin" );
        binToolTip = "select this checkbox to install/remove/update the binary part of this package";
        break;

    case Single:
        labels << tr ( "All" )<< tr ( "Bin" ) << tr ( "Lib" ) << tr ( "Doc" ) << tr ( "Src" );
        binToolTip = "select this checkbox to install/remove/update the binaries of this package";
        libToolTip = "select this checkbox to install/remove/update header and import libraries of this package";
        docToolTip = "select this checkbox to install/remove/update the documentation of this package";
        srcToolTip = "select this checkbox to install/remove/update the source of this package";
        break;
    }
    labels
    << tr ( "Package notes" )
    ;

    tree->setColumnCount ( ColumnCount );
    tree->setHeaderLabels ( labels );
    // see http://lists.trolltech.com/qt-interest/2006-06/thread00441-0.html
    // and Task Tracker Entry 106731
    //tree->setAlignment(Center);

    // adding top level items
    QList<QTreeWidgetItem *> categoryList;
    QList <Package*> packageList;

    // add packages which are installed but for which no config entry is there 
    foreach(Package *instPackage, categoryCache.packages(categoryName,*m_database)) 
    {
        Package *p = m_packageResources->getPackage(instPackage->name());
        if (!p)
            packageList << instPackage;
    }

    Settings &s = Settings::instance();
    foreach(Package *availablePackage,categoryCache.packages(categoryName,*m_packageResources))
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

    foreach(Package *availablePackage,packageList)
    {
        QString name = availablePackage->name();
        QStringList data;
        Package *installedPackage = m_database->getPackage(availablePackage->name());
        QString installedVersion = installedPackage ? installedPackage->installedVersion() : "";
        QString availableVersion = availablePackage->version();
        availablePackage->setInstalledVersion(installedVersion);

        /// @TODO add version format check to be sure available package is really newer
        data << availablePackage->name()
            << (availableVersion != installedVersion ? availablePackage->version() : "")
            << installedVersion
            << QString();
        QTreeWidgetItem *item = new QTreeWidgetItem ( ( QTreeWidgetItem* ) 0, data );
        setInitialState ( *item,availablePackage,installedPackage,0);

        /// @TODO add printing notes from ver file
        item->setText ( NotesColumn, availablePackage->notes() );
        item->setToolTip ( BINColumn, binToolTip );

        if (m_installMode == Developer)
            item->setToolTip ( SRCColumn, srcToolTip );
        else if (m_installMode == EndUser)
        {
            ;//item->setToolTip ( DOCColumn, docToolTip );
        }
        else if (m_installMode == Single)
        {
            item->setToolTip ( LIBColumn, libToolTip );
            item->setToolTip ( DOCColumn, docToolTip );
            item->setToolTip ( SRCColumn, srcToolTip );
        }
        categoryList.append(item);
    }
    tree->addTopLevelItems ( categoryList );
    tree->expandAll();
    tree->sortItems ( 0,Qt::AscendingOrder );
    for ( int i = 0; i < tree->columnCount(); i++ )
        tree->resizeColumnToContents ( i );
}

void InstallerEngineGui::updatePackageInfo(QTabWidget *packageInfo, const Package *availablePackage, const Package *installedPackage)
{
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
            list += tr ( "---- BIN package ----" ) + "\n" + m_database->getPackageFiles ( installedPackage->name(),Package::BIN ).join ( "\n" ) + "\n";
        if ( installedPackage->isInstalled ( Package::LIB ) )
            list += tr ( "---- LIB package ----" ) + "\n" + m_database->getPackageFiles ( installedPackage->name(),Package::LIB ).join ( "\n" ) + "\n";
        if ( installedPackage->isInstalled ( Package::DOC ) )
            list += tr ( "---- DOC package ----" ) + "\n" + m_database->getPackageFiles ( installedPackage->name(),Package::DOC ).join ( "\n" ) + "\n";
        if ( installedPackage->isInstalled ( Package::SRC ) )
            list += tr ( "---- SRC package ----" ) + "\n" + m_database->getPackageFiles ( installedPackage->name(),Package::SRC ).join ( "\n" ) + "\n";
        if ( list.isEmpty() )
            packageInfo->setTabEnabled ( 2,false );
        else {
            e->setText ( list );
            packageInfo->setTabEnabled ( 2,true );
        }
    } else
        packageInfo->setTabEnabled ( 2,false );
}

void InstallerEngineGui::itemClickedPackageSelectorPage ( QTreeWidgetItem *item, int column, QTabWidget *packageInfo )
{
    QString name = item->text ( NameColumn );
    QString installedVersion = item->text ( installedVersionColumn );
    QString availableVersion = item->text ( availableVersionColumn );

    Package *installedPackage = m_database->getPackage( name,installedVersion.toAscii() );
    Package *availablePackage = getPackageByName ( name,availableVersion  );
    updatePackageInfo(packageInfo, availablePackage, installedPackage);
    if ( !availablePackage && !installedPackage ) {
        qWarning() << __FUNCTION__ << "neither available or installed package present for package" << name;
        return;
    }

    // end Package Info display
    if ( column < BINColumn )
        return;

    if ( column == BINColumn || column == SRCColumn )
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
        setNextState ( *item, availablePackage, installedPackage, column);
    }
    // dependencies are selected later
}

bool InstallerEngineGui::downloadPackageItem(Package *pkg, Package::Type type )
{
    bool all = false; //isMarkedForInstall(pkg,Package::ALL);
    if ( !isMarkedForDownload ( pkg,type ) )
        return true;

    while (!m_canceled) {
        if (pkg->downloadItem ( type ))
            return true;
        if(Downloader::instance()->result() == Downloader::Aborted)
            return false;
        QMessageBox::StandardButton result = QMessageBox::critical(
            m_parent,
            tr("Download failed"),
            tr("The download of %1 failed with error %2").arg(pkg->getUrl(type).toString()).arg(Downloader::instance()->resultString()),
            QMessageBox::Cancel | QMessageBox::Ignore | QMessageBox::Retry,
            QMessageBox::Retry
        );
        if (result == QMessageBox::Cancel)
            return false;
        else if (result == QMessageBox::Ignore)
            return true;
        else if (result == QMessageBox::Retry)
            ; // try once again
        else
            ;
    }
    return false;
}

bool InstallerEngineGui::downloadPackages ( QTreeWidget *tree, const QString &category )
{
    QList<Package*> list = packageStates.packages(m_packageResources);
    Q_FOREACH ( Package *pkg, dependencyStates.packages(m_packageResources) ) 
        list.append(pkg);
    Downloader::instance()->progress()->setFileCount(list.size());
    int i = 0;
    Q_FOREACH ( Package *pkg, list ) {
        if ( !pkg ) {
            i++;
            continue;
        }
        if (m_canceled)
            return false;

        Downloader::instance()->progress()->setFileNumber(i++);
        if (!downloadPackageItem(pkg,Package::BIN))
            return false;
        if (!downloadPackageItem(pkg,Package::LIB))
            return false;
        if (!downloadPackageItem(pkg,Package::DOC))
            return false;
        if (!downloadPackageItem(pkg,Package::SRC))
            return false;
    }
    return true;
}

bool InstallerEngineGui::removePackages ( QTreeWidget *tree, const QString &category )
{
    QList<Package*> list = packageStates.packages(m_packageResources);
    Q_FOREACH ( Package *pkg, dependencyStates.packages(m_packageResources) )
        list.append(pkg);
    m_installer->progress()->setPackageCount(list.size());
    int i = 0;
    Q_FOREACH ( Package *pkg, list ) {
        if ( !pkg )
            continue;
        if (m_canceled)
            return false;

        m_installer->progress()->setPackageNumber(i++);
        bool all = false; //isMarkedForRemoval(pkg,Package::ALL);
        if ( all || isMarkedForRemoval ( pkg,Package::BIN ) )
            pkg->removeItem ( m_installer, Package::BIN );
        if (m_canceled)
            return false;
        if ( all || isMarkedForRemoval ( pkg,Package::LIB ) )
            pkg->removeItem ( m_installer, Package::LIB );
        if (m_canceled)
            return false;
        if ( all || isMarkedForRemoval ( pkg,Package::DOC ) )
            pkg->removeItem ( m_installer, Package::DOC );
        if (m_canceled)
            return false;
        if ( all || isMarkedForRemoval ( pkg,Package::SRC ) )
            pkg->removeItem ( m_installer, Package::SRC );
    }
    return true;
}

bool InstallerEngineGui::installPackages ( QTreeWidget *tree, const QString &_category )
{
    QList<Package*> list = packageStates.packages(m_packageResources);
    Q_FOREACH ( Package *pkg, dependencyStates.packages(m_packageResources) )
        list.append(pkg);
    m_installer->progress()->setPackageCount(list.size());
    int i = 0; 
    Q_FOREACH ( Package *pkg, list ) {
        if ( !pkg )
            continue;
        if (m_canceled)
            return false;

        m_installer->progress()->setPackageNumber(i++);
        bool all = false;//isMarkedForInstall(pkg,Package::ALL);
        if ( all || isMarkedForInstall ( pkg,Package::BIN ) )
            pkg->installItem ( m_installer, Package::BIN );
        if (m_canceled)
            return false;
        if ( all || isMarkedForInstall ( pkg,Package::LIB ) )
            pkg->installItem ( m_installer, Package::LIB );
        if (m_canceled)
            return false;
        if ( all || isMarkedForInstall ( pkg,Package::DOC ) )
            pkg->installItem ( m_installer, Package::DOC );
        if (m_canceled)
            return false;
        if ( all || isMarkedForInstall ( pkg,Package::SRC ) )
            pkg->installItem ( m_installer, Package::SRC );
        // @TODO: where to handle desktop icons creating
    }
    return true;
}
