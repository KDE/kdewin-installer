/****************************************************************************
**
** Copyright (C) 2008 Ralf Habacker <ralf.habacker@freenet.de> 
** All rights reserved.
**
** This file is part of the KDE installer for windows
**
** This file may be used under the terms of the GNU General Public
** License version 2.0 as published by the Free Software Foundation
** and appearing in the file LICENSE.GPL included in the packaging of
** this file.  Please review the following information to ensure GNU
** General Public Licensing requirements will be met:
** http://www.trolltech.com/products/qt/opensource.html
**
** If you are unsure which license is appropriate for your use, please
** review the following information:
** http://www.trolltech.com/products/qt/licensing.html or contact the
** sales department at sales@trolltech.com.
**
** This file is provided AS IS with NO WARRANTY OF ANY KIND, INCLUDING THE
** WARRANTY OF DESIGN, MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE.
**
****************************************************************************/

#include "config.h"
#include "debug.h"
#include "downloader.h"
#include "installer.h"
#include "installerprogress.h"
#include "installerenginegui.h"
#include "installerdialogs.h"
#include "package.h"
#include "packagelist.h"
#include "mirrors.h"
#include "settings.h"
#include "uninstaller.h"
#include "unpacker.h"
#include "enduserpackageselectorpage.h"
#include "packagecategorycache.h"

#include <QListWidget>
#include <QSplitter>
#include <QTextEdit>
#include <QTreeWidget>

extern InstallerEngineGui *engine;

EndUserPackageSelectorPage::EndUserPackageSelectorPage()  : InstallWizardPage(0)
{
    ui.setupUi(this);
    setTitle(windowTitle());
    if (0/* update*/) 
        setSubTitle(tr("This page shows all available updates for installed package and further available packages. "
            "You may click into the action column to exclude a package from the update, to installl further packages "
            "or to remove installed packages. If you are ready, press Next to start the update.")
        );
    else
        setSubTitle(statusTip());
}

