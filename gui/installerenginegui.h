/****************************************************************************
**
** Copyright (C) 2005-2008 Ralf Habacker. All rights reserved.
**
** This file is part of the KDE installer for windows
**
** SPDX-License-Identifier: LGPL-2.0-only
**
****************************************************************************/

#ifndef INSTALLERENGINEGUI_H
#define INSTALLERENGINEGUI_H

#include "installerengine.h"
#include "package.h"

class QTreeWidget;
class QTreeWidgetItem;

FileTypes::Type columnToType ( int column );
int typeToColumn ( FileTypes::Type type );

extern int BINColumn;
extern int SRCColumn;
extern int DBGColumn;
extern int NotesColumn;
extern int LIBColumn;
extern int DOCColumn;
extern int ColumnCount;

class InstallerEngineGui : public InstallerEngine
{
public:

    InstallerEngineGui(QWidget *parent);
    /// init all package definitions 
    virtual bool init();
    /// reload all package definitions
    virtual void reload();

    void selectAllPackagesForRemoval();
    void selectPackagesForReinstall();
    void unselectAllPackages();

    /// @todo merge with related methods from InstallerEngine class
    void checkUpdateDependencies(QTreeWidget *list);
    /// @todo merge with related methods from InstallerEngine class
    bool checkRemoveDependencies(QTreeWidget *list);

    bool downloadPackages(const QString &category=QString());
    bool installPackages(const QString &category=QString());
    bool removePackages(const QString &category=QString());

    // display Type developer means bin/doc/lib together 
    enum DisplayType { BinaryOnly, Developer, Single  };
    DisplayType displayMode() { return m_displayMode; }
    void setDisplayMode(DisplayType mode) { m_displayMode = mode; }

    void setPackageManagerMode(bool mode) { m_packageManagerMode = mode; }
    bool packageManagerMode() { return m_packageManagerMode; }

    /// @TODO: cleanup
    void setEndUserInitialState ( QTreeWidgetItem &item, Package *available, Package *installed, int column=0 );

    void setInitialState(QTreeWidgetItem &item, Package *available, Package *installed,int column=0);
    void setNextState(QTreeWidgetItem &item, Package *available, Package *installed, FileTypes::Type type, int column, bool handleMetaPackage=false);
    bool isPackageSelected ( Package *available, FileTypes::Type type );

    void setMetaPackageState(QTreeWidgetItem &item, int column);

    /// state of install debug packages flag
    bool installDebugPackages() { return m_installDebugPackages; }
    void setInstallDebugPackages(bool state) { m_installDebugPackages = state; }
    
protected:
    /// @todo merge with related methods from InstallerEngine class
    bool setDependencyState(Package *pkg, QTreeWidget *list);

private:
    DisplayType    m_displayMode;
    QWidget *m_parent;
    bool m_packageManagerMode;
    bool m_installDebugPackages;
    
    bool downloadPackageItem(Package *pkg, FileTypes::Type type );
};

#endif
