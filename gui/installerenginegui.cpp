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
#include <QFlags>

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
//#include "database.h"

#include "packagestates.h"

/// holds the package selection and icon states
PackageStates packageStates;

// Column definitions in package list tree widget
const int NameColumn = 0;
const int VersionColumn = 1;
const int ALLColumn = 2;
const int BINColumn = 3;
const int LIBColumn = 4;
const int DOCColumn = 5;
const int SRCColumn = 6;
const int NotesColumn = 7;
const int ColumnCount = 9;

int typeToColumn(Package::Type type)
{
    switch (type)
    {
        case Package::BIN : return BINColumn;
        case Package::LIB : return LIBColumn;
        case Package::DOC : return DOCColumn;
        case Package::SRC : return SRCColumn;
        default: return ALLColumn;
    }    
}

Package::Type columnToType(int column)
{
    switch (column)
    {
        case BINColumn: return Package::BIN;
        case LIBColumn: return Package::LIB; 
        case DOCColumn: return Package::DOC; 
        case SRCColumn: return Package::SRC; 
        case ALLColumn: return Package::ALL;
        default : return Package::NONE;
    }    
}

enum iconType {_install, _autoinstall,_keepinstalled, _update, _remove, _nothing, _disable}; 

void setIcon(QTreeWidgetItem &item, Package::Type type, iconType action)
{
#ifndef DISABLE_ICONS
    static QIcon *ai;
    static QIcon *ii;
    static QIcon *ki;
    static QIcon *ni;
    static QIcon *id;
    static QIcon *dl;
    static QIcon *up;

    if (!ii) 
    {
        ai = new QIcon(":/images/autoinstall.xpm");
        ii = new QIcon(":/images/install.xpm");
        ki = new QIcon(":/images/keepinstalled.xpm");
        ni = new QIcon(":/images/noinst.xpm");
        id = new QIcon(":/images/install_disabled.xpm");
        dl = new QIcon(":/images/del.xpm");
        up = new QIcon(":/images/update.xpm");
    }
#endif
    int column = typeToColumn(type);

#ifndef DISABLE_ICONS
    switch(action)
    {
        case _autoinstall: item.setIcon(column,*ai); return;
        case _install: item.setIcon(column,*ii); return;
        case _keepinstalled: item.setIcon(column,*ki); return;
        case _remove:  item.setIcon(column,*dl); return;
        case _update:  item.setIcon(column,*up); return;
        case _nothing: item.setIcon(column,*ni); return;
        case _disable: item.setIcon(column,*id); return;
    }
#else
    switch(action)
    {
        case _autoinstall: item.setText(column,"-I-"); return;
        case _install: item.setText(column,"-I-"); return;
        case _keepinstalled: item.setText(column,"-i-"); return;
        case _remove:  item.setText(column,"-R-"); return;
        case _update:  item.setText(column,"-U-"); return;
        case _nothing: item.setText(column,"---"); return;
        case _disable: item.setText(column,""); return;
    }
#endif
    // FIXME: does not work, don't know how to set the icon size 
    // item.icon(column).setIconSize(QSize(22,22));
}

enum actionType { _initial, _next, _deps, _sync}; 

