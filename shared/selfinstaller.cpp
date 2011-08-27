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

#include "config.h"
#include "debug.h"
#include "downloader.h"
#include "downloaderprogress.h"
#include "hash.h"
#include "registry.h"
#include "selfinstaller.h"
#include "settings.h"

#ifdef Q_WS_WIN
#include <windows.h>
#endif

#include <QCoreApplication>
#include <QtDebug>
#include <QDir>
#include <QProcess>
#include <QFileInfo>

#ifdef Q_WS_WIN
#include <psapi.h>
#endif

#define REGISTRY_KEY_BASE "SOFTWARE\\Microsoft\\Windows\\CurrentVersion\\Uninstall\\"

SelfInstaller::SelfInstaller()
{
    QString installerExePath = QCoreApplication::applicationFilePath();
    m_currentExecutable.setFile(installerExePath);
    m_installRoot = Settings::instance().installDir();
    m_installedExecutable.setFile(m_installRoot + "/bin/" + m_currentExecutable.fileName());    
    m_installedDesktopFile.setFile(m_installRoot + QString("/share/applications/kde4/%1.desktop").arg(m_installedExecutable.completeBaseName()));
       
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

    /**
     @todo compare sha1sum of running installa agains installed installer 
     because they have the same name 
    */ 
    if (!(m_installedExecutable.exists() && m_installedDesktopFile.exists()))
        return false;

    Hash hash(Hash::SHA1);
    QByteArray a = hash.hash(m_currentExecutable.absoluteFilePath()).toHex();
    QByteArray b = hash.hash(m_installedExecutable.absoluteFilePath()).toHex();
    if (a != b)
        return false;
    QByteArray data;
    if (!createDesktopFile(data))
        return false;
    a = hash.hash(data).toHex();
    b = hash.hash(m_installedDesktopFile.absoluteFilePath()).toHex();
    bool ret = a == b;        
    
    // @todo check if desktop file is there and have the correct content. 

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
    // to move file to temporary dir
    QString backupFileName = m_installedExecutable.absoluteFilePath()+".bak";
    QFile::remove(backupFileName);
    if (QFile::exists(m_installedExecutable.absoluteFilePath()))
    {
        qDebug() << "old installer found, rename it";
        QFile::rename(m_installedExecutable.absoluteFilePath(), backupFileName);
    }

    if (!QFile::copy(m_currentExecutable.absoluteFilePath(), m_installedExecutable.absoluteFilePath()))
    {
        qCritical() << "could not copy installer executable into installation bin dir";
        return false;
    }
    return true;
}

bool SelfInstaller::installResourceFiles()
{
    // move file to temporary dir
    QString backupFileName = m_installedDesktopFile.absoluteFilePath()+".bak";

    QFile::remove(backupFileName);
    if (QFile::exists(m_installedDesktopFile.absoluteFilePath()))
    {
        qDebug() << "old desktop file found, rename it";
        QFile::rename(m_installedDesktopFile.absoluteFilePath(), backupFileName);
    }

    QByteArray data;
    if (!createDesktopFile(data))
    {
        qCritical() << "could not create desktop file content";
        return false;
    }

    QFile outFile(m_installedDesktopFile.absoluteFilePath());

    if (!outFile.open(QIODevice::WriteOnly | QIODevice::Text))
    {
        qCritical() << "could not create desktop file" << m_installedDesktopFile.absoluteFilePath();
        return false;
    }

    outFile.write(data);

    return true;
}

bool SelfInstaller::createDesktopFile(QByteArray &fileData)
{
    QString appName = QLatin1String("KDE Installer " VERSION_PATCH);
    QFile inFile(":/installer.desktop");
    if (!inFile.open(QIODevice::ReadOnly | QIODevice::Text))
        return false;

    while (!inFile.atEnd()) {
        QByteArray line = inFile.readLine();
        line.replace("##TARGET_FILENAME##",m_installedExecutable.fileName().toLatin1());
        line.replace("##DESKTOP_APPNAME##",appName.toLatin1());
        fileData.append(line);
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
    return QFile::remove(m_installedDesktopFile.absoluteFilePath());
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
    qDebug() << __FUNCTION__ << "currentExecutable" << m_currentExecutable.absoluteFilePath() << "installedExecutable" << m_installedExecutable.absoluteFilePath();
    return m_currentExecutable.absoluteFilePath().toLower() == m_installedExecutable.absoluteFilePath().toLower();
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
