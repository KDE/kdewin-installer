/****************************************************************************
**
** Copyright (C) 2009 Ralf Habacker. All rights reserved.
**
** This file is part of the KDE installer for windows
**
** SPDX-License-Identifier: LGPL-2.0-only
**
****************************************************************************/
#ifndef SELFINSTALL_H
#define SELFINSTALL_H

#include <QString>
#include <QUrl>
#include <QFileInfo>
/**    
   \brief The SelfInstaller class handles self installation (and uninstallation) 
   of the installer into <install root>. 
   Self installing covers installing of the executable, embedded resource files 
   like desktop entries (for generating installer start menu entries) into recent
   kde install root directory tree and an uninstall entry in the software 
   control panel.    
   The class provides methods to detect the installation state, methods to 
   install the aboved mentioned executable, resource files and the software
   panel entries. 
   Additional in this class there are methods to get informations about the 
   path the currently running installer is started from. 
*/
class SelfInstaller
{
    public:
        /// constructor 
        SelfInstaller();

        /// destructor 
        ~SelfInstaller();

        /// check if installer is installed in install root
        bool isInstalled();

        /// install all installer components
        bool install();

        /// uninstall all installer components
        bool uninstall();

        /// check if the currently running installer is started from the install root
        bool isRunningFromInstallRoot();

        /// check if the currently running installer is started from temporary location
        bool isRunningFromTemporaryLocation();
        
        ///
        void runFromTemporayLocation(const QStringList &arguments);

        /// return singleton instance
        static SelfInstaller &instance();

    protected:
        /// install executable in install root
        bool installExecutable();

        /// install resource files
        bool installResourceFiles();

        /// add uninstall entry in software control panel
        bool installSoftwareControlPanelEntry();

        /// uninstall executable from install root
        bool uninstallExecutable();

        /// uninstall resource files
        bool uninstallResourceFiles();

        /// delete uninstall entry in software control panel
        bool uninstallSoftwareControlPanelEntry();

        /// create desktop file from template
        bool createDesktopFile(QByteArray &fileData);


        QFileInfo m_currentExecutable;
        QFileInfo m_installedExecutable;
        QFileInfo m_installedDesktopFile;
        QString m_installRoot;
        QString m_kdeVersion;
        QString m_kdeInstallKey;
        QString m_kdeReleaseMode;
    public:
        friend QDebug &operator<<(QDebug &out, const SelfInstaller &c);
};


#endif