/** 
 @deprecated  use set...State
*/ 
static void setState(QTreeWidgetItem &item, const Package *pkg, int column, actionType action, int syncColumn=0)
{
    Package::Type type = columnToType(column);

    if(!pkg)
        return;
    if (type != Package::ALL && !pkg->hasType(type)) 
    {
        setIcon(item,type,_disable);
        return; 
    }

    switch (action)
    {
        case _initial: 
        {
            stateType state = packageStates.getState(pkg->name(),pkg->version(),type);

            switch(state) 
            {
                case _Install: 
                    if (!pkg->isInstalled(type))
                    {
                        setIcon(item,type,_install);
                        packageStates.setState(pkg->name(),pkg->version(),type,_Install);
                    }
                    break;
                case _Nothing: 
                    if (pkg->isInstalled(type))
                    {
                        setIcon(item,type,_keepinstalled );
                        packageStates.setState(pkg->name(),pkg->version(),type,_Nothing);
                    }
                    else 
                    {
                        setIcon(item,type,_nothing);
                        packageStates.setState(pkg->name(),pkg->version(),type,_Nothing);
                    }
                    break;
                case _Remove:
                    if (pkg->isInstalled(type))
                    {
                        setIcon(item,type,_remove);
                        packageStates.setState(pkg->name(),pkg->version(),type,_Remove);
                    }
                    else 
                    {
                        setIcon(item,type,_nothing);
                        packageStates.setState(pkg->name(),pkg->version(),type,_Nothing);
                    }
                    break;
            }                       
            break;
        }
        
        // enter next state depending on current state 
        case _next: 
        {
            stateType state = packageStates.getState(pkg->name(),pkg->version(),type);

            switch(state)
            {
                case _Nothing:  
                    if (pkg->isInstalled(type) || 
                        (type == Package::ALL 
                         && (!pkg->hasType(Package::BIN) || pkg->hasType(Package::BIN) && pkg->isInstalled(Package::BIN)) 
                         && (!pkg->hasType(Package::LIB) || pkg->hasType(Package::LIB) && pkg->isInstalled(Package::LIB)) 
                         && (!pkg->hasType(Package::DOC) || pkg->hasType(Package::DOC) && pkg->isInstalled(Package::DOC))))
                    { 
                        setIcon(item,type,_remove);
                        packageStates.setState(pkg->name(),pkg->version(),type,_Remove);
                    }
                    else
                    {
                        setIcon(item,type,_install);
                        packageStates.setState(pkg->name(),pkg->version(),type,_Install);
                    }
                    break;

                case _Remove:  
                    if (pkg->isInstalled(type))
                    {
                        setIcon(item,type,_keepinstalled );
                        packageStates.setState(pkg->name(),pkg->version(),type,_Nothing);
                    }
                    else
                    {
                        setIcon(item,type,_nothing);
                        packageStates.setState(pkg->name(),pkg->version(),type,_Nothing);
                    }
                    break;

                case _Install:  
                    if (pkg->isInstalled(type))
                    {
                        setIcon(item,type,_install);
                        packageStates.setState(pkg->name(),pkg->version(),type,_Install);
                    }
                    else
                    {
                        setIcon(item,type,_nothing);  
                        packageStates.setState(pkg->name(),pkg->version(),type,_Nothing);
                    }
                    break;
            }
            break;
        }
        // handle dependeny selecting 
        case _deps: 
        {
            if (pkg->isInstalled(type))
            {
            }
            else
            {
                // FIXME: should be _autoinstall, but then the main package is using this icon too 
                setIcon(item,type,_install);
                packageStates.setState(pkg->name(),pkg->version(),type,_Install);
            }
            break;
        }
        case _sync: 
        {
            stateType state = packageStates.getState(pkg->name(),pkg->version(),columnToType(syncColumn));

            switch(state) 
            {
                case _Install: 
                    if (!pkg->isInstalled(type))
                    {
                        setIcon(item,type,_install);
                        packageStates.setState(pkg->name(),pkg->version(),type,_Install);
                    }
                    break;
                case _Nothing: 
                    if (!pkg->isInstalled(type))
                    {
                        setIcon(item,type,_nothing);
                        packageStates.setState(pkg->name(),pkg->version(),type,_Nothing);
                    }
                    break;
                case _Remove:
                    if (pkg->isInstalled(type))
                    {
                        setIcon(item,type,_remove);
                        packageStates.setState(pkg->name(),pkg->version(),type,_Remove);
                    }
                    else 
                    {
                        setIcon(item,type,_nothing);
                        packageStates.setState(pkg->name(),pkg->version(),type,_Nothing);
                    }
                    break;
            }
            break;
        }
    }        
}

static void setInitialState(QTreeWidgetItem &item, const Package *pkg, int column)
{
    setState(item,pkg,column,_initial);
}

static void setNextState(QTreeWidgetItem &item, const Package *pkg, int column)
{
    setState(item,pkg,column,_next);
}

static void setDependencyState(QTreeWidgetItem &item, const Package *pkg, int column)
{
    setState(item,pkg,column,_deps);
}


bool isMarkedForInstall(Package *pkg,Package::Type type)
{
    stateType state = packageStates.getState(pkg->name(),pkg->version(),type);
    return state == _Install;
}

