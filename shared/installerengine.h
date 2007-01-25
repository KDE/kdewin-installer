/****************************************************************************
**
** Copyright (C) 2005-2006 Ralf Habacker. All rights reserved.
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

class InstallerEngine
{
public:
    InstallerEngine(DownloaderProgress *progressBar,InstallerProgress *instProgressBar);
    void createMainPackagelist();
    bool downloadPackageLists();
    void stop();

#ifdef USE_GUI
    void setPageSelectorWidgetData(QTreeWidget *tree);
    void itemClickedPackageSelectorPage(QTreeWidgetItem *item, int column);
    bool downloadPackages(QTreeWidget *tree, const QString &category=QString());
    bool installPackages(QTreeWidget *tree, const QString &category=QString());
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
    const Settings &settings() const
    {
        return m_settings;
    }

private:
    QList <PackageList*> m_packageListList;
    QList <Installer*>   m_installerList;
    PackageList         *m_packageList;  // currently used packagelist
    Installer           *m_installer;    // currently used installer
    Downloader          *m_downloader;
    InstallerProgress   *m_instProgress;
    GlobalConfig        *m_globalConfig;
    InstallerProgress   *m_instProgressBar;
    Settings             m_settings;
};

#endif
