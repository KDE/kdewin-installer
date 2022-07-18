/****************************************************************************
**
** Copyright (C) 2008 Ralf Habacker <ralf.habacker@freenet.de> 
** All rights reserved.
**
** This file is part of the KDE installer for windows
**
** SPDX-License-Identifier: GPL-2.0-only
****************************************************************************/

#ifndef PACKAGESELECTORPAGE_H
#define PACKAGESELECTORPAGE_H

class QTabWidget;
class QTextEdit;
class QLineEdit;

#include "installwizard.h"
//#include "ui_packageselectorpage.h"

// must be global
extern QTreeWidget *tree;
extern QTreeWidget *leftTree;

class PackageSelectorPage : public InstallWizardPage
{
    Q_OBJECT

public:
    PackageSelectorPage();

    void initializePage();
    bool isComplete();
    bool validatePage();
    void cleanupPage();

public slots:
    void itemClicked(QTreeWidgetItem *item, int column);
    void on_leftTree_itemClicked(QTreeWidgetItem *item, int column);

    void installDirChanged(const QString &dir);
    void slotCompilerTypeChanged(void);
    void slotFilterTextChanged(const QString &text);

protected: 
    void updatePackageInfo(const Package *availablePackage, const Package *installedPackage);
    void setLeftTreeStatus();
    void setLeftTreeData();
    void setWidgetData( QString categoryName=QString());


private:
    QTabWidget *packageInfo;
    QLabel *categoryInfo;
    QLineEdit *filterEdit;
};

#endif