bool isMarkedForRemoval(Package *pkg,Package::Type type)
{
    stateType state = packageStates.getState(pkg->name(),pkg->version(),type);
    return state == _Remove;
}

InstallerEngineGui::InstallerEngineGui(DownloaderProgress *progressBar,InstallerProgress *instProgressBar)
: InstallerEngine(progressBar,instProgressBar)
{
    m_installMode = Single;
}

void InstallerEngineGui::setLeftTreeData(QTreeWidget *tree)
{
    tree->clear();
    QStringList labels;
    QList<QTreeWidgetItem *> items;
    labels << tr("pakgetgroups") << tr("description");
    tree->setColumnCount(2);
    tree->setHeaderLabels(labels);
    QList<QTreeWidgetItem *> categoryList;

    // adding top level items
    QStringList names;
    names << tr("all") << tr("all packages");
    QTreeWidgetItem *category = new QTreeWidgetItem((QTreeWidget*)0, names);
    QTreeWidgetItem *firstItem = category;
    categoryList.append(category);

    names.clear();
    names << tr("mingw") << tr("packages usable by win32 gcc");
    category = new QTreeWidgetItem((QTreeWidget*)0, names);
    categoryList.append(category);

    names.clear();
    names << tr("msvc") << tr("packages usable by MS VC");
    category = new QTreeWidgetItem((QTreeWidget*)0, names);
    categoryList.append(category);

    QList <PackageList *>::ConstIterator k = m_packageListList.constBegin();
    for ( ; k != m_packageListList.constEnd(); ++k)
    {
        if ((*k)->packageList().size() == 0)
            continue;
        QStringList names;
        names << (*k)->Name();
        names << (*k)->notes();
        QTreeWidgetItem *category = new QTreeWidgetItem((QTreeWidget*)0, names);
        category->setToolTip(0,(*k)->notes());
        categoryList.append(category);
    }
    tree->insertTopLevelItems(0,categoryList);
    tree->expandAll();
    tree->sortItems(0,Qt::AscendingOrder);
    // FIXME: don't know how to select an item as done with mouse 
    tree->setCurrentItem(firstItem);
    firstItem->setSelected(true);
    for (int i = 0; i < tree->columnCount(); i++)
        tree->resizeColumnToContents(i);
}
extern QTreeWidget *tree;

void InstallerEngineGui::on_leftTree_itemClicked(QTreeWidgetItem *item, int column)
{
    setPageSelectorWidgetData(tree,item->text(0));
}

