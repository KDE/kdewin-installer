/****************************************************************************
**
** Copyright (C) 2005-2008 Ralf Habacker. All rights reserved.
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
#include "debug.h"

#include "installerenginegui.h"
#include "downloader.h"
#include "installwizard.h"
#include "installerupdate.h"
#include "misc.h"

//#include "downloader.h"
#include "installer.h"
#include "downloaderprogress.h"
#include "package.h"
#include "packageinfo.h"
#include "packagelist.h"
#include "globalconfig.h"
#include "database.h"
#include "packagestates.h"
#include "installerdialogs.h"
#ifdef Q_OS_WIN
#include <windows.h>
#endif

#include <QTreeWidget>

int BINColumn = 3;
int SRCColumn = 4;
int DBGColumn = 5;
int NotesColumn = 6;
int LIBColumn = 7;
int DOCColumn = 8;
int ColumnCount = 9;

int typeToColumn ( FileTypes::Type type )
{
    switch ( type ) {
    case FileTypes::BIN :
        return BINColumn;
    case FileTypes::LIB :
        return LIBColumn;
    case FileTypes::DOC :
        return DOCColumn;
    case FileTypes::SRC :
        return SRCColumn;
    case FileTypes::DBG :
        return DBGColumn;
    default:
        return 0;
    }
}

FileTypes::Type columnToType ( int column )
{
    if (column == BINColumn)
        return FileTypes::BIN;
    else if (column == LIBColumn)
        return FileTypes::LIB;
    else if (column == DOCColumn)
        return FileTypes::DOC;
    else if (column == SRCColumn)
        return FileTypes::SRC;
    else if (column == DBGColumn)
        return FileTypes::DBG;
    else
        return FileTypes::NONE;
}

enum iconType {_icon_install, _icon_autoinstall, _icon_keepinstalled, _icon_update, _icon_remove, _icon_nothing, _icon_disable, _icon_dirty };


static void setIcon ( QTreeWidgetItem &item, int column, iconType action )
{
#ifndef DISABLE_ICONS
    static QIcon ai;
    static QIcon ii;
    static QIcon ki;
    static QIcon ni;
    static QIcon id;
    static QIcon dl;
    static QIcon up;
    static QIcon dr;

    if ( ii.isNull() ) {
        ai = QIcon ( ":/autoinstall.xpm" );
        ii = QIcon ( ":/install.xpm" );
        ki = QIcon ( ":/keepinstalled.xpm" );
        ni = QIcon ( ":/noinst.xpm" );
        id = QIcon ( ":/install_disabled.xpm" );
        dl = QIcon ( ":/del.xpm" );
        up = QIcon ( ":/update.xpm" );
        dr = QIcon ( ":/dirty.xpm" );
    }
#endif

#ifndef DISABLE_ICONS
    switch ( action ) {
    case _icon_autoinstall:
        item.setIcon ( column, ai );
        return;
    case _icon_install:
        item.setIcon ( column, ii );
        return;
    case _icon_keepinstalled:
        item.setIcon ( column, ki );
        return;
    case _icon_remove:
        item.setIcon ( column, dl );
        return;
    case _icon_update:
        item.setIcon ( column, up );
        return;
    case _icon_nothing:
        item.setIcon ( column, ni );
        return;
    case _icon_disable:
        item.setIcon ( column, id );
        return;
    case _icon_dirty:
        item.setIcon ( column, dr );
        return;
    }
#else
    switch ( action ) {
    case _icon_autoinstall:
        item.setText ( column,"-I-" );
        return;
    case _icon_install:
        item.setText ( column,"-I-" );
        return;
    case _icon_keepinstalled:
        item.setText ( column,"-i-" );
        return;
    case _icon_remove:
        item.setText ( column,"-R-" );
        return;
    case _icon_update:
        item.setText ( column,"-U-" );
        return;
    case _icon_nothing:
        item.setText ( column,"---" );
        return;
    case _icon_disable:
        item.setText ( column,"" );
        return;
    case _icon_dirty:
        item.setText ( column,"-#-" );
        return;
    }
#endif
    // FIXME: does not work, don't know how to set the icon size
    // item.icon(column).setIconSize(QSize(22,22));
}

//this function must directly take the enum FileTypes::TypeFlags , else the values get cast to int and this function is never called
static void setIcon ( QTreeWidgetItem &item, FileTypes::Type type, stateType state, iconType defType )
{
  iconType t = defType;
  switch( state ) {
    case _Install:
      t = _icon_install;
      break;
    case _Update:
      t = _icon_update;
      break;
    case _Remove:
      t = _icon_remove;
      break;
    default:
      break;
  }
  setIcon( item, typeToColumn(type), t );
}

static void setIcon ( QTreeWidgetItem &item, int column, stateType state, iconType defType )
{
  iconType t = defType;
  switch( state ) {
    case _Install:
      t = _icon_install;
      break;
    case _Update:
      t = _icon_update;
      break;
    case _Remove:
      t = _icon_remove;
      break;
    default:
      break;
  }
  setIcon( item, column, t );
}

static void setIcon ( QTreeWidgetItem &item, FileTypes::Type type, iconType action )
{
    setIcon(item,typeToColumn ( type ), action);
}
    
void InstallerEngineGui::setMetaPackageState(QTreeWidgetItem &item, int column)
{
    bool dirty = false;
    int c = 0;

    for(int i = 0; i < item.childCount(); ++i)
    {
        QString name = item.child(i)->data(column, Qt::StatusTipRole).toString();
        if(packageStates.getState(name, QString(), FileTypes::BIN) == _Install) 
        {
            dirty = true;
            c++;
        }
        else if(packageStates.getState(name, QString(), FileTypes::BIN) == _Update ||
                packageStates.getState(name, QString(), FileTypes::BIN) == _Remove) 
        {
            dirty = true;
        }
        else if(packageStates.getState(name, QString(), FileTypes::BIN) == _Nothing) 
        {
            // we need to find out whether this packages is kept or whether it just isn't installed.
            if(database()->getPackage(name)) {
                c++;
            }
        }
    }
    
    if(dirty && c < item.childCount()) setIcon(item, column, _Nothing, _icon_dirty);
    else if(dirty) setIcon(item, column, _Nothing, _icon_install);
    else if(!dirty && c == item.childCount()) setIcon(item, column, _Nothing, _icon_keepinstalled);
    else setIcon(item, column, _Nothing, _icon_nothing);
}

void InstallerEngineGui::setEndUserInitialState ( QTreeWidgetItem &item, Package *available, Package *installed, int column )
{
    if (installed && available && available->version() != installed->version())
    {
        if (installed->isInstalled(FileTypes::BIN))
        {
            setIcon(item,column,packageStates.getState(installed,FileTypes::BIN),_icon_update);
            packageStates.setState(available,FileTypes::BIN,_Update);
        }
    }
    else if (installed)
    {
        if (installed->isInstalled(FileTypes::BIN))
            setIcon(item,column,packageStates.getState(installed,FileTypes::BIN),_icon_keepinstalled);
    }
    else if (available)
    {
        if (available->hasType(FileTypes::BIN))
            setIcon(item,column,packageStates.getState(available,FileTypes::BIN),_icon_nothing);
        else if (available->hasType(FileTypes::META))
            setMetaPackageState(item, column);
    }
}

bool InstallerEngineGui::isPackageSelected ( Package *available, FileTypes::Type type )
{
    return packageStates.getState(available,type) == _Install;
}


void InstallerEngineGui::setInitialState ( QTreeWidgetItem &item, Package *available, Package *installed, int column )
{
    if (available)
    {
        if (available->hasType(FileTypes::BIN))
            setIcon(item,FileTypes::BIN,packageStates.getState(available,FileTypes::BIN),_icon_nothing);
        if (m_displayMode == Developer)
        {
            if (available->hasType(FileTypes::LIB))
                setIcon(item,FileTypes::BIN,packageStates.getState(available,FileTypes::LIB),_icon_nothing);
            if (available->hasType(FileTypes::DOC))
                setIcon(item,FileTypes::BIN,packageStates.getState(available,FileTypes::DOC),_icon_nothing);
            if (available->hasType(FileTypes::SRC))
                setIcon(item,FileTypes::SRC,packageStates.getState(available,FileTypes::SRC),_icon_nothing);
            if (available->hasType(FileTypes::DBG))
                setIcon(item,FileTypes::DBG,packageStates.getState(available,FileTypes::DBG),_icon_nothing);
        }
        else if (m_displayMode == Single)
        {
            if (available->hasType(FileTypes::LIB))
                setIcon(item,FileTypes::LIB,packageStates.getState(available,FileTypes::LIB),_icon_nothing);
            if (available->hasType(FileTypes::DOC))
                setIcon(item,FileTypes::DOC,packageStates.getState(available,FileTypes::DOC),_icon_nothing);
            if (available->hasType(FileTypes::SRC))
                setIcon(item,FileTypes::SRC,packageStates.getState(available,FileTypes::SRC),_icon_nothing);
            if (available->hasType(FileTypes::DBG))
                setIcon(item,FileTypes::DBG,packageStates.getState(available,FileTypes::DBG),_icon_nothing);
        }
    }
    if (installed)
    {
        if (installed->isInstalled(FileTypes::BIN))
            setIcon(item,FileTypes::BIN,packageStates.getState(installed,FileTypes::BIN),_icon_keepinstalled);
        if (m_displayMode == Developer)
        {
            if (installed->isInstalled(FileTypes::LIB))
                setIcon(item,FileTypes::BIN,packageStates.getState(installed,FileTypes::BIN),_icon_keepinstalled);
            if (installed->isInstalled(FileTypes::DOC))
                setIcon(item,FileTypes::BIN,packageStates.getState(installed,FileTypes::BIN),_icon_keepinstalled);
            if (installed->isInstalled(FileTypes::SRC))
                setIcon(item,FileTypes::SRC,packageStates.getState(installed,FileTypes::BIN),_icon_keepinstalled);
            if (installed->isInstalled(FileTypes::DBG))
                setIcon(item,FileTypes::DBG,packageStates.getState(installed,FileTypes::BIN),_icon_keepinstalled);
        }
        else if(m_displayMode == Single)
        {
            if (installed->isInstalled(FileTypes::LIB))
                setIcon(item,FileTypes::LIB,_icon_keepinstalled);
            if (installed->isInstalled(FileTypes::DOC))
                setIcon(item,FileTypes::DOC,_icon_keepinstalled);
            if (installed->isInstalled(FileTypes::SRC))
                setIcon(item,FileTypes::SRC,_icon_keepinstalled);
            if (installed->isInstalled(FileTypes::DBG))
                setIcon(item,FileTypes::DBG,_icon_keepinstalled);
        }
    }
}

void InstallerEngineGui::setNextState ( QTreeWidgetItem &item, Package *available, Package *installed, FileTypes::Type type, int column, bool handleMetaPackage)
{
    if (type == FileTypes::NONE)
        return;

    bool isAvailable;
    bool isInstalled;

    if (type == FileTypes::BIN && m_displayMode == Developer)
    {
        isAvailable = available && (available->hasType(type) || available->hasType(FileTypes::LIB) || available->hasType(FileTypes::DOC) || available->hasType(FileTypes::DBG));
        isInstalled = installed && (installed->isInstalled(type) || installed->hasType(FileTypes::LIB) || installed->hasType(FileTypes::DOC) || available->hasType(FileTypes::DBG));
    }
    else if (type == FileTypes::BIN && m_displayMode == BinaryOnly)
    {
        isAvailable = available && (available->hasType(type) || available->hasType(FileTypes::DBG));
        isInstalled = installed && (installed->isInstalled(type) || installed->hasType(FileTypes::DBG));
    }
    else
    {
        isAvailable = available && available->hasType(type);
        isInstalled = installed && installed->isInstalled(type);
    }
    bool sameVersion = available && installed && available->version() == installed->version();

    stateType currentState;
    if(available)
        currentState = packageStates.getState(available,type);
    else
        currentState = _Nothing;
    stateType newState = _Nothing;
    iconType iconState = _icon_nothing;

    if (currentState == _Nothing)
    {
        if (isAvailable && isInstalled && !sameVersion)
        {
            iconState = _icon_update;
            newState = _Update;
        }
        else if (isAvailable && isInstalled && sameVersion)
        {
            iconState = _icon_remove;
            newState = _Remove;
        }
        else if (!isAvailable && isInstalled)
        {
            iconState = _icon_remove;
            newState = _Remove;
        }
        else if (isAvailable && !isInstalled)
        {
            iconState = _icon_install;
            newState = _Install;
        }
        else if (!isAvailable && !isInstalled)
        {
            iconState = _icon_disable;
            newState = _Nothing;
        }
    }
    else if (currentState == _Update && !handleMetaPackage)
    {
        if (isAvailable && isInstalled && !sameVersion)
        {
            iconState = _icon_remove;
            newState = _Remove;
        }
    }
    else if (currentState == _Update && handleMetaPackage)
    {
        if (isAvailable && isInstalled && !sameVersion)
        {
            iconState = _icon_update;
            newState = _Update;
        }
    }
    else if (currentState == _Install)
    {
        if (isAvailable && !isInstalled)
        {
            iconState = _icon_nothing;
            newState = _Nothing;
        }
    }
    else if (currentState == _Remove)
    {
        if (isInstalled)
        {
            iconState =_icon_keepinstalled;
            newState = _Nothing;
        }
    }
    setIcon(item,column,iconState);

    if (available->hasType(type))
        packageStates.setState(available,type,newState);

    // set additional package types for download/install/remove
    if (type == FileTypes::BIN && m_displayMode == Developer)
    {
        if (available->hasType(FileTypes::LIB))
            packageStates.setState(available,FileTypes::LIB,newState);
        if (available->hasType(FileTypes::DOC))
            packageStates.setState(available,FileTypes::DOC,newState);
        if (available->hasType(FileTypes::DBG))
            packageStates.setState(available,FileTypes::DBG,newState);
    }
    else if (type == FileTypes::BIN && m_displayMode == BinaryOnly)
    {
        if (available->hasType(FileTypes::DBG) && m_installDebugPackages)
            packageStates.setState(available,FileTypes::DBG,newState);
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
    
    uilist->setHeaderLabels(QStringList() << "Package" << "Version" << "Description");
    Q_FOREACH(Package *pkg, packageStates.packages(m_packageResources)) {
        if (!setDependencyState(pkg,uilist))
            break;
    }
    if (uilist)
    {
        uilist->sortItems(0,Qt::AscendingOrder);
        uilist->resizeColumnToContents(0);
        uilist->resizeColumnToContents(1);
        uilist->resizeColumnToContents(2);
    }
    qDebug() <<  packageStates;
    qDebug() << dependencyStates;
}


bool InstallerEngineGui::setDependencyState(Package *_package, QTreeWidget *list)
{
    qDebug() << __FUNCTION__ << _package->name();

    Q_FOREACH(const QString &dep, _package->deps())
    {
        Package *package = m_packageResources->getPackage(dep);
        if (!package)
            continue;

        // check dependencies first
        setDependencyState(package, list);

        stateType state = packageStates.getState(package,FileTypes::BIN);
        stateType depState = dependencyStates.getState(package,FileTypes::BIN);

        Package *installedPackage = m_database->getPackage(dep);

        // if package is already installed, ignore it
        if (installedPackage && installedPackage->version() == package->version()) 
            continue;

        // set installed version for uninstaller - this should be set in a more central places
        if (installedPackage && package->installedVersion().isEmpty())
            package->setInstalledVersion(installedPackage->version());

        // the package is installed with a different version 
        stateType newState = installedPackage ? _Update : _Install;

        // only add package if is neither selected in main or dependency states
        if ((state == _Nothing || state == _Remove) && (depState == _Nothing || depState == _Remove))
        {
            qDebug() << __FUNCTION__ << "selected package" << package->name() << "in previous state" << state << "for" << newState;
            if (list) 
            {   
                QString name;
                if (m_packageManagerMode)
                    name = package->name();
                else
                    name = PackageInfo::baseName(package->name());
                QTreeWidgetItem * item = new QTreeWidgetItem(QStringList() << name << package->version().toString() << package->notes());
                list->addTopLevelItem(item);
            }
            dependencyStates.setState(package,FileTypes::BIN,newState);

            // set additional package types for download/install/remove
            if (m_displayMode == Developer)
            {
                if (package->hasType(FileTypes::LIB))
                    dependencyStates.setState(package,FileTypes::LIB,newState);
                if (package->hasType(FileTypes::DOC))
                    dependencyStates.setState(package,FileTypes::DOC,newState);
                if (package->hasType(FileTypes::DBG))
                    dependencyStates.setState(package,FileTypes::DBG,newState);
            }
            else if (m_displayMode == BinaryOnly)
            {
                if (package->hasType(FileTypes::DBG) && m_installDebugPackages)
                    dependencyStates.setState(package,FileTypes::DBG,_Install);
            }
        }
    }
    return true;
}

bool isMarkedForDownload ( Package *pkg,FileTypes::Type type )
{
    stateType state = packageStates.getState ( pkg, type );
    stateType depState = dependencyStates.getState ( pkg, type );
    bool result = state == _Install || state == _Update || depState == _Install || depState == _Update;
    if (Settings::hasDebug ( "InstallerEngineGui" ) && result)
        qDebug() << __FUNCTION__ << "select package for download" << pkg->name() << type;
    return result;
}

bool isMarkedForInstall ( Package *pkg,FileTypes::Type type )
{
    stateType state = packageStates.getState ( pkg, type );
    stateType depState = dependencyStates.getState ( pkg, type );
    bool result = state == _Install || state == _Update || depState == _Install || depState == _Update;
    if (Settings::hasDebug ( "InstallerEngineGui" ) && result)
        qDebug() << __FUNCTION__ << "select package for installation" << pkg->name() << type;
    return result;
}

bool isMarkedForRemoval ( Package *pkg,FileTypes::Type type )
{
    stateType state = packageStates.getState ( pkg, type );
    stateType depState = dependencyStates.getState ( pkg, type );
    bool result = state == _Remove || state == _Update || depState == _Remove || depState == _Update;;
    if (Settings::hasDebug ( "InstallerEngineGui" ) && result)
        qDebug() << __FUNCTION__ << "select package for removal" << pkg->name() << type;
    return result;
}

InstallerEngineGui::InstallerEngineGui (QWidget *parent)
        : InstallerEngine ( parent ), m_parent(parent), m_packageManagerMode(false)
{
    InstallerUpdate &iu = InstallerUpdate::instance();
    if (iu.isUpdateAvailable()) {
        if (InstallerDialogs::instance().newInstallerAvailable()) {
            if (iu.fetch())
                iu.run();
            else 
                InstallerDialogs::instance().newInstallerDownloadError();
        }
    }
}

bool InstallerEngineGui::init()
{
    m_displayMode = Settings::instance().isPackageManagerMode() ? Single : BinaryOnly;

    if (!initGlobalConfig())
        return false;
    
    if (isInstallerVersionOutdated())
        InstallerDialogs::instance().installerOutdated();

    return initPackages();
    /// @TODO add updates to category cache
}

void InstallerEngineGui::reload()
{
    m_displayMode = Settings::instance().isPackageManagerMode() ? Developer : BinaryOnly;
    packageStates.clear();
    dependencyStates.clear();
    InstallerEngine::reload();
}
void InstallerEngineGui::unselectAllPackages()
{
    packageStates.clear();
    dependencyStates.clear();
}

void InstallerEngineGui::selectAllPackagesForRemoval()
{
    FileTypes::Type type = FileTypes::BIN;
    stateType newState = _Remove;
    Q_FOREACH(const Package *installed,m_database->packages())
    {
        if (installed->hasType(type))
            packageStates.setState(installed,type,newState);
        // set additional package types for download/install/remove
        if (type == FileTypes::BIN && m_displayMode == Developer)
        {
            if (installed->hasType(FileTypes::LIB))
                packageStates.setState(installed,FileTypes::LIB,newState);
            if (installed->hasType(FileTypes::DOC))
                packageStates.setState(installed,FileTypes::DOC,newState);
            if (installed->hasType(FileTypes::DBG))
                packageStates.setState(installed,FileTypes::DBG,newState);
        }
        else if (type == FileTypes::BIN && m_displayMode == BinaryOnly)
        {
            if (installed->hasType(FileTypes::DBG))
                packageStates.setState(installed,FileTypes::DBG,newState);
        }
        m_packageResources->addPackage(*installed);
    }
    qDebug() << packageStates;
}

void InstallerEngineGui::selectPackagesForReinstall()
{
    qWarning() << "has to be implemented";
}

bool InstallerEngineGui::downloadPackageItem(Package *pkg, FileTypes::Type type )
{
    bool all = false; //isMarkedForInstall(pkg,FileTypes::ALL);
    if ( !isMarkedForDownload ( pkg,type ) )
        return true;

    while (!m_canceled) {
        if (pkg->downloadItem ( type ))
            return true;
        if(Downloader::instance()->result() == Downloader::Aborted)
            return false;
        QMessageBox::StandardButton result = InstallerDialogs::instance().downloadFailed(pkg->getUrl(type).toString(), pkg->error());
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

bool InstallerEngineGui::downloadPackages ( const QString &category )
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
        if (!downloadPackageItem(pkg,FileTypes::BIN))
            return false;
        if (!downloadPackageItem(pkg,FileTypes::LIB))
            return false;
        if (!downloadPackageItem(pkg,FileTypes::DOC))
            return false;
        if (!downloadPackageItem(pkg,FileTypes::SRC))
            return false;
        if (!downloadPackageItem(pkg,FileTypes::DBG))
            return false;
    }
    return true;
}

bool InstallerEngineGui::removePackages ( const QString &category )
{
    QList<Package*> list = packageStates.packages(m_database);
    Q_FOREACH ( Package *pkg, dependencyStates.packages(m_database) )
        list.append(pkg);
    m_installer->progress()->setPackageCount(list.size());
    int i = 0;
    m_removedPackages = 0;    

    if (list.size() > 0) 
    {
        if (isAnyKDEProcessRunning())
        {
            if (InstallerDialogs::instance().confirmKillKDEAppsDialog())
                killAllKDEApps();
            else 
                return false;
        }
    }

    Q_FOREACH ( Package *pkg, list ) {
        if ( !pkg )
            continue;
        if (m_canceled)
            return false;

        m_installer->progress()->setPackageNumber(i++);
        bool all = false; //isMarkedForRemoval(pkg,FileTypes::ALL);
        if ( all || isMarkedForRemoval ( pkg,FileTypes::BIN ) )
            pkg->removeItem ( m_installer, FileTypes::BIN );
        if (m_canceled)
            return false;
        if ( all || isMarkedForRemoval ( pkg,FileTypes::LIB ) )
            pkg->removeItem ( m_installer, FileTypes::LIB );
        if (m_canceled)
            return false;
        if ( all || isMarkedForRemoval ( pkg,FileTypes::DOC ) )
            pkg->removeItem ( m_installer, FileTypes::DOC );
        if (m_canceled)
            return false;
        if ( all || isMarkedForRemoval ( pkg,FileTypes::SRC ) )
            pkg->removeItem ( m_installer, FileTypes::SRC );
        if (m_canceled)
            return false;
        if ( all || isMarkedForRemoval ( pkg,FileTypes::DBG ) )
            pkg->removeItem ( m_installer, FileTypes::DBG );
        m_removedPackages++;
    }
    return true;
}

bool InstallerEngineGui::installPackages ( const QString &_category )
{
    QList<Package*> list = packageStates.packages(m_packageResources);
    Q_FOREACH ( Package *pkg, dependencyStates.packages(m_packageResources) )
        list.append(pkg);
    m_installer->progress()->setPackageCount(list.size());
    int i = 0; 
    m_installedPackages = 0;

    if (list.size() > 0 && m_removedPackages == 0)  
    {
        if (isAnyKDEProcessRunning()) 
        {
            if (InstallerDialogs::instance().confirmKillKDEAppsDialog())
                killAllKDEApps();
            else 
                return false;
        }
    }
            
    Q_FOREACH ( Package *pkg, list ) {
        if ( !pkg )
            continue;
        if (m_canceled)
            return false;

        m_installer->progress()->setPackageNumber(i++);
        bool all = false;//isMarkedForInstall(pkg,FileTypes::ALL);
        if ( all || isMarkedForInstall ( pkg,FileTypes::BIN ) )
            pkg->installItem ( m_installer, FileTypes::BIN );
        if (m_canceled)
            return false;
        if ( all || isMarkedForInstall ( pkg,FileTypes::LIB ) )
            pkg->installItem ( m_installer, FileTypes::LIB );
        if (m_canceled)
            return false;
        if ( all || isMarkedForInstall ( pkg,FileTypes::DOC ) )
            pkg->installItem ( m_installer, FileTypes::DOC );
        if (m_canceled)
            return false;
        if ( all || isMarkedForInstall ( pkg,FileTypes::SRC ) )
            pkg->installItem ( m_installer, FileTypes::SRC );
        if (m_canceled)
            return false;
        if ( all || isMarkedForInstall ( pkg,FileTypes::DBG ) )
            pkg->installItem ( m_installer, FileTypes::DBG );
        // @TODO: where to handle desktop icons creating
        m_installedPackages++;
    }
    return true;
}