void EndUserPackageSelectorPage::setWidgetData()
{
    QTreeWidget *tree = ui.packageList;
    tree->clear();
    QStringList labels;
    QList<QTreeWidgetItem *> items;
    QString toolTip = "select this checkbox to install or update this package";

    labels 
    << tr ( "Action" )
    << tr ( "Package" )
    << tr ( "Available" )
    << tr ( "Installed" )
    << tr ( "Package notes" )
    ;

    tree->setColumnCount ( 5 );
    tree->setHeaderLabels ( labels );
    // see http://lists.trolltech.com/qt-interest/2006-06/thread00441-0.html
    // and Task Tracker Entry 106731
    //tree->setAlignment(Center);

    // adding top level items
    QList<QTreeWidgetItem *> categoryList;
    QList <Package*> packageList;

    QStringList selectedCategories;
    Q_FOREACH(QString category, categoryCache.categories())
    {
        QStringList a = category.split(":");
        if (activeCategories.contains(a[0]))
            selectedCategories << a[0];
    }

    Q_FOREACH(QString categoryName, selectedCategories) 
    {
        // add packages which are installed but for which no config entry is there 
        Q_FOREACH(Package *instPackage, categoryCache.packages(categoryName,*engine->database())) 
        {
            Package *p = engine->packageResources()->getPackage(instPackage->name());
            if (!p)
                packageList << instPackage;
        }
    }
    
    Settings &s = Settings::instance();
    Q_FOREACH(QString categoryName, selectedCategories) 
    {
        Q_FOREACH(Package *availablePackage,categoryCache.packages(categoryName,*engine->packageResources()))
        {
            QString name = availablePackage->name();
            if ( ( categoryName == "mingw"  || s.compilerType() == Settings::MinGW )
                    && ( name.endsWith ( QLatin1String( "-msvc" ) ) || name.endsWith ( QLatin1String( "-vc90" ) ) 
                      || name.endsWith ( QLatin1String( "-mingw4" ) ) ) )
                continue;
            else if ( ( categoryName == "msvc"  || s.compilerType() == Settings::MSVC )
                      && ( name.endsWith ( QLatin1String ( "-mingw" ) ) || name.endsWith( QLatin1String( "-mingw4" ) ) ) )
                continue;
            else if ( ( categoryName == "mingw4"  || s.compilerType() == Settings::MinGW4 )
                    && ( name.endsWith ( QLatin1String( "-msvc" ) ) || name.endsWith ( QLatin1String( "-vc90" ) ) 
                      || name.endsWith ( QLatin1String( "-mingw" ) ) ) )
                continue;
            packageList << availablePackage;
        }
    }
    
    Q_FOREACH(Package *availablePackage,packageList)
    {
        QString name = availablePackage->name();
        if (m_displayType == Language && !name.contains("kde-l10n"))
            continue;
        else if (m_displayType == Spelling && !name.contains("aspell"))
            continue;
        else if (m_displayType == Application 
                && ( name.contains("aspell") 
                    || name.contains("kde-l10n") 
                    || name.contains("lib") 
                    || name.contains("runtime")
                    ) 
                )
            continue;
            
        QStringList data;
        Package *installedPackage = engine->database()->getPackage(availablePackage->name());
        Package::PackageVersion installedVersion = installedPackage ? installedPackage->installedVersion() : Package::PackageVersion();
        Package::PackageVersion availableVersion = availablePackage->version();
        availablePackage->setInstalledVersion(installedVersion);

        if (installedPackage && availableVersion == installedVersion)
            continue;
        data 
            << ""
            << PackageInfo::baseName(availablePackage->name())
            << (availableVersion != installedVersion ? availableVersion.toString() : "")
            << installedVersion.toString()
            << QString();
        QTreeWidgetItem *item = new QTreeWidgetItem ( ( QTreeWidgetItem* ) 0, data );
        // save real package name for selection code
        item->setData(0,Qt::StatusTipRole,availablePackage->name());
        engine->setEndUserInitialState( *item,availablePackage,installedPackage,0);
        item->setText ( 4, availablePackage->notes() );
        item->setToolTip ( 0, toolTip );
        categoryList.append(item);
    }
    tree->addTopLevelItems ( categoryList );
    tree->expandAll();
    tree->sortItems ( 0,Qt::AscendingOrder );
    for ( int i = 0; i < tree->columnCount(); i++ )
        tree->resizeColumnToContents ( i );
}

void EndUserPackageSelectorPage::initializePage()
{
    Settings::instance().setFirstRun(false);
    Settings::instance().setSkipBasicSettings(true);
    setSettingsButtonVisible(true);
    InstallerDialogs::instance().downloadProgressDialog(this,true,tr("Downloading Package Lists"));
    engine->init();
    InstallerDialogs::instance().downloadProgressDialog(this,false);
    connect(ui.packageList,SIGNAL(itemClicked(QTreeWidgetItem *, int)),this,SLOT(itemClicked(QTreeWidgetItem *, int)));
    connect(ui.selectAllCheckBox,SIGNAL(clicked()),this,SLOT(selectAllClicked()));

    connect(ui.applicationPackageButton,SIGNAL(clicked()),this,SLOT(slotApplicationPackageButton()));
    connect(ui.languagePackageButton,SIGNAL(clicked()),this,SLOT(slotLanguagePackageButton()));
    connect(ui.spellingPackageButton,SIGNAL(clicked()),this,SLOT(slotSpellingPackageButton()));

    connect(ui.filterEdit,SIGNAL(textChanged(const QString &)),this,SLOT(slotFilterTextChanged(const QString &)));

    activeCategories = engine->globalConfig()->endUserCategories().size() > 0 ? engine->globalConfig()->endUserCategories() : QStringList() << "KDE";
    setPackageDisplayType(Application);
    // @TODO remove
    if (ui.packageList->topLevelItemCount() == 0) {
        // no items skip page
    }
}

