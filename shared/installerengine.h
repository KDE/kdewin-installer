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
#include "globalconfig.h"

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

class InstallerEngine : public QObject
{
    Q_OBJECT
public:
    InstallerEngine(DownloaderProgress *progressBar,InstallerProgress *instProgressBar);
    ~InstallerEngine();
	/**
		fetch configuration from local (file procotol), http or ftp mirrors 
		The precedence is 
	*/
	bool readGlobalConfig();
    void createMainPackagelist();
    bool downloadPackageLists();
    void stop();

    PackageList *getPackageListByName(const QString &name);
    Package *getPackageByName(const QString &name,const QString &version=QString());

    Installer *installer()
    {
        return m_installer;
    }

    GlobalConfig *globalConfig() { return m_globalConfig; }
	/// download url 
	static QString defaultConfigURL;
	static QString fallBackURL;

protected slots:
    void installDirChanged(const QString &newdir);
    void mirrorChanged(const QString &mirror);

protected:
    QList <PackageList*> m_packageListList;
    Installer           *m_installer;    // currently used installer
    Downloader          *m_downloader;
    InstallerProgress   *m_instProgress;
    GlobalConfig        *m_globalConfig;
    InstallerProgress   *m_instProgressBar;
    Database            *m_database;
//    PackageList         *m_availablePackages; 
	void dump(const QString &title=QString());
};

#endif
