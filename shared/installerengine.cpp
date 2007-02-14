
#include <QtDebug>
#include <QDir>
#include <QTreeWidget>
#include <QFlags>

#include "installerengine.h"

#include "downloader.h"
#include "installer.h"
#include "installerprogress.h"
#include "package.h"
#include "packagelist.h"
#include "globalconfig.h"

InstallerEngine::InstallerEngine(DownloaderProgress *progressBar,InstallerProgress *instProgressBar)
{
    m_downloader = new Downloader(/*blocking=*/ true,progressBar);
    m_instProgressBar = instProgressBar;
    m_globalConfig = new GlobalConfig(QString("http://82.149.170.66/kde-windows/installer/config.txt"),*m_downloader);
    m_globalConfig->dump(__FUNCTION__);
    createMainPackagelist();
}

void InstallerEngine::createMainPackagelist()
{
    // package list is build from packages defined in global configuration
    m_packageList = new PackageList(m_downloader);
    m_packageList->setConfigFileName("packages.txt");
	m_packageList->setName("main");
    m_packageListList.append(m_packageList);

    m_installer = new Installer(m_packageList,m_instProgressBar );
    m_installer->setRoot(root());
    m_installerList.append(m_installer);

    QList<Package*>::iterator p;
    for (p = m_globalConfig->packages()->begin(); p != m_globalConfig->packages()->end(); p++)
    {
        m_packageList->addPackage(*(*p));
    }
    
    if (m_packageList->hasConfig())
        m_packageList->syncWithFile();

    m_packageList->writeToFile();
}

/// download all packagelists, which are available on the configured sites
bool InstallerEngine::downloadPackageLists()
{
    m_installer->setRoot(root());
    QList<Site*>::iterator s;
	PackageList *packageList = m_packageList;
    for (s = m_globalConfig->sites()->begin(); s != m_globalConfig->sites()->end(); s++)
    {
        qDebug() << "download package file list for site: " << (*s)->name();
        // packagelist needs to access Site::getDependencies() && Site::isExclude()
        packageList->setCurrentSite(*s);
        qDebug() << __FUNCTION__; 
        (*s)->dump();

        // FIXME:: hardcoded name, better to use an option in the config file ?
        if ((*s)->name() == "gnuwin32")
        {
            m_installer->setType(Installer::GNUWIN32);
            // FIXME: add additional option in config.txt for mirrors
            packageList->setBaseURL("http://heanet.dl.sourceforge.net/sourceforge/gnuwin32/");
        }
        else
            packageList->setBaseURL((*s)->url());

		// FIXME: it is probably better to download package list every 
		//        time and to sync with local copy 

		// download package list
        qDebug() << (*s)->url();
#ifdef DEBUG
        QFileInfo tmpFile(installer->Root() + "/packages-"+(*s)->Name()+".html");
        if (!tmpFile.exists())
            m_downloader->start((*s)->URL(), ba));

        // load and parse
        if (!packageList->readHTMLFromFile(tmpFile.absoluteFilePath(),(*s)->Type() == Site::ApacheModIndex ? PackageList::ApacheModIndex : PackageList::SourceForge ))
#else            
        QByteArray ba;
        m_downloader->start((*s)->url(), ba);
        if (!packageList->readHTMLFromByteArray(ba,(*s)->Type() == Site::ApacheModIndex ? PackageList::ApacheModIndex : PackageList::SourceForge ))
#endif
        {
            qDebug() << "error reading package list from download html file";
            continue;
        }
	
		if (packageList->hasConfig())
			packageList->syncWithFile();

		if (!packageList->writeToFile())
		{
			  qDebug() << "error writing package list to file";
			continue;
		}
	}

	return true;
}

#ifdef USE_GUI


int typeToColumn(Package::Type type)
{
    switch (type)
    {
        case Package::BIN : return 3;
        case Package::LIB : return 4;
        case Package::DOC : return 5;
        case Package::SRC : return 6;
        default: return 2;
    }	
}

Package::Type columnToType(int column)
{
    switch (column)
    {
        case 3: return Package::BIN;
        case 4: return Package::LIB; 
        case 5: return Package::DOC; 
        case 6: return Package::SRC; 
        case 2: return Package::ALL;
        default : return Package::NONE;
    }	
}

enum iconType {_install, _autoinstall,_keepinstalled, _update, _remove, _nothing, _disable}; 

void setIcon(QTreeWidgetItem &item, Package::Type type, iconType action)
{
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

    int column = typeToColumn(type);
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
    // FIXME: does not work, don't know how to set the icon size 
    // item.icon(column).setIconSize(QSize(22,22));
}

