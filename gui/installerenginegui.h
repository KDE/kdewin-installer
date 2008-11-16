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

#ifndef INSTALLERENGINEGUI_H
#define INSTALLERENGINEGUI_H

#include "installerengine.h"
#include "package.h"

class InstallerEngineGui : public InstallerEngine
{
public:
    // Install Type developer means bin/doc/lib together 
    enum Type { BinaryOnly, Developer, Single  };

    InstallerEngineGui(QWidget *parent);
    /// init all package definitions 
    virtual bool init();
    /// reload all package definitions
    virtual void reload();

    void selectAllPackagesForRemoval();
    void selectPackagesForReinstall();
    void unselectAllPackages();

    void checkUpdateDependencies(QTreeWidget *list);
    bool checkRemoveDependencies(QTreeWidget *list);

    bool downloadPackages(const QString &category=QString());
    bool installPackages(const QString &category=QString());
    bool removePackages(const QString &category=QString());
    Type installMode() { return m_installMode; }

    /// @TODO: cleanup
    void setEndUserInitialState ( QTreeWidgetItem &item, Package *available, Package *installed, int column=0 );

    void setInitialState(QTreeWidgetItem &item, Package *available, Package *installed,int column=0);
    void setNextState(QTreeWidgetItem &item, Package *available, Package *installed, Package::Type type, int column);

protected:
    bool setDependencyState(Package *pkg, QTreeWidget *list);

private:
    Type    m_installMode;
    QWidget *m_parent;
    bool downloadPackageItem(Package *pkg, Package::Type type );
};

#endif
