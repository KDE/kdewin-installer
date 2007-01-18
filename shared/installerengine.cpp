
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
#include "configparser.h"

InstallerEngine::InstallerEngine(DownloaderProgress *progressBar,InstallerProgress *instProgressBar)
{
    m_downloader = new Downloader(/*blocking=*/ true,progressBar);
    m_instProgressBar = instProgressBar;
    downloadGlobalConfig();
}

bool InstallerEngine::downloadGlobalConfig()
{
    PackageList *packageList = new PackageList(m_downloader);
    packageList->setConfigFileName("packages-other.txt");
    packageList->setName("Other");
    m_packageListList.append(packageList);

    // FIXME: must not be standalone 
    Installer *installer = new Installer(packageList,m_instProgressBar );
    QString root = m_settings.value("rootdir").toString();
    QFileInfo fi(root);
    if(!fi.exists() || !fi.isDir()) {
        root = QDir::currentPath();
        setRoot(root);
    }
    installer->setRoot(root);
    m_configParser = new ConfigParser(packageList);

#if 1
    QFileInfo cfi("config.txt");
    //if (!cfi.exists()) 
    {
        qDebug() << "download global configuration file";
        // FIXME uses version related config file to have more room for format changes
        m_downloader->start("http://82.149.170.66/kde-windows/installer/config.txt",cfi.fileName());
    }
#endif
    qDebug() << "parsing remote configuration file";
    int ret = m_configParser->parseFromFile("config.txt");
    fi = QFileInfo("config-local.txt");
    if (fi.exists()) 
    {
        ret = m_configParser->parseFromFile(fi.absoluteFilePath());
        qDebug() << "parsing local configuration file";
    }
    packageList->dump();
    packageList->writeToFile();
    return true;
}

