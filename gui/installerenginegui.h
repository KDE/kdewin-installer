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

#ifndef INSTALLERENGINEGUI_H
#define INSTALLERENGINEGUI_H

#include "installerengine.h"
#include "downloaderprogress.h"
#include "package.h"

class QTabWidget;
class QTextEdit;

class InstallerEngineGui : public InstallerEngine
{
public:
    enum Type { EndUser, Developer, Single };

    InstallerEngineGui(QWidget *parent, DownloaderProgress *progressBar,InstallerProgress *instProgressBar);
    /// init all package definitions 
    virtual void init();
    /// reload all package definitions
    virtual void reload();

    void setPageSelectorWidgetData(QTreeWidget *tree, QString categoryName=QString());
    void itemClickedPackageSelectorPage(QTreeWidgetItem *item, int column, QTabWidget *info);
    void setLeftTreeData(QTreeWidget *tree=0);
    void on_leftTree_itemClicked(QTreeWidgetItem *item, int column, QTextEdit *info);
    void setDependencies(Package *pkg, Package::Type type);

    bool downloadPackages(QTreeWidget *tree, const QString &category=QString());
    bool installPackages(QTreeWidget *tree, const QString &category=QString());
    bool removePackages(QTreeWidget *tree, const QString &category=QString());

private:
    Type    m_installMode;
    QWidget *m_parent;
    bool downloadPackageItem(Package *pkg, Package::Type type );
    void updatePackageInfo(QTabWidget *packageInfo, const Package *availablePackage, const Package *installedPackage);



};

#endif
