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


//#include "downloader.h"
#include "installer.h"
#include "downloaderprogress.h"
#include "package.h"
#include "packagelist.h"
#include "globalconfig.h"
#include "database.h"
#include "packagestates.h"
#include "installerdialogs.h"

#include <windows.h>

#include <QtGui/QTreeWidget>

/// holds the package selection and icon states
PackageStates packageStates;

/// holds the package dependency state
PackageStates dependencyStates;

// from packageselectorpage.cpp
int typeToColumn ( Package::Type type );

enum iconType {_install, _autoinstall,_keepinstalled, _update, _remove, _nothing, _disable};


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

    if ( ii.isNull() ) {
        ai = QIcon ( ":/autoinstall.xpm" );
        ii = QIcon ( ":/install.xpm" );
        ki = QIcon ( ":/keepinstalled.xpm" );
        ni = QIcon ( ":/noinst.xpm" );
        id = QIcon ( ":/install_disabled.xpm" );
        dl = QIcon ( ":/del.xpm" );
        up = QIcon ( ":/update.xpm" );
    }
#endif

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
  setIcon( item, typeToColumn(type), t );
}

static void setIcon ( QTreeWidgetItem &item, int column, stateType state, iconType defType )
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
  setIcon( item, column, t );
}

static void setIcon ( QTreeWidgetItem &item, Package::Type type, iconType action )
{
    setIcon(item,typeToColumn ( type ), action);
}
    
void InstallerEngineGui::setEndUserInitialState ( QTreeWidgetItem &item, Package *available, Package *installed, int column )
{
    if (installed && available->version() != installed->version())
    {
        if (installed->isInstalled(Package::BIN))
        {
            setIcon(item,column,packageStates.getState(installed,Package::BIN),_update);
            packageStates.setState(available,Package::BIN,_Update);
        }
    }
    else if (installed)
    {
        if (installed->isInstalled(Package::BIN))
            setIcon(item,column,packageStates.getState(installed,Package::BIN),_keepinstalled);
    }
    else if (available)
    {
        if (available->hasType(Package::BIN))
            setIcon(item,column,packageStates.getState(available,Package::BIN),_nothing);
    }
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

void InstallerEngineGui::setNextState ( QTreeWidgetItem &item, Package *available, Package *installed, Package::Type type, int column )
{
    if (type == Package::NONE)
        return;

    bool isAvailable;
    bool isInstalled;

    if (type == Package::BIN && m_installMode == Developer)
    {
        isAvailable = available && (available->hasType(type) || available->hasType(Package::LIB) || available->hasType(Package::DOC));
        isInstalled = installed && (installed->isInstalled(type) || installed->hasType(Package::LIB) || installed->hasType(Package::DOC));
    }
    else if (type == Package::BIN && m_installMode == BinaryOnly)
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
    setIcon(item,column,iconState);

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
    else if (type == Package::BIN && m_installMode == BinaryOnly)
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

        stateType state = packageStates.getState(package,Package::BIN);
        stateType depState = dependencyStates.getState(package,Package::BIN);

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
                QTreeWidgetItem * item = new QTreeWidgetItem(QStringList() << package->name() << package->version().toString() << package->notes());
                list->addTopLevelItem(item);
            }
            dependencyStates.setState(package,Package::BIN,newState);

            // set additional package types for download/install/remove
            if (m_installMode == Developer)
            {
                if (package->hasType(Package::LIB))
                    dependencyStates.setState(package,Package::LIB,newState);
                if (package->hasType(Package::DOC))
                    dependencyStates.setState(package,Package::DOC,newState);
            }
            else if (m_installMode == BinaryOnly)
            {
                ;//if (package->hasType(Package::DOC))
                 //   dependenciesStates.setState(package,Package::DOC,_Install);
            }
        }
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
    m_installMode = Settings::instance().isPackageManagerMode() ? Single : BinaryOnly;

    initGlobalConfig();
    if (m_globalConfig->installerUpdate().isUpdateAvailable()) {
        if (InstallerDialogs::instance().newInstallerAvailable())
            if (m_globalConfig->installerUpdate().fetch())
                m_globalConfig->installerUpdate().run();
            else 
                InstallerDialogs::instance().newInstallerDownloadError();
    }
    else if (isInstallerVersionOutdated())
        InstallerDialogs::instance().installerOutdated();
    return initPackages();

    /// @TODO add updates to category cache
}

void InstallerEngineGui::reload()
{
    m_installMode = Settings::instance().isPackageManagerMode() ? Developer : BinaryOnly;
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
    Package::Type type = Package::BIN;
    stateType newState = _Remove;
    Q_FOREACH(const Package *installed,m_database->packages())
    {
        if (installed->hasType(type))
            packageStates.setState(installed,type,newState);
        // set additional package types for download/install/remove
        if (type == Package::BIN && m_installMode == Developer)
        {
            if (installed->hasType(Package::LIB))
                packageStates.setState(installed,Package::LIB,newState);
            if (installed->hasType(Package::DOC))
                packageStates.setState(installed,Package::DOC,newState);
        }
        else if (type == Package::BIN && m_installMode == BinaryOnly)
        {
            ;//if (installed->hasType(Package::DOC))
             //   packageStates.setState(available,Package::DOC,newState);
        }
        m_packageResources->addPackage(*installed);
    }
    qDebug() << packageStates;
}

void InstallerEngineGui::selectPackagesForReinstall()
{
    qWarning() << "has to be implemented";
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
            tr("The download of %1 failed with error: %2").arg(pkg->getUrl(type).toString()).arg(pkg->error()),
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

bool killAllKDEApps()
{
    QString cmd = Settings::instance().installDir() +"/bin/kdeinit4.exe";
    QStringList args = QStringList() << "--help";
    QProcess p;
    p.start(cmd, args);
    if (!p.waitForStarted()) 
    {
        qCritical() << "could not start" << cmd << args;
        return false;
    }
    if (!p.waitForFinished())
    {
        qCritical() << "failed to run" << cmd << args;
        return false;
    }
    QByteArray _stdout = p.readAllStandardOutput();
    args = QStringList() << (_stdout.contains("--shutdown") ? "--shutdown" : "--terminate");

    /// I got cases where files are not removed and resulted into "a could not remove file error on installing" 
    p.start(cmd,args);
    if (!p.waitForStarted()) 
    {
        qCritical() << "could not start" << cmd << args;
        return false;
    }
    if (!p.waitForFinished())
    {
        qCritical() << "failed to run" << cmd << args;
        return false;
    }
    qDebug() << "run" << cmd << args << "without errors"; 
    // give applications some time to really be terminated
    Sleep(1000);
    return true;
}

bool InstallerEngineGui::removePackages ( const QString &category )
{

    QList<Package*> list = packageStates.packages(m_packageResources);
    Q_FOREACH ( Package *pkg, dependencyStates.packages(m_packageResources) )
        list.append(pkg);
    m_installer->progress()->setPackageCount(list.size());
    int i = 0;
    m_removedPackages = 0;    

    if (list.size() > 0) 
        killAllKDEApps();

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
        killAllKDEApps();

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
        m_installedPackages++;
    }
    return true;
}