/// download all packagelists, which are available on the configured sites
bool InstallerEngine::downloadPackageLists()
{
    // FIXME: m_packageListList has to be synced with sites, may be packageList contains a name used in view
    QList<Site*>::iterator s;
    for (s = m_configParser->sites()->begin(); s != m_configParser->sites()->end(); s++)
    {
        qDebug() << "download package file list for site: " << (*s)->Name();
        PackageList *packageList = new PackageList(m_downloader);
        packageList->setConfigFileName("packages-" + (*s)->Name() + ".txt");
        packageList->setName((*s)->Name());
        m_packageList = packageList;
        m_packageListList.append(packageList);
        Installer *installer = new Installer(packageList,m_instProgressBar );

        // FIXME:: hardcoded name, better to use an option in the config file ?
        if ((*s)->Name() == "gnuwin32")
        {
            installer->setType(Installer::GNUWIN32);
            // FIXME: add additional option in config.txt for mirrors
            packageList->setBaseURL("http://heanet.dl.sourceforge.net/sourceforge/gnuwin32/");
        }
        else
            packageList->setBaseURL((*s)->URL());
        installer->setRoot(m_settings.value("rootdir").toString());
        m_installerList.append(installer);
        m_installer = installer;

        if ( !packageList->hasConfig() )
        {
            // download package list
            qDebug() << (*s)->URL();
#ifdef DEBUG
            QFileInfo tmpFile(installer->Root() + "/packages-"+(*s)->Name()+".html");
            if (!tmpFile.exists())
                m_downloader->start((*s)->URL(), ba));

            // load and parse
            if (!packageList->readHTMLFromFile(tmpFile.absoluteFilePath(),(*s)->Type() == Site::ApacheModIndex ? PackageList::ApacheModIndex : PackageList::SourceForge ))
#else            
            QByteArray ba;
            m_downloader->start((*s)->URL(), ba);
            if (!packageList->readHTMLFromByteArray(ba,(*s)->Type() == Site::ApacheModIndex ? PackageList::ApacheModIndex : PackageList::SourceForge ))
#endif
            {
                qDebug() << "error reading package list from download html file";
                continue;
            }

            // save into file
            if (!packageList->writeToFile())
            {
                qDebug() << "error writing package list to file";
                continue;
            }

        }
        else
        {
            // read list from file
            if (!packageList->readFromFile())
            {
                qDebug() << "error reading package list from file";
                continue;
            }
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

enum actionType { _initial, _next, _deps}; 
enum stateType { _Install, _Update, _Remove, _Nothing}; 

void setState(QTreeWidgetItem &item, const Package &pkg, int column, actionType action)
{
    Package::Type type = columnToType(column);

    if (type != Package::ALL && !pkg.hasType(type)) 
    {
        setIcon(item,type,_disable);
        qDebug() << "disabled";
        return; 
    }

    switch (action)
    {
        case _initial: 
            if (pkg.isInstalled(type))
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
        
        case _next:
            stateType state = (stateType)item.data(column,Qt::UserRole).toInt();
            qDebug() << __FUNCTION__ << state << type;
            // enter next state depending on current state 
            switch(state)
            {
                case _Nothing:  
                    if (pkg.isInstalled(type))
                    {
                        setIcon(item,type,_remove);
                        item.setData(column,Qt::UserRole,_Remove);
                    }
                    else
                    {
                        setIcon(item,type,_install);
                        item.setData(column,Qt::UserRole,_Install);
                    }
                    break;

                case _Remove:  
                    item.setData(column,Qt::UserRole,_Nothing);
                    setIcon(item,type,_nothing);  
                    break;

                case _Install:  
                    item.setData(column,Qt::UserRole,_Nothing);
                    setIcon(item,type,_nothing);  
                    break;
            }
        case _deps:
            if (pkg.isInstalled(type))
            {
            }
            else
            {
                // FIXME: should be _autoinstall, but then the main package is using this icon too 
                setIcon(item,type,_install);
                item.setData(column,Qt::UserRole,_install);
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

    qDebug() << "adding categories size:" << m_configParser->sites()->size();

    QList <PackageList *>::ConstIterator k = m_packageListList.constBegin();
    for ( ; k != m_packageListList.constEnd(); ++k)
    {
        QTreeWidgetItem *category = new QTreeWidgetItem((QTreeWidget*)0, QStringList((*k)->Name()));
        categoryList.append(category);

        // adding sub items
        QList<Package>::ConstIterator i = (*k)->packageList().constBegin();
        for ( ; i != (*k)->packageList().constEnd(); ++i)
        {
            QStringList data;
            data << i->name()
            << i->version()
            << QString()
            ;
            QTreeWidgetItem *item = new QTreeWidgetItem(category, data);
            setState(*item,*i,2,_initial);
            setState(*item,*i,3,_initial);
            setState(*item,*i,4,_initial);
            setState(*item,*i,5,_initial);
            setState(*item,*i,6,_initial);
        }
    }
    tree->insertTopLevelItems(0,categoryList);
    tree->expandAll();
}
extern QTreeWidget *tree;

void InstallerEngine::itemClickedPackageSelectorPage(QTreeWidgetItem *item, int column)
{
    if (column < 2)
        return;

    Package *pkg = getPackageByName(item->text(0));

    if (column == 2 && pkg)
    {
       setState(*item,*pkg,3,_next);
       setState(*item,*pkg,4,_next);
       setState(*item,*pkg,5,_next);
       setState(*item,*pkg,6,_next);
    }
    else
       setState(*item,*pkg,column,_next);

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

                setState(*depItem,*depPkg,3,_deps);
            }
        }    
    }

}

bool InstallerEngine::downloadPackages(QTreeWidget *tree, const QString &category)
{
    for (int i = 0; i < tree->topLevelItemCount(); i++)
    {
        QTreeWidgetItem *item = static_cast<QTreeWidgetItem*>(tree->topLevelItem(i));
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
    QList<Site*>::iterator s;
    for (s = m_configParser->sites()->begin(); s != m_configParser->sites()->end(); s++)
    {
       qDebug() << (*s)->Name();
       PackageList *packageList = getPackageListByName((*s)->Name());
       if (!packageList)
       {
           qDebug() << __FUNCTION__ << " packagelist for " << (*s)->Name() << " not found";
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
    QList<Site*>::iterator s;
    for (s = m_configParser->sites()->begin(); s != m_configParser->sites()->end(); s++)
    {
       qDebug() << (*s)->Name();
       PackageList *packageList = getPackageListByName((*s)->Name());
       if (!packageList)
       {
           qDebug() << __FUNCTION__ << " packagelist for " << (*s)->Name() << " not found";
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

void InstallerEngine::setRoot(const QString &root)
{
    m_settings.setValue("rootdir", QDir::convertSeparators(root));
}

QString InstallerEngine::root() const
{
    QString root = m_settings.value("rootdir").toString();
    if (root.isEmpty())
        return QDir::convertSeparators(QDir::currentPath());
    return root;
}

void InstallerEngine::stop()
{
    m_downloader->cancel();
}
