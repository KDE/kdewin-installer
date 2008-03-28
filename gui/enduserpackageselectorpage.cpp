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
    setSubTitle(statusTip());
    categories = "KDE";
}

void EndUserPackageSelectorPage::setWidgetData(QString categoryName)
{
    QTreeWidget *tree = ui.packageList;
    tree->clear();
    QStringList labels;
    QList<QTreeWidgetItem *> items;
    QString toolTip = "select this checkbox to install this package";

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

    // add packages which are installed but for which no config entry is there 
    foreach(Package *instPackage, categoryCache.packages(categoryName,*engine->database())) 
    {
        Package *p = engine->packageResources()->getPackage(instPackage->name());
        if (!p)
            packageList << instPackage;
    }

    Settings &s = Settings::instance();
    foreach(Package *availablePackage,categoryCache.packages(categoryName,*engine->packageResources()))
    {
        QString name = availablePackage->name();
        if ( ( categoryName == "mingw"  || s.compilerType() == Settings::MinGW )
                && ( name.endsWith ( QLatin1String( "-msvc" ) ) ) )
            continue;
        else if ( ( categoryName == "msvc"  || s.compilerType() == Settings::MSVC )
                  && ( name.endsWith ( QLatin1String ( "-mingw" ) ) ) )
            continue;
        packageList << availablePackage;
    }

    foreach(Package *availablePackage,packageList)
    {
        QString name = availablePackage->name();
        QStringList data;
        Package *installedPackage = engine->database()->getPackage(availablePackage->name());
        QString installedVersion = installedPackage ? installedPackage->installedVersion() : "";
        QString availableVersion = availablePackage->version();
        availablePackage->setInstalledVersion(installedVersion);

        /// @TODO add version format check to be sure available package is really newer
        data 
            << ""
            << availablePackage->name()
            << (availableVersion != installedVersion ? availablePackage->version() : "")
            << installedVersion
            << QString();
        QTreeWidgetItem *item = new QTreeWidgetItem ( ( QTreeWidgetItem* ) 0, data );
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
    InstallerDialogs::instance().downloadProgressDialog(this,true,tr("Downloading Package Lists"));
    engine->init();
    InstallerDialogs::instance().downloadProgressDialog(this,false);
    connect(ui.packageList,SIGNAL(itemClicked(QTreeWidgetItem *, int)),this,SLOT(itemClicked(QTreeWidgetItem *, int)));
    connect(&Settings::instance(),SIGNAL(installDirChanged(const QString &)),this,SLOT(installDirChanged(const QString &)));
    connect(&Settings::instance(),SIGNAL(compilerTypeChanged()),this,SLOT(slotCompilerTypeChanged()));
    setWidgetData(categories);
}

void EndUserPackageSelectorPage::itemClicked(QTreeWidgetItem *item, int column)
{
    QString name = item->text ( 1 );
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

void EndUserPackageSelectorPage::installDirChanged(const QString &dir)
{
    engine->reload();
    setWidgetData(categories);
}

void EndUserPackageSelectorPage::slotCompilerTypeChanged()
{
    setWidgetData(categories);
}

bool EndUserPackageSelectorPage::validatePage()
{
    return true;
}

void EndUserPackageSelectorPage::cleanupPage()
{
    disconnect(ui.packageList,SIGNAL(itemClicked(QTreeWidgetItem *, int)),this,SLOT(itemClicked(QTreeWidgetItem *, int)));
    disconnect(&Settings::instance(),SIGNAL(installDirChanged(const QString &)),this,SLOT(installDirChanged(const QString &)));
    disconnect(&Settings::instance(),SIGNAL(compilerTypeChanged()),this,SLOT(slotCompilerTypeChanged()));
}

bool EndUserPackageSelectorPage::isComplete()
{
    return true;
}

#include "enduserpackageselectorpage.moc"
