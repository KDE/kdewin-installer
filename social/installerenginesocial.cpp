#include "installerenginesocial.h"
#include "downloader.h"
InstallerEngineSocial::InstallerEngineSocial(QObject *parent) :
    InstallerEngine(parent)
{
    this->defaultConfigURL = QString("http://www.winkde.org/pub/kde/ports/win32/releases/stable/4.5.4/");
    this->initGlobalConfig();
    this->initPackages();
}
QStringList InstallerEngineSocial::getDependencies(QString package_name)
{
    QStringList packages;
    packages<<package_name;
    int i = 0;
    while (i < packages.size())
    {
        Package *tpack  = this->getPackageByName(packages.at(i));
        if (m_database->getPackage(packages.at(i)) != NULL)
        {
            qDebug()<<"package is allready installed: "<<packages.at(i);
            ++i;
            continue;
        }
        if (tpack==NULL)
        {
            qDebug()<<"can't get package: "<<packages.at(i);
            packages.removeAt(i);
            qDebug()<<"removed package from list";
            continue;
        }
        qDebug()<<"processing package:"<<tpack->name();
        Q_FOREACH(const QString &dependency, tpack->deps())
        {
            if (!packages.contains(dependency))
            {
                packages<<dependency;
                qDebug()<<"added package:"<<dependency;
            }
            else
                qDebug()<<"allready installing package:"<<dependency;
        }
        ++i;
        qDebug()<<"i is:"<<i<<"size is:"<<packages.size();
        qDebug()<<packages;
    }
    return packages;
}

bool InstallerEngineSocial::installpackage(QString name)
{
    Package *pack = this->getPackageByName(name);
    if (pack == NULL)
        return false;
    QStringList packages = this->getDependencies(pack->name());
    Downloader::instance()->progress()->setFileCount(packages.size());
    int i = 0;
    Q_FOREACH(const QString &package_name, packages)
    {
        Downloader::instance()->progress()->setFileNumber(i++);
        Package *pack = this->getPackageByName(package_name);
        if (pack == NULL)
        {
            qDebug()<<"can't find package "<<package_name<<" skipping it";
            continue;
        }
        if (!pack->isInstalled(FileTypes::BIN))
        {

            qDebug()<<"downloading package "<<pack->name();
            pack->downloadItem(FileTypes::BIN);
            //qDebug()<<"should install pacakage: "<<pack->name();
        }
        else
            qDebug()<<"package allready installed: "<<pack->name();

    }
    qDebug()<<"finished downloading packages";
    emit packagesToInstall(packages.size());
    Q_FOREACH(const QString &package_name, packages)
    {
        Package *pack = this->getPackageByName(package_name);
        emit packageInstalled(package_name);
        if (pack == NULL)
        {
            qDebug()<<"can't find package "<<package_name<<" skipping it";
            continue;
        }
        if (!pack->isInstalled(FileTypes::BIN))
        {

            qDebug()<<"installing package"<<pack->name();
            pack->installItem(m_installer,FileTypes::BIN);
            qDebug()<<"finished installing"<<pack->name();

            //qDebug()<<"should install pacakage: "<<pack->name();
        }
        else
            qDebug()<<"package allready installed: "<<pack->name();

    }
    qDebug()<<"finished installing software";
    postInstallTasks();

}

bool InstallerEngineSocial::postInstallTasks()
{
    qDebug()<<"running post-install tasks";
    emit postInstalationStart();
    QProcess::execute(Settings::instance().installDir()+"/bin/update-mime-database.exe",QStringList() << QDir::fromNativeSeparators(Settings::instance().installDir()) + "/share/mime");
    QProcess::execute(Settings::instance().installDir()+"/bin/kbuildsycoca4.exe");
    QProcess::execute(Settings::instance().installDir()+"/bin/kwinstartmenu.exe",QStringList() <<  "--remove");
    QProcess::execute(Settings::instance().installDir()+"/bin/kwinstartmenu.exe");
    qDebug()<<"finished post-installl tasks";
    emit postInstalationEnd();
}
