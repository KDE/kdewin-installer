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
const int availableVersionColumn = 1;
const int installedVersionColumn = 2;
const int VersionColumn = 2;

const int ALLColumn = 2;
const int BINColumn = 3;
const int SRCColumn = 4;
const int NotesColumn = 5;
const int LIBColumn = 6;
const int DOCColumn = 7;
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

void InstallerEngineGui::setInitialState ( QTreeWidgetItem &item, Package *available, Package *installed, int column )
{
    if (available) 
    {
        if (available->hasType(Package::BIN))
            setIcon(item,Package::BIN,_nothing);
        if (m_installMode == Developer && available->hasType(Package::LIB))
            setIcon(item,Package::BIN,_nothing);
        if (m_installMode == Developer && available->hasType(Package::DOC))
            setIcon(item,Package::BIN,_nothing);
        if (available->hasType(Package::SRC))
            setIcon(item,Package::SRC,_nothing);
    }
    if (installed) 
    {
        if (installed->isInstalled(Package::BIN))
            setIcon(item,Package::BIN,_keepinstalled);
        if (m_installMode == Developer && installed->isInstalled(Package::LIB))
            setIcon(item,Package::BIN,_keepinstalled);
        if ((m_installMode == EndUser || m_installMode == Developer)
                && installed->isInstalled(Package::DOC))
            setIcon(item,Package::BIN,_keepinstalled);
        if (installed->isInstalled(Package::SRC))
            setIcon(item,Package::SRC,_keepinstalled);
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
        isAvailable = available && (available->hasType(type) || available->hasType(Package::DOC));
        isInstalled = installed && (installed->isInstalled(type) || installed->hasType(Package::DOC));
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
        if (available->hasType(Package::DOC))
            packageStates.setState(available,Package::DOC,newState);
    }  
}

bool InstallerEngineGui::setDependencyState(Package *_package)
{
    stateType state = packageStates.getState(_package,Package::BIN);
    // @TODO check reverse dependency 
    if (state == _Remove || state == _Nothing)
        return true;

    foreach(QString dep, _package->deps()) 
    {
        Package *package = m_packageResources->getPackage(dep);
        if (!package)
            continue;

        // if package is already installed, don't install it again
        if (m_database->getPackage(dep))
            continue;

        stateType state = packageStates.getState(package,Package::BIN);
        if (state == _Nothing || state == _Remove)
        {
            qDebug() << __FUNCTION__ << "selected package" << package->name() << "in previous state" << state << "for installation"; 
            packageStates.setState(package,Package::BIN,_Install);

            // set additional package types for download/install/remove 
            if (m_installMode == Developer)
            {
                if (package->hasType(Package::LIB)) 
                    packageStates.setState(package,Package::LIB,_Install);
                if (package->hasType(Package::DOC))
                    packageStates.setState(package,Package::DOC,_Install);
            }
            else if (m_installMode == EndUser)
            {
                if (package->hasType(Package::DOC))
                    packageStates.setState(package,Package::DOC,_Install);
            }  
        }        
        setDependencyState(package);
    }
    return true;
}

bool isMarkedForDownload ( Package *pkg,Package::Type type )
{
    stateType state = packageStates.getState ( pkg, type );
    bool result = state == _Install || state == _Update;
    if (Settings::hasDebug ( "InstallerEngineGui" ) && result)
        qDebug() << __FUNCTION__ << "select package for download" << pkg->name() << type;
    return result;
}

bool isMarkedForInstall ( Package *pkg,Package::Type type )
{
    stateType state = packageStates.getState ( pkg, type );
    bool result = state == _Install || state == _Update;
    if (Settings::hasDebug ( "InstallerEngineGui" ) && result)
        qDebug() << __FUNCTION__ << "select package for installation" << pkg->name() << type;
    return result;
}

bool isMarkedForRemoval ( Package *pkg,Package::Type type )
{
    stateType state = packageStates.getState ( pkg, type );
    bool result = state == _Remove || state == _Update;
    if (Settings::hasDebug ( "InstallerEngineGui" ) && result)
        qDebug() << __FUNCTION__ << "select package for removal" << pkg->name() << type;
    return result;
}