void InstallerEngineGui::setPageSelectorWidgetData(QTreeWidget *tree, QString categoryName)
{
    tree->clear();
    QStringList labels;
    QList<QTreeWidgetItem *> items;
    // QTreeWidgetItem *item;
    QString allToolTip("select this checkbox to install/remove/update binaries, headers, import libraries and docs of this package");
    QString binToolTip("select this checkbox to install/remove/update the binaries of this package");
    QString libToolTip("select this checkbox to install/remove/update header and import libraries of this package");
    QString docToolTip("select this checkbox to install/remove/update the documentation of this package");
    QString srcToolTip("select this checkbox to install/remove/update the source of this package");

    labels
    << tr("Package")
    << tr("Version");
    switch (m_installMode) 
    {
        case Developer: 
            labels
            << ""
            << tr("bin/lib/doc")
            << ""
            << "";
            break;

        case EndUser: 
            labels
            << ""
            << tr("bin/doc")
            << ""
            << "";
            break;

        case Single: 
            labels
            << tr("all")
            << tr("bin")
            << tr("lib")
            << tr("doc");
            break;
    }
    labels
    << tr("src")
    << tr("package notes")
    << tr("news");

    tree->setColumnCount(ColumnCount);
    tree->setHeaderLabels(labels);
    // see http://lists.trolltech.com/qt-interest/2006-06/thread00441-0.html
    // and Task Tracker Entry 106731
    //tree->setAlignment(Center);

    // adding top level items
    QList<QTreeWidgetItem *> categoryList;

#ifdef DEBUG
    qDebug() << "adding categories size:" << m_globalConfig->sites()->size();
#endif

    QList <PackageList *>::ConstIterator k = m_packageListList.constBegin();
    for ( ; k != m_packageListList.constEnd(); ++k)
    {
        if ((*k)->packageList().size() == 0)
            continue;
        if (!categoryName.isEmpty() && categoryName != "mingw" && categoryName != "msvc" 
            && categoryName != "all" && categoryName != (*k)->Name())
            continue;
        QStringList names;
        names << (*k)->Name();
        names << "";
        names << "";
        names << "";
        names << "";
        names << "";
        names << "";
        names << "";
//        names << (*k)->notes();
        QTreeWidgetItem *category = new QTreeWidgetItem((QTreeWidget*)0, names);
        category->setToolTip(0,(*k)->notes());
        categoryList.append(category);

        // adding sub items
        QList<Package*>::ConstIterator i = (*k)->packageList().constBegin();
        for ( ; i != (*k)->packageList().constEnd(); ++i)
        {
            Package *pkg = *i;
            if (categoryName == "mingw" 
                && (pkg->name().toLower().contains("msvc") || pkg->notes().toLower().contains("msvc")))
                continue;
            else if (categoryName == "msvc" 
                && (pkg->name().toLower().contains("mingw") || pkg->notes().toLower().contains("mingw")))
                continue;
            QStringList data;
            data << pkg->name()
                 << pkg->version()
                 << QString();
            QTreeWidgetItem *item = new QTreeWidgetItem(category, data);
            if (m_installMode == Single)
                setInitialState(*item,pkg,ALLColumn);

            setInitialState(*item,pkg,BINColumn);
            if (m_installMode != Developer )
            {
                setInitialState(*item,pkg,LIBColumn);
                setInitialState(*item,pkg,DOCColumn);
            }
            setInitialState(*item,pkg,SRCColumn);
            item->setText(NotesColumn, pkg->notes());
            // FIXME 
            //item->setText(8, m_globalConfig->news()->value(pkg->name()+"-"+pkg->version()));
            item->setToolTip ( ALLColumn, allToolTip);
            item->setToolTip ( BINColumn, binToolTip);
            item->setToolTip ( LIBColumn, libToolTip);
            item->setToolTip ( DOCColumn, docToolTip);
            item->setToolTip ( SRCColumn, srcToolTip);
        }
    }
    tree->insertTopLevelItems(0,categoryList);
    tree->expandAll();
    tree->sortItems(0,Qt::AscendingOrder);
    for (int i = 0; i < tree->columnCount(); i++)
        tree->resizeColumnToContents(i);
}


/** 
  select depending packages of package pkg
  @param pkg package of which the depending packages should be selected
  @param type Type of selection: ALL or BIN
  @todo add support for indirect dependencies 
*/
void InstallerEngineGui::setDependencies(Package *pkg, Package::Type type)
{
    const QStringList &deps = pkg->deps();

    qDebug() << deps.join(" ");    
    for (int i = 0; i < deps.size(); ++i)
    {  
        QString dep = deps.at(i);
        Package *depPkg = getPackageByName(dep);
        if (!depPkg)
            continue;
        //setDependencies(depPkg,type);

        // find item in display list
        QList<QTreeWidgetItem *> items = tree->findItems(deps.at(i),Qt::MatchFixedString | Qt::MatchRecursive);
        qDebug() << __FUNCTION__ << "found " << items.size() << "displayed items";
        for (int j = 0; j < items.size(); ++j) 
        {
            qDebug() << items.at(j);
            QTreeWidgetItem * depItem = static_cast<QTreeWidgetItem*>(items[j]);
            /// the dependency is only for bin package and one way to switch on
            if (m_installMode == Developer)
            {
                setState(*depItem,depPkg,BINColumn,_deps);
                setState(*depItem,depPkg,LIBColumn,_deps);
                setState(*depItem,depPkg,DOCColumn,_deps);
            }
            else if (m_installMode == EndUser)
            {
                setState(*depItem,depPkg,BINColumn,_deps);
                // lib is excluded
                setState(*depItem,depPkg,DOCColumn,_deps);
            }
            else if (m_installMode == Single)
            {    
                if (type == Package::ALL)
                {
                    setState(*depItem,depPkg,BINColumn,_deps);
                    setState(*depItem,depPkg,LIBColumn,_deps);
                    setState(*depItem,depPkg,DOCColumn,_deps);
                }
                else
                    setState(*depItem,depPkg,BINColumn,_deps);
            }
        }
    }
}
// old version deprecated6
#if 0
void InstallerEngineGui::setDependencies(Package *pkg, Package::Type type)
{
    const QStringList &deps = pkg->deps();
    
    qDebug() << deps.join(" ");    
    for (int i = 0; i < deps.size(); ++i)
    {  
        QString dep = deps.at(i);
        QList<QTreeWidgetItem *> items = tree->findItems(deps.at(i),Qt::MatchFixedString | Qt::MatchRecursive);
        qDebug() << items.size();
        for (int j = 0; j < items.size(); ++j) 
        {
               qDebug() << items.at(j);
            QTreeWidgetItem * depItem = static_cast<QTreeWidgetItem*>(items[j]);
            /// the dependency is only for bin package and one way to switch on
            Package *depPkg = getPackageByName(dep);
    
            if (m_installMode == Developer)
            {
                setState(*depItem,depPkg,BINColumn,_deps);
                setState(*depItem,depPkg,LIBColumn,_deps);
                setState(*depItem,depPkg,DOCColumn,_deps);
            }
            else if (m_installMode == EndUser)
            {
                setState(*depItem,depPkg,BINColumn,_deps);
                // lib is excluded
                setState(*depItem,depPkg,DOCColumn,_deps);
            }
            else if (m_installMode == Single)
            {    
                if (type == Package::ALL)
                {
                    setState(*depItem,depPkg,BINColumn,_deps);
                    setState(*depItem,depPkg,LIBColumn,_deps);
                    setState(*depItem,depPkg,DOCColumn,_deps);
                }
                else
                    setState(*depItem,depPkg,BINColumn,_deps);
            }
        }
    }    
 }
