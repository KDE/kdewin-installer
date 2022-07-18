/****************************************************************************
**
** Copyright (C) 2008-2010 Ralf Habacker <ralf.habacker@freenet.de> 
** All rights reserved.
**
** This file is part of the KDE installer for windows
**
** SPDX-License-Identifier: GPL-2.0-only
****************************************************************************/

#ifndef ENDUSERPACKAGESELECTORPAGE_H
#define ENDUSERPACKAGESELECTORPAGE_H


#include "installwizard.h"
#include "ui_enduserpackageselectorpage.h"

class EndUserPackageSelectorPage : public InstallWizardPage
{
    Q_OBJECT

public:
    typedef enum { Application, Language, Spelling } PackageDisplayType;
    EndUserPackageSelectorPage();

    void initializePage();
    bool isComplete();
    bool validatePage();
    void cleanupPage();

protected:
    void setWidgetData();
    void setPackageDisplayType(PackageDisplayType type);
    void preSelectPackages(const QString &package);
    QTreeWidgetItem *addPackageToTree(Package *package, QTreeWidgetItem *parent);
    bool includePackage(const QString &name, PackageDisplayType displayType);


protected slots:
    void itemClicked(QTreeWidgetItem *item, int column);
    void selectAllClicked();
    
    void slotApplicationPackageButton();
    void slotLanguagePackageButton();
    void slotSpellingPackageButton();
    void slotFilterTextChanged(const QString &text);
    void slotInstallDebugPackages();

private:
    Ui::EndUserPackageSelectorPage ui;
    QStringList activeCategories;
    PackageDisplayType m_displayType;
};

#endif
