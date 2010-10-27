/****************************************************************************
**
** Copyright (C) 2008 Ralf Habacker <ralf.habacker@freenet.de> 
** Copyright (C) 2010 Patrick Spendrin <ps_ml@gmx.de> 
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
#include "packageinfo.h"
#include "packagelist.h"
#include "mirrors.h"
#include "settings.h"
#include "uninstaller.h"
#include "unpacker.h"
#include "enduserpackageselectorpage.h"
#include "packagecategorycache.h"

#include <QVector>
#include <QListWidget>
#include <QSplitter>
#include <QTextEdit>
#include <QTreeWidget>

extern InstallerEngineGui *engine;
typedef enum { C_NAME, C_ACTION, C_AVAILABLE, C_INSTALLED, C_NOTES,  /* always leave this item at the end */ C_COLUMNCOUNT } columnvalue;

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

bool EndUserPackageSelectorPage::includePackage(const QString &name, PackageDisplayType displayType)
{
    if (displayType == Language && !name.contains("-l10n") )
        return false;
    else if (displayType == Spelling && !name.contains("aspell"))
        return false;
    else if (displayType == Application 
            && ( name.contains("aspell") 
                || name.contains("-l10n") 
                || name.contains("lib") 
                || name.contains("runtime")
                ) 
            )
        return false;
    else 
        return true;
}

void EndUserPackageSelectorPage::setWidgetData()
{
    QTreeWidget *tree = ui.packageList;
    tree->clear();
    QVector<QString> labels;
    QList<QTreeWidgetItem *> items;
    QString toolTip = "select this checkbox to install or update this package";

    labels.resize( C_COLUMNCOUNT );
    labels[C_NAME] = tr ( "Package" );
    labels[C_ACTION] = tr ( "Action" );
    labels[C_AVAILABLE] = tr ( "Available" );
    labels[C_INSTALLED] = tr ( "Installed" );
    labels[C_NOTES] = tr ( "Package notes" );

    tree->setColumnCount ( C_COLUMNCOUNT );
    tree->setHeaderLabels ( QList<QString>::fromVector( labels ) );

    tree->setIndentation(10);

    // see http://lists.trolltech.com/qt-interest/2006-06/thread00441-0.html
    // and Task Tracker Entry 106731
    //tree->setAlignment(Center);

    // adding top level items
    QList<QTreeWidgetItem *> categoryList;
    QHash<QString, Package*> packageList;

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
            QString name = instPackage->name();
            Package *p = engine->packageResources()->getPackage(name);
            
            if (!p) {
                packageList[PackageInfo::baseName(name)] = instPackage;
            }
        }
    }
    
    Settings &s = Settings::instance();
    Q_FOREACH(QString categoryName, selectedCategories) 
    {
        Q_FOREACH(Package *availablePackage,categoryCache.packages(categoryName,*engine->packageResources()))
        {
            QString name = availablePackage->name();
            if ( ( categoryName == "mingw"  || s.compilerType() == MinGW )
                    && ( name.endsWith ( QLatin1String( "-msvc" ) ) || name.endsWith ( QLatin1String( "-vc90" ) ) 
                      || name.endsWith ( QLatin1String( "-mingw4" ) ) || name.endsWith ( QLatin1String( "-vc100" ) ) ) )
                continue;
            else if ( ( categoryName == "msvc"  || s.compilerType() == MSVC )
                      && ( name.endsWith ( QLatin1String ( "-mingw" ) ) || name.endsWith( QLatin1String( "-mingw4" ) ) ) )
                continue;
            else if ( ( categoryName == "mingw4"  || s.compilerType() == MinGW4 )
                    && ( name.endsWith ( QLatin1String( "-msvc" ) ) || name.endsWith ( QLatin1String( "-vc90" ) ) 
                      || name.endsWith ( QLatin1String( "-mingw" ) ) || name.endsWith ( QLatin1String( "-vc100" ) ) ) )
                continue;
            packageList[PackageInfo::baseName(name)] = availablePackage;
        }
    }
    
    // go through all metaPackages now
    Q_FOREACH(QString metaPackage, engine->globalConfig()->metaPackages().keys())
    {
        Package *p = engine->packageResources()->find(metaPackage);
        if(!p) p = packageList[PackageInfo::baseName(metaPackage)];

        // in case p is 0, we couldn't find a predefined package - e.g. another package has been defined before
        if(!p || p->hasType(Package::BIN) || p->hasType(Package::LIB)) continue;

        if (!includePackage(p->name(),m_displayType))
            continue;

        QTreeWidgetItem *item = addPackageToTree(p, 0);

        // if this is no metaPackage, simply ignore it
        if(p->hasType(Package::META)) {
            int packageCount = 0;

            // now check whether we have packages for this metaPackage in our packageList and add them as child elements
            Q_FOREACH(QString name, engine->globalConfig()->metaPackages()[metaPackage])
            {
                // if this package is contained in the packageList and inside our metaPackage, we want to make it a child item
                // of this QTreeWidgetItem
                if(packageList.keys().contains(name)) {
                    if(addPackageToTree(packageList[name], item) != 0) ++packageCount;
                    packageList.remove( name );
                }
            }

            // in case the packageCount is 0 (e.g. there are no childItems available for this metaPackage) simply delete the
            // item again which we got from addPackageToTree
            if(packageCount > 0) {
                categoryList.append(item);
                engine->setMetaPackageState(*item, C_ACTION);
            } else {
                delete item;
            }
        }
    }
    
    Q_FOREACH(Package *availablePackage,packageList.values())
    {
        if (!includePackage(availablePackage->name(),m_displayType))
            continue;
        if(availablePackage->hasType(Package::BIN)) {
            QTreeWidgetItem *item = addPackageToTree(availablePackage, 0);
            if (item)
                categoryList.append(item);
        }
    }
    tree->addTopLevelItems ( categoryList );
    tree->sortItems ( C_NAME, Qt::AscendingOrder );
    for ( int i = 0; i < tree->columnCount(); i++ )
        tree->resizeColumnToContents ( i );
}