enum actionType { _initial, _next, _deps, _sync}; 
enum stateType { _Install, _Update, _Remove, _Nothing}; 

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
            if (pkg->isInstalled(type))
            {
                setIcon(item,type,_keepinstalled );
                item.setData(column,Qt::UserRole,_Nothing);
            }
            else
            {
                setIcon(item,type,_nothing);
                item.setData(column,Qt::UserRole,_Nothing);
            }
            break;
        
        // enter next state depending on current state 
        case _next: 
        {
            stateType state = (stateType)item.data(column,Qt::UserRole).toInt();

            switch(state)
            {
                case _Nothing:  
                    if (pkg->isInstalled(type))
                    {
// FIXME: add remove support
#if 0
                        setIcon(item,type,_remove);
                        item.setData(column,Qt::UserRole,_Remove);
#endif
                    }
                    else
                    {
                        setIcon(item,type,_install);
                        item.setData(column,Qt::UserRole,_Install);
                    }
                    break;

                case _Remove:  
                    if (pkg->isInstalled(type))
                    {
                        setIcon(item,type,_keepinstalled );
                        item.setData(column,Qt::UserRole,_Nothing);
                    }
                    else
                    {
                        item.setData(column,Qt::UserRole,_Nothing);
                        setIcon(item,type,_nothing);  
                    }
                    break;

                case _Install:  
                    item.setData(column,Qt::UserRole,_Nothing);
                    setIcon(item,type,_nothing);  
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
                item.setData(column,Qt::UserRole,_Install);
            }
            break;
        }
        case _sync: 
            stateType state = (stateType)item.data(syncColumn,Qt::UserRole).toInt();
            switch(state) 
            {
                case _Install: 
                    if (!pkg->isInstalled(type))
                    {
                        setIcon(item,type,_install);
                        item.setData(column,Qt::UserRole,_Install);
                    }
                    break;
                case _Nothing: 
                    if (!pkg->isInstalled(type))
                    {
                        setIcon(item,type,_nothing);
                        item.setData(column,Qt::UserRole,_Nothing);
                    }
                    break;
// FIXME: add remove support
#if 0
                case _Remove:
                    break;
#endif
            }                       
	}        
}

bool isMarkedForInstall(QTreeWidgetItem &item,Package::Type type)
{
    int column = typeToColumn(type);
    stateType state = (stateType)item.data(column,Qt::UserRole).toInt();
    return state == _Install;
}

bool isMarkedForRemoval(QTreeWidgetItem &item,Package::Type type)
{
    int column = typeToColumn(type);
    stateType state = (stateType)item.data(column,Qt::UserRole).toInt();
    return state == _Remove;
}

void InstallerEngine::setPageSelectorWidgetData(QTreeWidget *tree)
{
    QStringList labels;
    QList<QTreeWidgetItem *> items;
    // QTreeWidgetItem *item;

    labels
    << "Package"
    << "Version"
    << "all"
    << "bin"
    << "lib"
    << "doc"
    << "src"
    << "Notes";

	tree->setColumnCount(8);
    tree->setHeaderLabels(labels);
    // see http://lists.trolltech.com/qt-interest/2006-06/thread00441-0.html
    // and Task Tracker Entry 106731
    //tree->setAlignment(Center);

    // adding top level items
    QList<QTreeWidgetItem *> categoryList;

    qDebug() << "adding categories size:" << m_globalConfig->sites()->size();

    QList <PackageList *>::ConstIterator k = m_packageListList.constBegin();
    for ( ; k != m_packageListList.constEnd(); ++k)
    {
        QTreeWidgetItem *category = new QTreeWidgetItem((QTreeWidget*)0, QStringList((*k)->Name()));
        categoryList.append(category);

        // adding sub items
        QList<Package*>::ConstIterator i = (*k)->packageList().constBegin();
        for ( ; i != (*k)->packageList().constEnd(); ++i)
        {
            QStringList data;
            data << (*i)->name()
                 << (*i)->version()
                 << QString();
            QTreeWidgetItem *item = new QTreeWidgetItem(category, data);
            setState(*item,*i,2,_initial);
            setState(*item,*i,3,_initial);
            setState(*item,*i,4,_initial);
            setState(*item,*i,5,_initial);
            setState(*item,*i,6,_initial);
            item->setText(7, (*i)->notes());
        }
    }
    tree->insertTopLevelItems(0,categoryList);
    tree->expandAll();
	tree->sortItems(0,Qt::AscendingOrder);
}
extern QTreeWidget *tree;

void InstallerEngine::itemClickedPackageSelectorPage(QTreeWidgetItem *item, int column)
{
    if (column < 2)
        return;

    Package *pkg = getPackageByName(item->text(0));

    if (column == 2 && pkg)
    {
       setState(*item,pkg,2,_next);
       setState(*item,pkg,3,_sync,2);
       setState(*item,pkg,4,_sync,2);
       setState(*item,pkg,5,_sync,2);
       setState(*item,pkg,6,_sync,2);
    }
    else
       setState(*item,pkg,column,_next);

    // select depending packages in case all or bin is selected
    //pkg->dump();
    if (column == 2 || column == 3 && pkg) 
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

                setState(*depItem,depPkg,3,_deps);
            }
        }    
    }

}

