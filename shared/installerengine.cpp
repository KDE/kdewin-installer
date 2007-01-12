
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
    installer->setRoot(m_settings.value("rootdir").toString());
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
    QFileInfo fi("config-local.txt");
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

    QList <PackageList *>::iterator k;
    for (k = m_packageListList.begin(); k != m_packageListList.end(); ++k)
    {
        QTreeWidgetItem *category = new QTreeWidgetItem((QTreeWidget*)0, QStringList((*k)->Name()));
        categoryList.append(category);

        // adding sub items
        QList<Package>::iterator i;
        for (i = (*k)->packageList()->begin(); i != (*k)->packageList()->end(); ++i)
        {
            QStringList data;
            data << i->name()
            << i->version()
            << ""
            << (i->isInstalled(Package::BIN) ? "-I-" : "")
            << (i->isInstalled(Package::LIB) ? "-I-" : "")
            << (i->isInstalled(Package::DOC) ? "-I-" : "")
            << (i->isInstalled(Package::SRC) ? "-I-" : "")
            ;
            QTreeWidgetItem *item = new QTreeWidgetItem(category, data);
            if (i->hasType(Package::BIN) && !i->isInstalled(Package::BIN)) item->setCheckState(3, Qt::Unchecked);
            if (i->hasType(Package::LIB) && !i->isInstalled(Package::LIB)) item->setCheckState(4, Qt::Unchecked);
            if (i->hasType(Package::DOC) && !i->isInstalled(Package::DOC)) item->setCheckState(5, Qt::Unchecked);
            if (i->hasType(Package::SRC) && !i->isInstalled(Package::SRC)) item->setCheckState(6, Qt::Unchecked);
            item->setCheckState(2, Qt::Unchecked);
        }
    }
    tree->insertTopLevelItems(0,categoryList);
    /*
     QStringList data; 
     data.clear(); 
     data << "kdelibs" << "4.1.2";
     item = new QTreeWidgetItem(categoryList.at(5), data);
     data.clear(); 
     data << "kdebase" << "4.1.2";
     item = new QTreeWidgetItem(categoryList.at(5), data);
     data.clear(); 
     data << "kdepim" << "4.1.2";
     item = new QTreeWidgetItem(categoryList.at(5), data);
    */
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
        if (pkg->hasType(Package::BIN) && !pkg->isInstalled(Package::BIN)) item->setCheckState(3,item->checkState(column)); 
        if (pkg->hasType(Package::LIB) && !pkg->isInstalled(Package::LIB)) item->setCheckState(4,item->checkState(column)); 
        if (pkg->hasType(Package::DOC) && !pkg->isInstalled(Package::DOC)) item->setCheckState(5,item->checkState(column)); 
        if (pkg->hasType(Package::SRC) && !pkg->isInstalled(Package::SRC)) item->setCheckState(6,item->checkState(column)); 
    }
    // select depending packages in case all or bin is selected
    //pkg->dump();
    if (column == 2 || column == 3 && pkg) 
    {
        const QStringList &deps = pkg->deps();
    
        for (int i = 0; i < deps.size(); ++i)
        {  
        	 qDebug() << deps.at(i);
        	 QString dep = deps.at(i);
           QList<QTreeWidgetItem *> items = tree->findItems(deps.at(i),Qt::MatchFixedString | Qt::MatchRecursive);
           qDebug() << items.size();
           for (int j = 0; j < items.size(); ++j) 
           {
           	   qDebug() << items.at(j);
               QTreeWidgetItem * depItem = items.at(j);
               /// the dependency is only for bin package and one way to switch on
               if (depItem->text(3) != "-I-")
                   depItem->setCheckState(3,item->checkState(column));
           }
       }    
    }

}

bool InstallerEngine::downloadPackages(QTreeWidget *tree, const QString &category)
{
    for (int i = 0; i < tree->topLevelItemCount(); i++)
    {
        QTreeWidgetItem *item = tree->topLevelItem(i);
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
                QTreeWidgetItem *child = item->child(j);
                qDebug("%s %s %d",child->text(0).toAscii().data(),child->text(1).toAscii().data(),child->checkState(2));
                Package::Types types;
                types |= child->checkState(3) == Qt::Checked ? Package::BIN : Package::NONE;
                types |= child->checkState(4) == Qt::Checked ? Package::LIB : Package::NONE;
                types |= child->checkState(5) == Qt::Checked ? Package::DOC : Package::NONE;
                types |= child->checkState(6) == Qt::Checked ? Package::SRC : Package::NONE;
               	types |= child->checkState(2) == Qt::Checked ? Package::ALL: Package::NONE;
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
        QTreeWidgetItem *item = tree->topLevelItem(i);
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
                QTreeWidgetItem *child = item->child(j);
                qDebug("%s %s %d",child->text(0).toAscii().data(),child->text(1).toAscii().data(),child->checkState(2));
                Package::Types types;
                types |= child->checkState(3) == Qt::Checked ? Package::BIN : Package::NONE;
                types |= child->checkState(4) == Qt::Checked ? Package::LIB : Package::NONE;
                types |= child->checkState(5) == Qt::Checked ? Package::DOC : Package::NONE;
                types |= child->checkState(6) == Qt::Checked ? Package::SRC : Package::NONE;
               	types |= child->checkState(2) == Qt::Checked ? Package::ALL: Package::NONE;
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