QTreeWidgetItem *EndUserPackageSelectorPage::addPackageToTree(Package *availablePackage, QTreeWidgetItem *parent)
{
    QVector<QString> data;
    QString toolTip = "select this checkbox to install or update this package";

    Package *installedPackage = engine->database()->getPackage(availablePackage->name());
    Package::PackageVersion installedVersion = installedPackage ? installedPackage->installedVersion() : Package::PackageVersion();
    Package::PackageVersion availableVersion = availablePackage->version();
    availablePackage->setInstalledVersion(installedVersion);

    if (installedPackage && availableVersion == installedVersion)
        return 0;
    data.resize( C_COLUMNCOUNT );
    data[C_NAME] = PackageInfo::baseName(availablePackage->name());
    data[C_AVAILABLE] = (availableVersion != installedVersion ? availableVersion.toString() : "");
    data[C_INSTALLED] = installedVersion.toString();
    QTreeWidgetItem *item = new QTreeWidgetItem ( parent, QList<QString>::fromVector( data ) );
    // save real package name for selection code
    item->setData(C_ACTION, Qt::StatusTipRole, availablePackage->name());
    
    // in case the package is a meta package, we need to check whether the subpackages are installed -> fix that after this function
    engine->setEndUserInitialState(*item, availablePackage, installedPackage, C_ACTION);
    item->setText(C_NOTES, availablePackage->notes());
    item->setToolTip(C_NAME, toolTip);

    return item;
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

void EndUserPackageSelectorPage::preSelectPackages(const QString &package)
{
    QTreeWidget *tree = ui.packageList;
    QString systemCode = QLocale::system().name();
    QStringList b = systemCode.split('_');
    QString languageCode = b[0];
    QStringList searchCodes = QStringList() << systemCode << languageCode;
    QString pattern = package + "-%1";
  
    bool found = false;
    foreach(QString code, searchCodes)
    {
        QList<QTreeWidgetItem *> list = tree->findItems (QString(pattern).arg(code), Qt::MatchContains, C_NAME );
        foreach(QTreeWidgetItem *item, list)
        {
            QString name = item->data(C_ACTION, Qt::StatusTipRole).toString();
            QString availableVersion = item->text ( C_AVAILABLE );
            Package *availablePackage = engine->getPackageByName ( name, availableVersion );
            if (!engine->isPackageSelected(availablePackage,Package::BIN))
            {
                QString installedVersion = item->text ( C_INSTALLED );
                Package *installedPackage = engine->database()->getPackage( name,installedVersion.toAscii() );
                engine->setNextState(*item, availablePackage, installedPackage, Package::BIN, C_ACTION );
                qDebug() << "found" << QString(pattern).arg(code);
            }   
            found = true;
        }
        if (found)
            break;
    }
}

void EndUserPackageSelectorPage::setPackageDisplayType(PackageDisplayType type)
{
    m_displayType = type;
    setWidgetData();
    if (type == Application) 
    {
        ui.applicationPackageButton->setEnabled(false);
        ui.languagePackageButton->setEnabled(true);
        ui.spellingPackageButton->setEnabled(true);
        ui.selectAllCheckBox->setEnabled(true);
    }
    else if (type == Language) 
    {
        ui.applicationPackageButton->setEnabled(true);
        ui.languagePackageButton->setEnabled(false);
        ui.spellingPackageButton->setEnabled(true);
        ui.selectAllCheckBox->setEnabled(false);
        if (!Database::isAnyPackageInstalled(Settings::instance().installDir())) 
            preSelectPackages("kde-l10n");
    }
    else if (type == Spelling) 
    {
        ui.applicationPackageButton->setEnabled(true);
        ui.languagePackageButton->setEnabled(true);
        ui.spellingPackageButton->setEnabled(false);
        ui.selectAllCheckBox->setEnabled(false);
        if (!Database::isAnyPackageInstalled(Settings::instance().installDir())) 
            preSelectPackages("aspell");
    }
}

void EndUserPackageSelectorPage::itemClicked(QTreeWidgetItem *item, int column)
{
    QString name = item->data( C_ACTION, Qt::StatusTipRole ).toString();
    QString installedVersion = item->text ( C_INSTALLED );
    QString availableVersion = item->text ( C_AVAILABLE );

    Package *installedPackage = engine->database()->getPackage( name,installedVersion.toAscii() );
    Package *availablePackage = engine->getPackageByName( name, availableVersion );

    if ( !availablePackage && !installedPackage ) {
        qWarning() << __FUNCTION__ << "neither available or installed package present for package" << name;
        return;
    }

    if ( engine->globalConfig()->metaPackages().keys().contains( PackageInfo::baseName(name) ) ) {

        // this is a metaPackage which has no representation in the database
        // now go through all subpackages
        Q_FOREACH(QString package, engine->globalConfig()->metaPackages()[PackageInfo::baseName(name)])
        {
            int i = 0;
            QString packageName;
            
            // try to find the childItems which are connected to this metaPackage
            for(; i < item->childCount(); ++i)
            {
                packageName = item->child(i)->data(C_ACTION, Qt::StatusTipRole).toString();
                if(package == PackageInfo::baseName(item->child(i)->data(C_ACTION, Qt::StatusTipRole).toString()))
                    break;
            }
            if(i == item->childCount()) continue;
    
            QString _installedVersion = item->child(i)->text ( C_INSTALLED );
            QString _availableVersion = item->child(i)->text ( C_AVAILABLE );

            Package *ip = engine->database()->getPackage( packageName, _installedVersion.toAscii() );
            Package *ap = engine->getPackageByName( packageName, _availableVersion );

            if(!ap && !ip) continue;
            
            // switch state for the childItem
            if ( column == C_ACTION) engine->setNextState(*item->child(i), ap, ip, Package::BIN, C_ACTION, true);
        }
    }

    if ( column == C_ACTION )
    {
        engine->setNextState(*item, availablePackage, installedPackage, Package::BIN, C_ACTION );
        if(availablePackage->hasType(Package::META))
            engine->setMetaPackageState( *item, C_ACTION );

        // now check that if we're no top level item, we need to handle also the category icon
        if (ui.packageList->indexOfTopLevelItem(item) == -1) {
            engine->setMetaPackageState( *item->parent(), C_ACTION );
        }
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
        QString name = item->data(C_ACTION, Qt::StatusTipRole).toString();
        if (name.startsWith("aspell-") || name.startsWith("kde-l10n"))
            continue;
        itemClicked(item, C_ACTION);
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
            for(int j = 0; j < item->childCount(); j++)
            {
                item->child(j)->setHidden(false);
            }
        }
        return; 
    }

    QList<QTreeWidgetItem *> list = tree->findItems (text, Qt::MatchContains|Qt::MatchRecursive, C_NAME );
    Q_FOREACH(QTreeWidgetItem *item, tree->findItems (text, Qt::MatchContains|Qt::MatchRecursive, C_NOTES ))
    {
        if (!list.contains(item))
            list.append(item);
    }
    
    Q_FOREACH(QTreeWidgetItem *item, list)
    {
        QTreeWidgetItem *parent = item->parent();
        if (!list.contains(parent) && parent != tree->invisibleRootItem())
            list.append(parent);
    }
    
    for(int i = 0; i < tree->topLevelItemCount(); i++)
    {
        QTreeWidgetItem *item = tree->topLevelItem (i);
        item->setHidden(!list.contains(item));
        for(int j = 0; j < item->childCount(); j++)
        {
            QTreeWidgetItem *child = item->child(j);
            child->setHidden(!list.contains(child));
        }
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
