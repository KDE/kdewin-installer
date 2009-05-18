/****************************************************************************
**
** Copyright (C) 2006-2009 Ralf Habacker <ralf.habacker@freenet.de>. 
** All rights reserved.
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

#include "debug.h"
#include "downloader.h"
#include "downloaderprogress.h"
#include "registry.h"
#include "selfinstaller.h"
#include "settings.h"

#include <windows.h>

#include <QCoreApplication>
#include <QtDebug>
#include <QDir>
#include <QProcess>
#include <QFileInfo>

#include <psapi.h>

#define REGISTRY_KEY_BASE "SOFTWARE\\Microsoft\\Windows\\CurrentVersion\\Uninstall\\"

SelfInstaller::SelfInstaller()
{
    char installerExePath[MAX_PATH+1];
    GetModuleFileNameA(NULL, installerExePath, MAX_PATH);
    m_currentExecutable.setFile(installerExePath);
    m_installRoot = Settings::instance().installDir();
    m_installedExecutable.setFile(m_installRoot + "/bin/" + m_currentExecutable.fileName());    
       
    m_kdeVersion = "4.2.0";
    m_kdeInstallKey = "kde420r";
    m_kdeReleaseMode = "Release";
    qDebug() << __FUNCTION__ << m_currentExecutable.absoluteFilePath();
    qDebug() << __FUNCTION__ << m_installedExecutable.absoluteFilePath();
}

SelfInstaller::~SelfInstaller()
{
}

bool SelfInstaller::isInstalled()
{

    bool ret = m_installedExecutable.exists();
    qDebug() << __FUNCTION__ << ret;
    return ret;
}

bool SelfInstaller::install()
{
    installExecutable();
    installResourceFiles();
//    installManifestFiles();
    installSoftwareControlPanelEntry();
    return true;
}

bool SelfInstaller::uninstall()
{
    uninstallExecutable();
    uninstallResourceFiles();
//    uninstallManifestFiles();
    uninstallSoftwareControlPanelEntry();
    return true;
}

bool SelfInstaller::installExecutable()
{
    if (!QFile::copy(m_currentExecutable.absoluteFilePath(), m_installedExecutable.absoluteFilePath()))
    {
        qCritical() << "could not copy installer executable into installation bin dir";
        return false;
    }
    return true;
}

bool SelfInstaller::installResourceFiles()
{
    if (!QFile::copy(":/installer.desktop",m_installRoot + "/share/applications/kde4/installer.desktop"))
    {
        qCritical() << "could not install installer desktop file";
        return false;
    }
    return true;
}

bool SelfInstaller::installSoftwareControlPanelEntry()
{
#if 0
    QString keyPath = REGISTRY_KEY_BASE + m_kdeInstallKey;
    
    // check if an  uninstall entry is available 
    bool ok;
    getWin32RegistryValue(hKEY_LOCAL_MACHINE, keyPath, "UninstallString", &ok );
    if (ok)
    {
        // check install path 
        qDebug() << "already installed"; 
        return true;
    }

    QString installName = "KDE " + m_kdeVersion + " " + m_kdeReleaseMode;
    setWin32RegistryValue(hKEY_LOCAL_MACHINE, keyPath, "UninstallString", m_installedExecutable.absoluteFilePath(), qt_String);
    setWin32RegistryValue(hKEY_LOCAL_MACHINE, keyPath, "InstallLocation", m_installRoot, qt_String);
    setWin32RegistryValue(hKEY_LOCAL_MACHINE, keyPath, "DisplayName", installName, qt_String);
    setWin32RegistryValue(hKEY_LOCAL_MACHINE, keyPath, "DisplayIcon", m_installedExecutable.absoluteFilePath()+",0", qt_String);
    setWin32RegistryValue(hKEY_LOCAL_MACHINE, keyPath, "DisplayVersion", m_kdeVersion, qt_String);
    setWin32RegistryValue(hKEY_LOCAL_MACHINE, keyPath, "Publisher", "The KDE Windows project", qt_String);
    setWin32RegistryValue(hKEY_LOCAL_MACHINE, keyPath, "URLInfoAbout", "http://windows.kde.org", qt_String);
    setWin32RegistryValue(hKEY_LOCAL_MACHINE, keyPath, "URLInfoAbout", "http://windows.kde.org", qt_String);
    setWin32RegistryValue(hKEY_LOCAL_MACHINE, keyPath, "NoModify", 1, qt_DWORD);
    setWin32RegistryValue(hKEY_LOCAL_MACHINE, keyPath, "NoRepair", 1, qt_DWORD);
    return true;
#else
    return false;
#endif
}
#if 0
bool SelfInstaller::installManifestFiles()
{
    QString keyPath = REGISTRY_KEY_BASE + m_kdeInstallKey;
    
    return true;
}
#endif
bool SelfInstaller::uninstallExecutable()
{
    return QFile::remove(m_installedExecutable.absoluteFilePath());
}

bool SelfInstaller::uninstallResourceFiles()
{
    return QFile::remove(m_installRoot + "/share/applications/kde4/installer.desktop");
}
 
bool SelfInstaller::uninstallSoftwareControlPanelEntry()
{
#if 0
    // get version based registry key kde<version>-(r|d)
    QString kdeInstallKey = "kde420r";

    QString keyPath = REGISTRY_KEY_BASE + kdeInstallKey;
    
    // check if an  uninstall entry is available 
    bool ok;
    getWin32RegistryValue(hKEY_LOCAL_MACHINE, keyPath, "UninstallString", &ok );
    if (ok)
    {
        return delWin32RegistryValue(hKEY_LOCAL_MACHINE, keyPath);
    }
    return true;
#else
    return false;
#endif
}

bool SelfInstaller::isRunningFromInstallRoot()
{
    qDebug() << __FUNCTION__;
    return isInstalled() && m_currentExecutable.absoluteFilePath() == m_installedExecutable.absoluteFilePath();
}

/// check if the currently running installer is started from the install root
bool SelfInstaller::isRunningFromTemporaryLocation()
{
    QString tempDir = QDir::tempPath();
    QFileInfo tempExecutable(tempDir + "/" + m_currentExecutable.fileName());
    bool ret = tempExecutable.absoluteFilePath() == m_currentExecutable.absoluteFilePath();
    qDebug() << __FUNCTION__ << ret;
    return ret;
}

void SelfInstaller::runFromTemporayLocation(const QStringList &arguments)
{
    qDebug() << __FUNCTION__;
    QString tempDir = QDir::tempPath();
    QString destPath = tempDir + "/" + m_currentExecutable.fileName();
    
    QFile::remove(destPath);
    if (QFile::copy(m_currentExecutable.absoluteFilePath(), destPath))
    {
        qDebug() << __FUNCTION__ << "2";
        QProcess::startDetached(destPath, arguments);
        exit(0);
    }
}
    
SelfInstaller &SelfInstaller::instance()
{
    static SelfInstaller selfInstall;
    return selfInstall;
}

QDebug &operator<<(QDebug &out, const SelfInstaller &c)
{
    out << "SelfInstaller ("
        << ")";
    return out;
}
