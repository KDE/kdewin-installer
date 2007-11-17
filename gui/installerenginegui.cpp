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

#include <QtDebug>
#include <QDir>
#include <QTreeWidget>
#include <QTextEdit>
#include <QFlags>
#include <QMessageBox>

#include <windows.h>

#include "installerenginegui.h"
#include "installwizard.h"
#ifdef ENABLE_STYLE
#define DISABLE_ICONS
#endif

//#include "downloader.h"
//#include "installer.h"
//#include "installerprogress.h"
#include "downloaderprogress.h"
#include "package.h"
#include "packagelist.h"
#include "globalconfig.h"
#include "database.h"

#include "packagestates.h"

/// holds the package selection and icon states
PackageStates packageStates;

// Column definitions in package list tree widget
const int NameColumn = 0;
const int installedVersionColumn = 1;
const int availableVersionColumn = 2;
const int VersionColumn = 2;
const int ALLColumn = 3;
const int BINColumn = 4;
const int LIBColumn = 5;
const int DOCColumn = 6;
const int SRCColumn = 7;
const int NotesColumn = 8;
const int ColumnCount = 8;

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
        return ALLColumn;
    }
}

Package::Type columnToType ( int column )
{
    switch ( column ) {
    case BINColumn:
        return Package::BIN;
    case LIBColumn:
        return Package::LIB;
    case DOCColumn:
        return Package::DOC;
    case SRCColumn:
        return Package::SRC;
    case ALLColumn:
        return Package::ALL;
    default :
        return Package::NONE;
    }
}

enum iconType {_install, _autoinstall,_keepinstalled, _update, _remove, _nothing, _disable};