void EndUserPackageSelectorPage::setPackageDisplayType(PackageDisplayType type)
{
    m_displayType = type;
    if (type == Application) 
    {
        ui.applicationPackageButton->setEnabled(false);
        ui.languagePackageButton->setEnabled(true);
        ui.spellingPackageButton->setEnabled(true);
    }
    else if (type == Language) 
    {
        ui.applicationPackageButton->setEnabled(true);
        ui.languagePackageButton->setEnabled(false);
        ui.spellingPackageButton->setEnabled(true);
    }
    else if (type == Spelling) 
    {
        ui.applicationPackageButton->setEnabled(true);
        ui.languagePackageButton->setEnabled(true);
        ui.spellingPackageButton->setEnabled(false);
    }
    setWidgetData();
}

void EndUserPackageSelectorPage::itemClicked(QTreeWidgetItem *item, int column)
{
    QString name = item->data(0, Qt::StatusTipRole).toString();
    QString installedVersion = item->text ( 3 );
    QString availableVersion = item->text ( 2 );

    Package *installedPackage = engine->database()->getPackage( name,installedVersion.toAscii() );
    Package *availablePackage = engine->getPackageByName ( name,availableVersion  );
    if ( !availablePackage && !installedPackage ) {
        qWarning() << __FUNCTION__ << "neither available or installed package present for package" << name;
        return;
    }

    if ( column == 0)
    {
        engine->setNextState(*item, availablePackage, installedPackage, Package::BIN, 0 );
    }
    // dependencies are selected later
}

void EndUserPackageSelectorPage::selectAllClicked()
{
    QTreeWidget *tree = ui.packageList;
    int count = tree->topLevelItemCount();
    for(int i = 0; i < count; i++)
    {
        QTreeWidgetItem *item = tree->topLevelItem(i);
        QString name = item->data(0, Qt::StatusTipRole).toString();
        if (name.startsWith("aspell-") || name.startsWith("kde-l10n"))
            continue;
        itemClicked(item,0);
    }
}

void EndUserPackageSelectorPage::slotApplicationPackageButton()
{
    setPackageDisplayType(Application);
}

void EndUserPackageSelectorPage::slotLanguagePackageButton()
{
    setPackageDisplayType(Language);
}

void EndUserPackageSelectorPage::slotSpellingPackageButton()
{
    setPackageDisplayType(Spelling);
}

void EndUserPackageSelectorPage::slotFilterTextChanged(const QString &text)
{
    QTreeWidget *tree = ui.packageList;
    if (text.isEmpty())
    {
        for(int i = 0; i < tree->topLevelItemCount(); i++)
        {
            QTreeWidgetItem *item = tree->topLevelItem (i);
            item->setHidden(false);
        }
        return; 
    }
    QList<QTreeWidgetItem *> list = tree->findItems (text, Qt::MatchContains, 1 );
    foreach(QTreeWidgetItem *item, tree->findItems (text, Qt::MatchContains, 4 ))
    {
        if (!list.contains(item))
            list.append(item);
    }       
    for(int i = 0; i < tree->topLevelItemCount(); i++)
    {
        QTreeWidgetItem *item = tree->topLevelItem (i);
        item->setHidden(!list.contains(item));
    }
}

bool EndUserPackageSelectorPage::validatePage()
{
    return true;
}

void EndUserPackageSelectorPage::cleanupPage()
{
    disconnect(ui.packageList,SIGNAL(itemClicked(QTreeWidgetItem *, int)),this,SLOT(itemClicked(QTreeWidgetItem *, int)));
    disconnect(ui.selectAllCheckBox,SIGNAL(clicked()),this,SLOT(selectAllClicked()));

    disconnect(ui.filterEdit,SIGNAL(textChanged(const QString &)),this,SLOT(slotFilterTextChanged(const QString &)));

    disconnect(ui.applicationPackageButton,SIGNAL(clicked()),this,SLOT(slotApplicationPackageButton()));
    disconnect(ui.languagePackageButton,SIGNAL(clicked()),this,SLOT(slotLanguagePackageButton()));
    disconnect(ui.spellingPackageButton,SIGNAL(clicked()),this,SLOT(slotSpellingPackageButton()));

    engine->unselectAllPackages();
}

bool EndUserPackageSelectorPage::isComplete()
{
    return true;
}
