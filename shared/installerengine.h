/****************************************************************************
**
** Copyright (C) 2005-2007 Ralf Habacker. All rights reserved.
**
** This file is part of the KDE installer for windows
**
** This library is free software; you can redistribute it and/or
** modify it under the terms of the GNU Library General Public
** License version 2 as published by the Free Software Foundation.
**
** This library is distributed in the hope that it will be useful,
** but WITHOUT ANY WARRANTY; without even the implied warranty of
** MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU
** Library General Public License for more details.
**
** You should have received a copy of the GNU Library General Public License
** along with this library; see the file COPYING.LIB.  If not, write to
** the Free Software Foundation, Inc., 51 Franklin Street, Fifth Floor,
** Boston, MA 02110-1301, USA.
**
****************************************************************************/

#ifndef INSTALLERENGINE_H
#define INSTALLERENGINE_H

#include <QString>
#include <QList>

#include "settings.h"

class PackageList;
class Downloader;
class DownloaderProgress;
class Installer;
class InstallerProgress;
class GlobalConfig;
class Settings;
class Package;
class InstallWizard;
class QTreeWidget;
class QTreeWidgetItem;
class Database;

class InstallerEngine
{
public:
    InstallerEngine(DownloaderProgress *progressBar,InstallerProgress *instProgressBar);
	void readGlobalConfig();
    void createMainPackagelist();
    bool downloadPackageLists();
    void stop();

#ifdef USE_GUI
    void setPageSelectorWidgetData(QTreeWidget *tree);
    void itemClickedPackageSelectorPage(QTreeWidgetItem *item, int column);
    bool downloadPackages(QTreeWidget *tree, const QString &category=QString());
    bool installPackages(QTreeWidget *tree, const QString &category=QString());
	bool removePackages(QTreeWidget *tree, const QString &category=QString());
#else
    void listPackages(const QString &title);
    bool downloadPackages(const QStringList &packages, const QString &category=QString());
    bool installPackages(const QStringList &packages, const QString &category=QString());
#endif
    PackageList *getPackageListByName(const QString &name);
    Package *getPackageByName(const QString &name);
    
    void setRoot(QString root);
    const QString root();


    PackageList *packageList()
    {
        return m_packageList;
    }

    Installer *installer()
    {
        return m_installer;
    }

private:
	enum Type { EndUser, Developer, Single };
    QList <PackageList*> m_packageListList;
    QList <Installer*>   m_installerList;
    PackageList         *m_packageList;  // currently used packagelist
    Installer           *m_installer;    // currently used installer
    Downloader          *m_downloader;
    InstallerProgress   *m_instProgress;
    GlobalConfig        *m_globalConfig;
    InstallerProgress   *m_instProgressBar;
	DownloaderProgress  *m_progressBar;
	Database            *m_database;
	Type                 m_installMode;
};

#endif