bool InstallerEngine::downloadPackages(QTreeWidget *tree, const QString &category)
{
    for (int i = 0; i < tree->topLevelItemCount(); i++)
    {
        QTreeWidgetItem *item = static_cast<QTreeWidgetItem*>(tree->topLevelItem(i));
#ifdef DEBUG
        qDebug() << __FUNCTION__ << " " << item->text(0);
#endif
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
//                qDebug("%s %s %d",child->text(0).toAscii().data(),child->text(1).toAscii().data(),child->checkState(2));
                Package::Types types;
                types |= isMarkedForInstall(*child,Package::BIN) ? Package::BIN : Package::NONE;
                types |= isMarkedForInstall(*child,Package::LIB) ? Package::LIB : Package::NONE;
                types |= isMarkedForInstall(*child,Package::DOC) ? Package::DOC : Package::NONE;
                types |= isMarkedForInstall(*child,Package::SRC) ? Package::SRC : Package::NONE;
//               	types |= isMarkedForInstall(child,Package::ALL) ? Package::ALL: Package::NONE;
                if (!packageList->downloadPackage(child->text(0), types))
                    qDebug() << "could not download package";
            }
        }
    }
    return true;
}

bool InstallerEngine::installPackages(QTreeWidget *tree,const QString &category)
{
    for (int i = 0; i < tree->topLevelItemCount(); i++)
    {
        QTreeWidgetItem *item = static_cast<QTreeWidgetItem*>(tree->topLevelItem(i));
#ifdef DEBUG
        qDebug() << __FUNCTION__ << " " << item->text(0);
#endif
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
//                qDebug("%s %s %d",child->text(0).toAscii().data(),child->text(1).toAscii().data(),child->checkState(2));
                Package::Types types;
                types |= isMarkedForInstall(*child,Package::BIN) ? Package::BIN : Package::NONE;
                types |= isMarkedForInstall(*child,Package::LIB) ? Package::LIB : Package::NONE;
                types |= isMarkedForInstall(*child,Package::DOC) ? Package::DOC : Package::NONE;
                types |= isMarkedForInstall(*child,Package::SRC) ? Package::SRC : Package::NONE;
//               	types |= child->checkState(2) == Qt::Checked ? Package::ALL: Package::NONE;
                if (!packageList->installPackage(child->text(0),types))
                    qDebug() << "could not install package";
            }
            packageList->writeToFile();
        }
    }
    return true;
}

#else

void InstallerEngine::listPackages(const QString &title)
{
    QList <PackageList *>::iterator k;
    for (k = m_packageListList.begin(); k != m_packageListList.end(); ++k)
    {
       qDebug() << (*k)->Name();
       (*k)->listPackages(title);
   }
}

bool InstallerEngine::downloadPackages(const QStringList &packages, const QString &category)
{
    QList<Site*>::ConstIterator s = m_globalConfig->sites()->constBegin();
    for ( ; s != m_globalConfig->sites()->constEnd(); s++)
    {
       qDebug() << (*s)->name();
       PackageList *packageList = getPackageListByName((*s)->name());
       if (!packageList)
       {
           qDebug() << __FUNCTION__ << " packagelist for " << (*s)->name() << " not found";
           continue;
       }
       foreach(QString package, packages)
       {
           if (!packageList->downloadPackage(package))
               qDebug() << "could not download package" << package;
       }
   }
   return true;
}

bool InstallerEngine::installPackages(const QStringList &packages,const QString &category)
{
    QList<Site*>::ConstIterator s = m_globalConfig->sites()->constBegin();
    for ( ; s != m_globalConfig->sites()->constEnd(); s++)
    {
       qDebug() << (*s)->name();
       PackageList *packageList = getPackageListByName((*s)->name());
       if (!packageList)
       {
           qDebug() << __FUNCTION__ << " packagelist for " << (*s)->name() << " not found";
           continue;
       }
       foreach(QString package, packages)
       {
           if (!packageList->installPackage(package))
               qDebug() << "could not download package" << package;
       }
       packageList->writeToFile();
   }
   return true;
}
#endif

PackageList *InstallerEngine::getPackageListByName(const QString &name)
{
    QList <PackageList *>::iterator k;
    for (k = m_packageListList.begin(); k != m_packageListList.end(); ++k)
    {
        if ((*k)->Name() == name)
            return (*k);
    }
    return 0;
}

Package *InstallerEngine::getPackageByName(const QString &name)
{
	  Package *pkg;
    QList <PackageList *>::iterator k;
    for (k = m_packageListList.begin(); k != m_packageListList.end(); ++k)
    {
       pkg = (*k)->getPackage(name);
       if (pkg)
           return pkg;
    }
    return 0;
}

void InstallerEngine::setRoot(QString root)
{
    Settings::getInstance().setInstallDir(root);
}

const QString InstallerEngine::root()
{
    QString root = Settings::getInstance().installDir();
    if (root.isEmpty())
    {
        root = QDir::currentPath();
        Settings::getInstance().setInstallDir(root);
    }
    return QDir::convertSeparators(root);
}

void InstallerEngine::stop()
{
    m_downloader->cancel();
}