#endif

void InstallerEngineGui::itemClickedPackageSelectorPage(QTreeWidgetItem *item, int column)
{
    if (column < ALLColumn)
        return;

    Package *pkg = getPackageByName(item->text(NameColumn),item->text(VersionColumn));
    if (!pkg)
        return;
    if (m_installMode == Single && column == ALLColumn)
    {
       setState(*item,pkg,ALLColumn,_next);
       setState(*item,pkg,BINColumn,_sync,ALLColumn);
       setState(*item,pkg,LIBColumn,_sync,ALLColumn);
       setState(*item,pkg,DOCColumn,_sync,ALLColumn);
       setState(*item,pkg,SRCColumn,_sync,ALLColumn);
    }
    else if (m_installMode == Developer && column == BINColumn)
    {
        setState(*item,pkg,BINColumn,_next);
        setState(*item,pkg,LIBColumn,_next);
        setState(*item,pkg,DOCColumn,_next);
    }
    else if (m_installMode == EndUser && column == BINColumn)
    {
        setState(*item,pkg,BINColumn,_next);
        // lib excluded
        setState(*item,pkg,DOCColumn,_next);
    }
    else
        setState(*item,pkg,column,_next);

    // select depending packages in case all or bin is selected
    if (column == ALLColumn) 
        setDependencies(pkg,Package::ALL);
    else if (column == BINColumn) 
        setDependencies(pkg,Package::BIN);
}

bool InstallerEngineGui::downloadPackages(QTreeWidget *tree, const QString &category)
{
    // FIXME: uses packageLists instead of widget selection 
    for (int i = 0; i < tree->topLevelItemCount(); i++)
    {
        QTreeWidgetItem *item = static_cast<QTreeWidgetItem*>(tree->topLevelItem(i));
		if (Settings::hasDebug("InstallerEngineGui::downloadPackages"))
			qDebug() << __FUNCTION__ << " " << item->text(0);
        if (category.isEmpty() || item->text(0) == category)
        {
            PackageList *packageList = getPackageListByName(item->text(0));
            if (!packageList)
            {
                qDebug() << __FUNCTION__ << " packagelist for " << item->text(0) << " not found";
                continue;
            }
            for (int j = 0; j < item->childCount(); j++)
            {
                QTreeWidgetItem *child = static_cast<QTreeWidgetItem*>(item->child(j));
//              qDebug("%s %s %d",child->text(0).toAscii().data(),child->text(1).toAscii().data(),child->checkState(2));
                Package *pkg = packageList->getPackage(child->text(NameColumn),child->text(VersionColumn).toAscii());
                if (!pkg)
                    continue;
				if (Settings::hasDebug("InstallerEngineGui"))
					pkg->dump("downloadPackages");

				bool all = isMarkedForInstall(pkg,Package::ALL);
                if (all || isMarkedForInstall(pkg,Package::BIN))
                    pkg->downloadItem(m_downloader, Package::BIN);
                if (all || isMarkedForInstall(pkg,Package::LIB))
                    pkg->downloadItem(m_downloader, Package::LIB);
                if (all || isMarkedForInstall(pkg,Package::DOC))
                    pkg->downloadItem(m_downloader, Package::DOC);
                if (all || isMarkedForInstall(pkg,Package::SRC))
                    pkg->downloadItem(m_downloader, Package::SRC);
            }
        }
    }
    return true;
}

