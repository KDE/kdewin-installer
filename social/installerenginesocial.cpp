/****************************************************************************
**
** Copyright (C) 2011 Constantin Tudorica <tudalex@gmail.com>
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

#include "installerenginesocial.h"
#include "downloader.h"
#include <QtGui>
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
    int status = 0;
    QWidget *download = new QWidget;
    DownloaderProgress *progress = new DownloaderProgress(download);
    QVBoxLayout *layout = new QVBoxLayout;
    layout->addWidget(progress);
    layout->addStretch(1);
    download->setLayout(layout);


    Downloader::instance()->setProgress(progress);


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



    InstallerProgressSocial *inst_progress = new InstallerProgressSocial();
    connect(this,SIGNAL(packagesToInstall(int)),inst_progress,SLOT(getpackageno(int)));
    connect(this,SIGNAL(packageInstalled(QString)),inst_progress,SLOT(packageinstalled(QString)));
    connect(this,SIGNAL(postInstalationStart()),inst_progress,SLOT(InstallMenuItems()));
    connect(this,SIGNAL(postInstalationEnd()),inst_progress,SLOT(FinishedInstallMenuItems()));


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
            status&=pack->installItem(m_installer,FileTypes::BIN);
            qDebug()<<"finished installing"<<pack->name();

            //qDebug()<<"should install pacakage: "<<pack->name();
        }
        else
            qDebug()<<"package allready installed: "<<pack->name();

    }
    qDebug()<<"finished installing software";

    delete inst_progress;
    return status|!postInstallTasks();
}
bool InstallerEngineSocial::runCommand(const QString &msg, const QString &app, const QStringList &params)
{
    QFileInfo f(Settings::instance().installDir()+"/bin/" + app + ".exe");
    qDebug() << "checking for app " << app << " - "  <<(f.exists() ? "found" : "not found");
    if (!f.exists())
        return false;


    QCoreApplication::processEvents();
    qDebug() << "running " << app << params;
    return QProcess::execute( f.absoluteFilePath(), params) == 0 ? true : false;
}

bool InstallerEngineSocial::postInstallTasks()
{
    //TODO: Notice if any files of this crashes and report-it to the logfile
    qDebug()<<"running post-install tasks";
    emit postInstalationStart();
    int status = 0;
    status|=runCommand("updating mime database","update-mime-database",QStringList() << QDir::fromNativeSeparators(Settings::instance().installDir()) + "/share/mime");
    status|=runCommand("updating system configuration database","kbuildsycoca4");
    status|=runCommand("deleting old windows start menu entries","kwinstartmenu",QStringList() <<  "--remove");
    status|=runCommand("creating new windows start menu entries","kwinstartmenu");
    qDebug()<<"finished post-installl tasks";
    emit postInstalationEnd();
    return true;
}