void setIcon ( QTreeWidgetItem &item, Package::Type type, iconType action )
{
#ifndef DISABLE_ICONS
    static QIcon *ai;
    static QIcon *ii;
    static QIcon *ki;
    static QIcon *ni;
    static QIcon *id;
    static QIcon *dl;
    static QIcon *up;

    if ( !ii ) {
        ai = new QIcon ( ":/images/autoinstall.xpm" );
        ii = new QIcon ( ":/images/install.xpm" );
        ki = new QIcon ( ":/images/keepinstalled.xpm" );
        ni = new QIcon ( ":/images/noinst.xpm" );
        id = new QIcon ( ":/images/install_disabled.xpm" );
        dl = new QIcon ( ":/images/del.xpm" );
        up = new QIcon ( ":/images/update.xpm" );
    }
#endif
    int column = typeToColumn ( type );

#ifndef DISABLE_ICONS
    switch ( action ) {
    case _autoinstall:
        item.setIcon ( column,*ai );
        return;
    case _install:
        item.setIcon ( column,*ii );
        return;
    case _keepinstalled:
        item.setIcon ( column,*ki );
        return;
    case _remove:
        item.setIcon ( column,*dl );
        return;
    case _update:
        item.setIcon ( column,*up );
        return;
    case _nothing:
        item.setIcon ( column,*ni );
        return;
    case _disable:
        item.setIcon ( column,*id );
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

enum actionType { _initial, _next, _deps, _sync};

/**
 @deprecated  use set...State
*/
static void setState ( QTreeWidgetItem &item, Package *available, Package *installed, int column, actionType action, int syncColumn=0 )
{
    Package::Type type = columnToType ( column );
    Package *pkg = installed;
    if ( !pkg )
        return;
    if ( type != Package::ALL && !pkg->hasType ( type ) ) {
        setIcon ( item,type,_disable );
        return;
    }

    switch ( action ) {
    case _initial: {
        stateType state = packageStates.getState ( pkg->name(),pkg->version(),type );

        switch ( state ) {
        case _Install:
            if ( !pkg->isInstalled ( type ) ) {
                setIcon ( item,type,_install );
                packageStates.setState ( pkg->name(),pkg->version(),type,_Install );
            }
            break;
        case _Nothing:
            if ( pkg->isInstalled ( type ) ) {
                setIcon ( item,type,_keepinstalled );
                packageStates.setState ( pkg->name(),pkg->version(),type,_Nothing );
            } else {
                setIcon ( item,type,_nothing );
                packageStates.setState ( pkg->name(),pkg->version(),type,_Nothing );
            }
            break;
        case _Remove:
            if ( pkg->isInstalled ( type ) ) {
                setIcon ( item,type,_remove );
                packageStates.setState ( pkg->name(),pkg->version(),type,_Remove );
            } else {
                setIcon ( item,type,_nothing );
                packageStates.setState ( pkg->name(),pkg->version(),type,_Nothing );
            }
            break;
        }
        break;
    }

    // enter next state depending on current state
    case _next: {
        stateType state = packageStates.getState ( pkg->name(),pkg->version(),type );

        switch ( state ) {
        case _Nothing:
            if ( pkg->isInstalled ( type ) ||
                    ( type == Package::ALL &&
                      ( !pkg->hasType ( Package::BIN ) || pkg->isInstalled ( Package::BIN ) ) &&
                      ( !pkg->hasType ( Package::LIB ) || pkg->isInstalled ( Package::LIB ) ) &&
                      ( !pkg->hasType ( Package::DOC ) || pkg->isInstalled ( Package::DOC ) ) &&
                      ( pkg->hasType ( Package::BIN ) || pkg->hasType ( Package::LIB ) || pkg->hasType ( Package::DOC ) ) ) ) {
                setIcon ( item,type,_remove );
                packageStates.setState ( pkg->name(),pkg->version(),type,_Remove );
            } else {
                setIcon ( item,type,_install );
                packageStates.setState ( pkg->name(),pkg->version(),type,_Install );
            }
            break;

        case _Remove:
            if ( pkg->isInstalled ( type ) ) {
                setIcon ( item,type,_keepinstalled );
                packageStates.setState ( pkg->name(),pkg->version(),type,_Nothing );
            } else {
                setIcon ( item,type,_nothing );
                packageStates.setState ( pkg->name(),pkg->version(),type,_Nothing );
            }
            break;

        case _Install:
            if ( pkg->isInstalled ( type ) ) {
                setIcon ( item,type,_install );
                packageStates.setState ( pkg->name(),pkg->version(),type,_Install );
            } else {
                setIcon ( item,type,_nothing );
                packageStates.setState ( pkg->name(),pkg->version(),type,_Nothing );
            }
            break;
        }
        break;
    }
    // handle dependeny selecting
    case _deps: {
        if ( pkg->isInstalled ( type ) ) {
        } else {
            // FIXME: should be _autoinstall, but then the main package is using this icon too
            setIcon ( item,type,_install );
            packageStates.setState ( pkg->name(),pkg->version(),type,_Install );
        }
        break;
    }
    case _sync: {
        stateType state = packageStates.getState ( pkg->name(),pkg->version(),columnToType ( syncColumn ) );

        switch ( state ) {
        case _Install:
            if ( !pkg->isInstalled ( type ) ) {
                setIcon ( item,type,_install );
                packageStates.setState ( pkg->name(),pkg->version(),type,_Install );
            }
            break;
        case _Nothing:
            if ( !pkg->isInstalled ( type ) ) {
                setIcon ( item,type,_nothing );
                packageStates.setState ( pkg->name(),pkg->version(),type,_Nothing );
            }
            break;
        case _Remove:
            if ( pkg->isInstalled ( type ) ) {
                setIcon ( item,type,_remove );
                packageStates.setState ( pkg->name(),pkg->version(),type,_Remove );
            } else {
                setIcon ( item,type,_nothing );
                packageStates.setState ( pkg->name(),pkg->version(),type,_Nothing );
            }
            break;
        }
        break;
    }
    }
}

static void setInitialState ( QTreeWidgetItem &item, Package *available, Package *installed, int column )
{
    setState ( item,available,installed,column,_initial );
}

static void setNextState ( QTreeWidgetItem &item, Package *available, Package *installed, int column )
{
    setState ( item,available,installed,column,_next );
}

static void setDependencyState ( QTreeWidgetItem &item, Package *available, Package *installed, int column )
{
    setState ( item,available,installed,column,_deps );
}


bool isMarkedForInstall ( Package *pkg,Package::Type type )
{
    stateType state = packageStates.getState ( pkg->name(),pkg->version(),type );
    return state == _Install;
}

bool isMarkedForRemoval ( Package *pkg,Package::Type type )
{
    stateType state = packageStates.getState ( pkg->name(),pkg->version(),type );
    return state == _Remove;
}

InstallerEngineGui::InstallerEngineGui (QWidget *parent, DownloaderProgress *progressBar,InstallerProgress *instProgressBar )
        : InstallerEngine ( progressBar,instProgressBar ), m_parent(parent)
{
    m_installMode = Single;
    //packageStates = new PackageStates(*m_packageResources,*m_database);
}

void InstallerEngineGui::init()
{
    InstallerEngine::init();
}

void InstallerEngineGui::reload()
{
    packageStates.clear();
    InstallerEngine::reload();
}

void InstallerEngineGui::setLeftTreeData ( QTreeWidget *tree )
{
    tree->clear();
    QStringList labels;
    QList<QTreeWidgetItem *> items;
    labels << tr ( "Paket Groups" );
    tree->setColumnCount ( 1 );
    tree->setHeaderLabels ( labels );
    QList<QTreeWidgetItem *> categoryList;

    qDebug() << categoryCache.categories();
    Settings &s = Settings::getInstance();
    foreach (QString category,categoryCache.categories()) 
    {
        QStringList names = category.split(":");
        if (names[0] != "all") 
#if 0
            if ( (s.compilerType() == Settings::MinGW ||s.compilerType() == Settings::MSVC) 
                && (names[0] == "msvc" || names[0] == "mingw") )
#endif
                continue;

        QTreeWidgetItem *categoryItem = new QTreeWidgetItem ( ( QTreeWidget* ) 0, names );
        categoryItem->setToolTip ( 0, names[1] );
        categoryList.append ( categoryItem );
    }
    tree->insertTopLevelItems ( 0,categoryList );
    tree->expandAll();
    tree->sortItems ( 0,Qt::AscendingOrder );
    // FIXME: don't know how to select an item as done with mouse
    tree->setCurrentItem ( categoryList.first() );
    categoryList.first()->setSelected ( true );
    for ( int i = 0; i < tree->columnCount(); i++ )
        tree->resizeColumnToContents ( i );
}
// @TODO
extern QTreeWidget *tree;

void InstallerEngineGui::on_leftTree_itemClicked ( QTreeWidgetItem *item, int column, QTextEdit *info )
{
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
    QString allToolTip ( "select this checkbox to install/remove/update binaries, headers, import libraries and docs of this package" );
    QString binToolTip ( "select this checkbox to install/remove/update the binaries of this package" );
    QString libToolTip ( "select this checkbox to install/remove/update header and import libraries of this package" );
    QString docToolTip ( "select this checkbox to install/remove/update the documentation of this package" );
    QString srcToolTip ( "select this checkbox to install/remove/update the source of this package" );

    labels
    << tr ( "Package" )
    << tr ( "available" )
    << tr ( "installed" );
    switch ( m_installMode ) {
    case Developer:
        labels
        << ""
        << tr ( "bin/lib/doc" )
        << ""
        << "";
        break;

    case EndUser:
        labels
        << ""
        << tr ( "bin/doc" )
        << ""
        << "";
        break;

    case Single:
        labels
        << tr ( "all" )
        << tr ( "bin" )
        << tr ( "lib" )
        << tr ( "doc" );
        break;
    }
    labels
    << tr ( "src" )
    << tr ( "package notes" )
    ;

    tree->setColumnCount ( ColumnCount );
    tree->setHeaderLabels ( labels );
    // see http://lists.trolltech.com/qt-interest/2006-06/thread00441-0.html
    // and Task Tracker Entry 106731
    //tree->setAlignment(Center);

    // adding top level items
    QList<QTreeWidgetItem *> categoryList;

    Settings &s = Settings::getInstance();

#ifdef USE_PACKAGE_POINTER_FROM_CATEGORY_CACHE
    foreach(Package *availablePackage,m_categoryCache->getPackages(categoryName)) 
#else
    foreach(Package *availablePackage,categoryCache.packages(categoryName,*m_packageResources)) 
#endif
    {
        QString name = availablePackage->name();
        if ( ( categoryName == "mingw"  || s.compilerType() == Settings::MinGW )
                && ( name.contains ( "msvc" ) ) )
            continue;
        else if ( ( categoryName == "msvc"  || s.compilerType() == Settings::MSVC )
                  && ( name.contains ( "mingw" ) ) )
            continue;

        QStringList data;
        Package *installedPackage = m_database->getPackage(availablePackage->name());
        QString installedVersion = installedPackage ? installedPackage->version() : "";
        QString availableVersion = availablePackage->version();
        data << availablePackage->name() 
            << (availableVersion != installedVersion ? availablePackage->version() : "")
            << installedVersion 
            << QString();
        QTreeWidgetItem *item = new QTreeWidgetItem ( ( QTreeWidgetItem* ) 0, data );
        if ( m_installMode == Single )
            setInitialState ( *item,availablePackage,installedPackage,ALLColumn );

        setInitialState ( *item,availablePackage,installedPackage,BINColumn );
        if ( m_installMode != Developer ) {
            setInitialState ( *item,availablePackage,installedPackage,LIBColumn );
            setInitialState ( *item,availablePackage,installedPackage,DOCColumn );
        }
        setInitialState ( *item,availablePackage,installedPackage,SRCColumn );
        item->setText ( NotesColumn, availablePackage->notes() );
        // FIXME
        //item->setText(8, m_globalConfig->news()->value(pkg->name()+"-"+pkg->version()));
        item->setToolTip ( ALLColumn, allToolTip );
        item->setToolTip ( BINColumn, binToolTip );
        item->setToolTip ( LIBColumn, libToolTip );
        item->setToolTip ( DOCColumn, docToolTip );
        item->setToolTip ( SRCColumn, srcToolTip );
        categoryList.append(item);
    }
    tree->addTopLevelItems ( categoryList );
    tree->expandAll();
    tree->sortItems ( 0,Qt::AscendingOrder );
    for ( int i = 0; i < tree->columnCount(); i++ )
        tree->resizeColumnToContents ( i );
}


/**
  select depending packages of package pkg
  @param pkg package of which the depending packages should be selected
  @param type Type of selection: ALL or BIN
  @todo add support for indirect dependencies
  @todo check intalled versions of a depending package first to avoid selecting another version
  @todo don't select dependend items when deselecting icon
*/
void InstallerEngineGui::setDependencies ( Package *pkg, Package::Type type )
{
    const QStringList &deps = pkg->deps();

    if ( Settings::hasDebug ( "InstallerEngineGui" ) )
        qDebug() << __FUNCTION__ << "found dependencies" << deps;
    for ( int i = 0; i < deps.size(); ++i ) {
        QString dep = deps.at ( i );
        Package *depPkg = getPackageByName ( dep );
        if ( !depPkg )
            continue;
        //setDependencies(depPkg,type);

        // find item in display list
        QList<QTreeWidgetItem *> items = tree->findItems ( depPkg->name(),Qt::MatchContains | Qt::MatchRecursive );
        if ( !items.size() ) {
            if ( m_installMode == Developer ) {
                packageStates.setState ( depPkg->name(),depPkg->version(), Package::BIN, _Install );
                packageStates.setState ( depPkg->name(),depPkg->version(), Package::LIB, _Install );
                packageStates.setState ( depPkg->name(),depPkg->version(), Package::DOC, _Install );
            } else
                if ( m_installMode == EndUser ) {
                    packageStates.setState ( depPkg->name(),depPkg->version(), Package::BIN, _Install );
                    // lib is excluded
                    packageStates.setState ( depPkg->name(),depPkg->version(), Package::DOC, _Install );
                } else
                    if ( m_installMode == Single ) {
                        if ( type == Package::ALL ) {
                            packageStates.setState ( depPkg->name(),depPkg->version(), Package::BIN, _Install );
                            packageStates.setState ( depPkg->name(),depPkg->version(), Package::LIB, _Install );
                            packageStates.setState ( depPkg->name(),depPkg->version(), Package::DOC, _Install );
                        } else {
                            packageStates.setState ( depPkg->name(),depPkg->version(), Package::BIN, _Install );
                        }
                    }
        }
        if ( Settings::hasDebug ( "InstallerEngineGui" ) )
            qDebug() << __FUNCTION__ << "found " << items.size() << "displayed items";
        for ( int j = 0; j < items.size(); ++j ) {
            QTreeWidgetItem * depItem = static_cast<QTreeWidgetItem*> ( items[j] );
            if ( depItem->text ( VersionColumn ) != depPkg->version() ) {
                if ( Settings::hasDebug ( "InstallerEngineGui" ) )
                    qDebug() << __FUNCTION__  << depItem->text ( NameColumn ) << depItem->text ( VersionColumn ) << "ignored because of version mismatch, requested" << dep;
                continue;
            }
            if ( Settings::hasDebug ( "InstallerEngineGui" ) )
                qDebug() << __FUNCTION__ << depItem->text ( NameColumn ) << depItem->text ( VersionColumn ) << "selected as dependency";
            /// the dependency is only for bin package and one way to switch on
            if ( m_installMode == Developer ) {
                setState ( *depItem,depPkg,0,BINColumn,_deps );
                setState ( *depItem,depPkg,0,LIBColumn,_deps );
                setState ( *depItem,depPkg,0,DOCColumn,_deps );
            } else if ( m_installMode == EndUser ) {
                setState ( *depItem,depPkg,0,BINColumn,_deps );
                // lib is excluded
                setState ( *depItem,depPkg,0,DOCColumn,_deps );
            } else if ( m_installMode == Single ) {
                if ( type == Package::ALL ) {
                    setState ( *depItem,depPkg,0,BINColumn,_deps );
                    setState ( *depItem,depPkg,0,LIBColumn,_deps );
                    setState ( *depItem,depPkg,0,DOCColumn,_deps );
                } else
                    setState ( *depItem,depPkg,0,BINColumn,_deps );
            }
        }
    }
}

void InstallerEngineGui::itemClickedPackageSelectorPage ( QTreeWidgetItem *item, int column, QTabWidget *packageInfo )
{
    Package *pkg = getPackageByName ( item->text ( NameColumn ),item->text ( availableVersionColumn ) );
    if ( !pkg ) {
        packageInfo->setEnabled ( false );
        return;
    }
    Package *installedPkg = m_database->getPackage( item->text ( NameColumn ),item->text ( installedVersionColumn ).toAscii() );

    // Package Info display
    packageInfo->setEnabled ( true );
    QTextEdit *e = ( QTextEdit* ) packageInfo->widget ( 0 );
    if ( !pkg->longNotes().isEmpty() ) {
        packageInfo->setTabEnabled ( 0,true );
        e->setText ( pkg->longNotes() );
    } else {
        packageInfo->setTabEnabled ( 0,false );
        e->setText ( "" );
    }
    e = ( QTextEdit* ) packageInfo->widget ( 1 );
    QString deps = pkg->deps().join ( "\n" );
    if ( !deps.isEmpty() ) {
        packageInfo->setTabEnabled ( 1,true );
        e->setText ( pkg->deps().join ( "\n" ) );
    } else {
        packageInfo->setTabEnabled ( 1,false );
        e->setText ( "" );
    }

    e = ( QTextEdit* ) packageInfo->widget ( 2 );
    if ( e && installedPkg) {
        QString list;
        if ( installedPkg->isInstalled ( Package::BIN ) )
            list += tr ( "---- BIN package ----" ) + "\n" + m_database->getPackageFiles ( pkg->name(),Package::BIN ).join ( "\n" ) + "\n";
        if ( installedPkg->isInstalled ( Package::LIB ) )
            list += tr ( "---- LIB package ----" ) + "\n" + m_database->getPackageFiles ( pkg->name(),Package::LIB ).join ( "\n" ) + "\n";
        if ( installedPkg->isInstalled ( Package::DOC ) )
            list += tr ( "---- DOC package ----" ) + "\n" + m_database->getPackageFiles ( pkg->name(),Package::DOC ).join ( "\n" ) + "\n";
        if ( installedPkg->isInstalled ( Package::SRC ) )
            list += tr ( "---- SRC package ----" ) + "\n" + m_database->getPackageFiles ( pkg->name(),Package::SRC ).join ( "\n" ) + "\n";
        if ( list.isEmpty() )
            packageInfo->setTabEnabled ( 2,false );
        else {
            e->setText ( list );
            packageInfo->setTabEnabled ( 2,true );
        }
    } else
        packageInfo->setTabEnabled ( 2,false );

    // end Package Info display
    if ( column < ALLColumn )
        return;

    if ( m_installMode == Single && column == ALLColumn ) {
        setNextState ( *item, pkg, installedPkg,ALLColumn );
        setState ( *item,pkg,installedPkg,BINColumn,_sync,ALLColumn );
        setState ( *item,pkg,installedPkg,LIBColumn,_sync,ALLColumn );
        setState ( *item,pkg,installedPkg,DOCColumn,_sync,ALLColumn );
        setState ( *item,pkg,installedPkg,SRCColumn,_sync,ALLColumn );
    } else if ( m_installMode == Developer && column == BINColumn ) {
        setNextState ( *item,pkg,installedPkg,BINColumn );
        setNextState ( *item,pkg,installedPkg,LIBColumn );
        setNextState ( *item,pkg,installedPkg,DOCColumn );
    } else if ( m_installMode == EndUser && column == BINColumn ) {
        setNextState ( *item,pkg,installedPkg,BINColumn );
        // lib excluded
        setNextState ( *item,pkg,installedPkg,DOCColumn );
    } else {
        setNextState ( *item,pkg,installedPkg,column );
    }

    // select depending packages in case all or bin is selected
    if ( column == ALLColumn )
        setDependencies ( pkg,Package::ALL );
    else
        if ( column == BINColumn )
            setDependencies ( pkg,Package::BIN );
}

bool InstallerEngineGui::downloadPackageItem(Package *pkg, Package::Type type )
{
    bool all = false; //isMarkedForInstall(pkg,Package::ALL);
    if ( !all && !isMarkedForInstall ( pkg,type ) ) 
        return true;

    while (1) {
        if (pkg->downloadItem ( m_downloader, type ))
            return true;
        QMessageBox::StandardButton result = QMessageBox::critical(
            m_parent,
            tr("Download failed"),
            tr("The download of the package failed with error %1").arg(m_downloader->resultString()),
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
}


bool InstallerEngineGui::downloadPackages ( QTreeWidget *tree, const QString &category )
{
    QList<Package*>::ConstIterator i = m_packageResources->packageList().constBegin();
    for ( ; i != m_packageResources->packageList().constEnd(); ++i ) {
        Package *pkg = *i;
        if ( !pkg )
            continue;
        if ( Settings::hasDebug ( "InstallerEngineGui" ) )
            pkg->dump ( "downloadPackages" );

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
    QList<Package*>::ConstIterator i = m_packageResources->packageList().constBegin();
    for ( ; i != m_packageResources->packageList().constEnd(); ++i ) {
        Package *pkg = *i;
        if ( !pkg )
            continue;
        if ( Settings::hasDebug ( "InstallerEngineGui" ) )
            pkg->dump ( "removePackages" );
        bool all = false; //isMarkedForRemoval(pkg,Package::ALL);
        if ( all | isMarkedForRemoval ( pkg,Package::BIN ) )
            pkg->removeItem ( m_installer, Package::BIN );
        if ( all | isMarkedForRemoval ( pkg,Package::LIB ) )
            pkg->removeItem ( m_installer, Package::LIB );
        if ( all | isMarkedForRemoval ( pkg,Package::DOC ) )
            pkg->removeItem ( m_installer, Package::DOC );
        if ( all | isMarkedForRemoval ( pkg,Package::SRC ) )
            pkg->removeItem ( m_installer, Package::SRC );
    }
    return true;
}

bool InstallerEngineGui::installPackages ( QTreeWidget *tree,const QString &_category )
{
    QList<Package*>::ConstIterator i = m_packageResources->packageList().constBegin();
    for ( ; i != m_packageResources->packageList().constEnd(); ++i ) {
        Package *pkg = *i;
        if ( !pkg )
            continue;
        if ( Settings::hasDebug ( "InstallerEngineGui" ) )
            pkg->dump ( "installPackages" );
        bool all = false;//isMarkedForInstall(pkg,Package::ALL);
        if ( all || isMarkedForInstall ( pkg,Package::BIN ) )
            pkg->installItem ( m_installer, Package::BIN );
        if ( all || isMarkedForInstall ( pkg,Package::LIB ) )
            pkg->installItem ( m_installer, Package::LIB );
        if ( all || isMarkedForInstall ( pkg,Package::DOC ) )
            pkg->installItem ( m_installer, Package::DOC );
        if ( all || isMarkedForInstall ( pkg,Package::SRC ) )
            pkg->installItem ( m_installer, Package::SRC );
        // @TODO: where to handle desktop icons creating
    }
    return true;
}
