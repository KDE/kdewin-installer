
#include <QtDebug>
#include <QDir>
#include <QtreeWidget>

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
    m_configParser = new ConfigParser();
    downloadGlobalConfig();
}

bool InstallerEngine::downloadGlobalConfig()
{
#if 1
    QFileInfo cfi("config.txt");
    //if (!cfi.exists()) 
    {
        qDebug() << "download global configuration file";
        m_downloader->start("http://82.149.170.66/kde-windows/installer/config.txt",cfi.fileName());
    }
#endif
    qDebug() << "parsing global configuration file";
    return m_configParser->parseFromFile("config.txt");
}

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
    << "src"
    << "doc"
    << "Notes";

    tree->setColumnCount(8);
    tree->setHeaderLabels(labels);
    // see http://lists.trolltech.com/qt-interest/2006-06/thread00441-0.html
    // and Task Tracker Entry 106731
    //tree->setAlignment(Center);

    // adding top level items
    QList<QTreeWidgetItem *> categoryList;

    qDebug() << "adding categories size:" << m_configParser->sites()->size();

    QList<Site*>::iterator s;
    for (s = m_configParser->sites()->begin(); s != m_configParser->sites()->end(); s++)
    {
        qDebug() << (*s)->Name();
        QTreeWidgetItem *category = new QTreeWidgetItem((QTreeWidget*)0, QStringList((*s)->Name()));
        categoryList.append(category);
    }

    tree->insertTopLevelItems(0,categoryList);

    QList <PackageList *>::iterator k;
    // FIXME: m_packageListList has to be synced with sites
    int t=0;
    for (k = m_packageListList.begin(); k != m_packageListList.end(); ++k)
    {
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
            << (i->isInstalled(Package::SRC) ? "-I-" : "")
            << (i->isInstalled(Package::DOC) ? "-I-" : "")
            ;
            QTreeWidgetItem *item = new QTreeWidgetItem(categoryList.at(t), data);
            bool installed = i->isInstalled(Package::BIN)
                             || i->isInstalled(Package::LIB)
                             || i->isInstalled(Package::SRC)
                             || i->isInstalled(Package::DOC);

            if (!installed)
            {
                item->setCheckState(2, Qt::Unchecked);
                item->setCheckState(3, Qt::Unchecked);
                item->setCheckState(4, Qt::Unchecked);
                item->setCheckState(5, Qt::Unchecked);
                item->setCheckState(6, Qt::Unchecked);
            }
        }
        t++;
    }
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
}

void InstallerEngine::itemClickedPackageSelectorPage(QTreeWidgetItem *item, int column)
{
    if (column < 2)
        return;
    if (column == 2)
    {
        item->setCheckState(3,item->checkState(column));
        item->setCheckState(4,item->checkState(column));
        item->setCheckState(5,item->checkState(column));
        item->setCheckState(6,item->checkState(column));
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
                if (child->checkState(2) == Qt::Checked)
                {
                    if (!packageList->downloadPackage(child->text(0)))
                        qDebug() << "could not download package";
                }
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
                if (child->checkState(2) == Qt::Checked)
                {
                    if (!packageList->installPackage(child->text(0)))
                        qDebug() << "could not install package";
                }
            }
        		packageList->writeToFile();
        }
    }
    return true;
}

#else

void InstallerEngine::listPackages(const QString &title)
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
       packageList->listPackages(title);
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