bool InstallerEngineGui::removePackages(QTreeWidget *tree, const QString &category)
{
    // FIXME: uses packageLists instead of widget selection 
    for (int i = 0; i < tree->topLevelItemCount(); i++)
    {
        QTreeWidgetItem *item = static_cast<QTreeWidgetItem*>(tree->topLevelItem(i));
        if (category.isEmpty() || item->text(0) == category)
        {
            PackageList *packageList = getPackageListByName(item->text(0));
            if (!packageList)
            {
                qDebug() << __FUNCTION__ << " packagelist for " << item->text(0) << " not found";
                continue;
            }
            for (int j = 0; j < item->childCount(); j++)
            {
                QTreeWidgetItem *child = static_cast<QTreeWidgetItem*>(item->child(j));
                Package *pkg = packageList->getPackage(child->text(NameColumn),child->text(VersionColumn).toAscii());
                if (!pkg)
                    continue;
				if (Settings::hasDebug("InstallerEngineGui"))
					pkg->dump("removePackages");
                bool all = false; //isMarkedForRemoval(pkg,Package::ALL);
                if (all | isMarkedForRemoval(pkg,Package::BIN))
                    pkg->removeItem(m_installer, Package::BIN);
                if (all | isMarkedForRemoval(pkg,Package::LIB))
                    pkg->removeItem(m_installer, Package::LIB);
                if (all | isMarkedForRemoval(pkg,Package::DOC))
                    pkg->removeItem(m_installer, Package::DOC);
                if (all | isMarkedForRemoval(pkg,Package::SRC))
                    pkg->removeItem(m_installer, Package::SRC);
            }
        }
    }
    return true;
}

bool InstallerEngineGui::installPackages(QTreeWidget *tree,const QString &_category)
{
    // FIXME: uses packageLists instead of widget selection 
    for (int i = 0; i < tree->topLevelItemCount(); i++)
    {
        QTreeWidgetItem *item = static_cast<QTreeWidgetItem*>(tree->topLevelItem(i));
        QString category = item->text(0); 
#ifdef DEBUG
        qDebug() << __FUNCTION__ << " " << category;
#endif
        if (_category.isEmpty() || category == _category)
        {
            PackageList *packageList = getPackageListByName(category);
            if (!packageList)
            {
                qDebug() << __FUNCTION__ << " packagelist for " << category << " not found";
                continue;
            }
            for (int j = 0; j < item->childCount(); j++)
            {
                QTreeWidgetItem *child = static_cast<QTreeWidgetItem*>(item->child(j));
                QString pkgName = child->text(NameColumn);
                Package *pkg = packageList->getPackage(pkgName,child->text(VersionColumn).toAscii());
                if (!pkg)
                    continue;
				if (Settings::hasDebug("InstallerEngineGui"))
					pkg->dump("installPackages");
                bool all = isMarkedForInstall(pkg,Package::ALL);
                if (all || isMarkedForInstall(pkg,Package::BIN))
                    pkg->installItem(m_installer, Package::BIN);
                if (all || isMarkedForInstall(pkg,Package::LIB))
                    pkg->installItem(m_installer, Package::LIB);
                if (all || isMarkedForInstall(pkg,Package::DOC))
                    pkg->installItem(m_installer, Package::DOC);
                if (all || isMarkedForInstall(pkg,Package::SRC))
                    pkg->installItem(m_installer, Package::SRC);
            }
        }
    }
    return true;
}