InstallerEngineGui::InstallerEngineGui (QWidget *parent, DownloaderProgress *progressBar,InstallerProgress *instProgressBar )
        : InstallerEngine ( progressBar,instProgressBar ), m_parent(parent)
{
    m_installMode = Developer; // install/update bin/lib/doc package
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
        if ( (s.compilerType() == Settings::MinGW ||s.compilerType() == Settings::MSVC) 
            && (names[0] == "msvc" || names[0] == "mingw") )
            continue;

        QTreeWidgetItem *categoryItem = new QTreeWidgetItem ( ( QTreeWidget* ) 0, names );
        categoryItem->setToolTip ( 0, names[1] );
        categoryList.append ( categoryItem );
    }
    tree->insertTopLevelItems ( 0,categoryList );
    tree->expandAll();
    tree->sortItems ( 0,Qt::AscendingOrder );
    for ( int i = 0; i < tree->columnCount(); i++ )
        tree->resizeColumnToContents ( i );

    tree->setCurrentItem ( categoryList.first() );
    categoryList.first()->setSelected ( true );
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
    int _ColumnCount ;
    switch ( m_installMode ) {
    case Developer:
        labels << tr ( "bin/lib/doc" );
        _ColumnCount = ColumnCount-3;
        break;

    case EndUser:
        labels << tr ( "bin/doc" );
        _ColumnCount = ColumnCount-3;
        break;

    case Single:
        labels
        << tr ( "all" )
        << tr ( "bin" )
        << tr ( "lib" )
        << tr ( "doc" );
        _ColumnCount = ColumnCount-3;
        break;
    }
    labels
    << tr ( "src" )
    << tr ( "package notes" )
    ;

    tree->setColumnCount ( _ColumnCount );
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
        setInitialState ( *item,availablePackage,installedPackage,0);

        item->setText ( NotesColumn, availablePackage->notes() );
        // FIXME
        //item->setText(8, m_globalConfig->news()->value(pkg->name()+"-"+pkg->version()));
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

void InstallerEngineGui::updatePackageInfo(QTabWidget *packageInfo, const Package *availablePackage, const Package *installedPackage)
{
    if ( !availablePackage && !installedPackage  ) { 
        packageInfo->setEnabled ( false );
        return;
    }
    packageInfo->setEnabled ( true );
    QTextEdit *e = ( QTextEdit* ) packageInfo->widget ( 0 );
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

bool InstallerEngineGui::checkRemoveDependencies()
{
    return true;
}

void InstallerEngineGui::checkUpdateDependencies()
{
    QList<Package*> list = packageStates.packages(m_packageResources);
    QList<Package*>::ConstIterator i = list.constBegin();
    for ( ; i != list.constEnd(); ++i ) {
        Package *pkg = *i;
        if (!setDependencyState(pkg))
            break;
    }
}

bool InstallerEngineGui::downloadPackageItem(Package *pkg, Package::Type type )
{
    bool all = false; //isMarkedForInstall(pkg,Package::ALL);
    if ( !isMarkedForDownload ( pkg,type ) ) 
        return true;

    while (1) {
        if (pkg->downloadItem ( m_downloader, type ))
            return true;
        QMessageBox::StandardButton result = QMessageBox::critical(
            m_parent,
            tr("Download failed"),
            tr("The download of %1 failed with error %2").arg(pkg->getUrl(type).toString()).arg(m_downloader->resultString()),
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
    checkUpdateDependencies();

    qDebug() << __FUNCTION__ << packageStates;
    QList<Package*> list = packageStates.packages(m_packageResources);
    QList<Package*>::ConstIterator i = list.constBegin();
    for ( ; i != list.constEnd(); ++i ) {
        Package *pkg = *i;
        if ( !pkg )
            continue;

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
    QList<Package*>::ConstIterator i = list.constBegin();
    for ( ; i != list.constEnd(); ++i ) {
        Package *pkg = *i;
        if ( !pkg )
            continue;
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
    QList<Package*> list = packageStates.packages(m_packageResources);
    QList<Package*>::ConstIterator i = list.constBegin();
    for ( ; i != list.constEnd(); ++i ) {
        Package *pkg = *i;
        if ( !pkg )
            continue;
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
