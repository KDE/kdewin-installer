/****************************************************************************
**
** Copyright (C) 2006-2008 Ralf Habacker <ralf.habacker@freenet.de>. 
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
#include "installerupdate.h"
#include "settings.h"

#include <windows.h>

#include <QCoreApplication>
#include <QtDebug>
#include <QProcess>
#include <QFileInfo>

#include <psapi.h>

bool isProcessRunning(int pid)
{
    DWORD aProcesses[1024], cbNeeded, cProcesses;
    unsigned int i;

    if ( !EnumProcesses( aProcesses, sizeof(aProcesses), &cbNeeded ) )
        return false;

    // Calculate how many process identifiers were returned.

    cProcesses = cbNeeded / sizeof(DWORD);

    // Print the name and process identifier for each process.

    for ( i = 0; i < cProcesses; i++ ) 
    {
        if (aProcesses[i] == pid)
            return true;
    }
    return false;
}


InstallerUpdate::InstallerUpdate()
{
}

InstallerUpdate::~InstallerUpdate()
{
}

void InstallerUpdate::setCurrentVersion(const QString &version)
{ 
    if (!version.contains("-"))
        m_currentVersion = version + "-0";
    else
        m_currentVersion = version;
}

void InstallerUpdate::setNewVersion(const QString &version) 
{
    if (!version.contains("-"))
        m_newVersion = version + "-0";
    else
        m_newVersion = version;
}

bool InstallerUpdate::isUpdateAvailable()
{
    bool result = m_url.isValid() 
        && !m_newVersion.isEmpty()
        && !m_currentVersion.isEmpty() 
        && m_newVersion > m_currentVersion;
    if (result)
        qDebug() << "update found" << "- old" << m_currentVersion << "new" << m_newVersion;
    else
        qDebug() << "no update found";
    return result;
}

void InstallerUpdate::setUrl(const QUrl &url)
{
    m_url = url;
    char installerExePath[MAX_PATH+1];
    GetModuleFileNameA(NULL, installerExePath, MAX_PATH);
    QFileInfo pi(installerExePath); 
    m_currentInstallerFilePath = installerExePath;
    QFileInfo fi(m_url.path()); 
    m_localFilePath = pi.absolutePath() + "/" + fi.fileName();
}

bool InstallerUpdate::fetch()
{
    if (m_localFilePath.isEmpty() || !m_url.isValid())
    {
        qCritical() << "no url or local file path set";
        return false;
    }
    if (m_url.scheme() == "file")
    {
        QFile a(m_url.toLocalFile());
        if (!a.exists())
        {
            qCritical() << "could not find file to copy" << m_url.toLocalFile();
            return false;
        }
        if (!a.copy(m_localFilePath)) 
        {
            qCritical() << "could not copy file from" << m_url.toLocalFile() << "to" << m_localFilePath;
            return false;
        }
        return true;
    }
    else if (!Downloader::instance()->fetch(m_url,m_localFilePath))
    {
        qCritical() << "could not fetch new installer" << m_url;
        return false;
    }
    return true;
}

bool InstallerUpdate::run()
{
    if (m_url.isValid())
    {
        QString processID = QString::number(GetCurrentProcessId());
        QProcess::startDetached(m_localFilePath,QStringList() << "--finish-update" << m_currentInstallerFilePath << processID);
        QCoreApplication::quit();
        exit(0);
		// never reached;
    }
    return false;
}

bool InstallerUpdate::finish(const QStringList &args, int startIndex)
{
    QString oldPath = args.at(startIndex);
    int oldPid = args.at(startIndex+1).toInt();

    int interval = 500;
    int oneSec = 1000;
    int maxSecs = 10;
    if (oldPid) 
    {
        for (int i=maxSecs*oneSec/interval; i > 0; i++)
        {
            if (!isProcessRunning(oldPid))
            {
                qDebug() << "process" << oldPid << "is terminated";
                break;
            }
            Sleep(interval);
        }
    }
    else 
       qDebug() << "no process id given";

    if (!oldPath.isEmpty())
    {
        qDebug() << "removing old installer executable" << oldPath;
        return QFile::remove (oldPath);
    }
    return true;
}

InstallerUpdate &InstallerUpdate::instance()
{
    static InstallerUpdate InstallerUpdate;
    return InstallerUpdate;
}

QDebug &operator<<(QDebug &out, const InstallerUpdate &c)
{
    out << "InstallerUpdate ("
        << "m_url:" << c.m_url
        << "m_newVersion:" << c.m_newVersion
        << "m_currentVersion:" << c.m_currentVersion
        << "m_installerExePath:" << c.m_currentVersion
        << ")";
    return out;
}
