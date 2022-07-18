/****************************************************************************
**
** Copyright (C) 2006-2008 Ralf Habacker <ralf.habacker@freenet.de>. 
** All rights reserved.
**
** This file is part of the KDE installer for windows
**
** SPDX-License-Identifier: LGPL-2.0-only
**
****************************************************************************/

#include "config.h"
#include "debug.h"
#include "downloader.h"
#include "downloaderprogress.h"
#include "installerupdate.h"
#include "settings.h"

#ifdef Q_OS_WIN32
#include <windows.h>
#else
#define Sleep sleep
#endif

#include <QCoreApplication>
#include <QtDebug>
#include <QProcess>
#include <QFileInfo>

#ifdef Q_OS_WIN32
#include <psapi.h>
#else
#include <unistd.h>
#endif

QUrl installerUpdateUrl("http://download.kde.org/" INSTALLER_URL_PATH "/");
QByteArray installerName ="kdewin-installer-gui-";

bool isProcessRunning(int pid)
{
#ifdef Q_OS_WIN32
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
#endif
    return false;
}


InstallerUpdate::InstallerUpdate()
{
    setCurrentVersion(VERSION_PATCH);
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
    QByteArray data;
    if (!Downloader::instance()->fetch(installerUpdateUrl,data))
        return false;
   
    QByteArray tempVersion;
    QByteArray tempName;
    foreach(QByteArray line, data.split('\n')) {
        if (!line.contains(installerName ) || line.contains(installerName + "latest"))
            continue;
        int a = line.indexOf("href=\"") + 6;
        int b = line.indexOf("\">",a);
        const QByteArray name = line.mid(a,b-a);
        if (name.endsWith(".exe")) {
            QByteArray version = name;
            version.replace(installerName,"");
            version.replace(".exe","");
            // set highest version
            if (tempVersion < version) 
            {
                tempVersion = version;
                tempName =  name;
            }
        }
    }
    if (tempVersion > m_currentVersion)
    {
        setNewVersion(tempVersion);
        setUrl(installerUpdateUrl.toString() + tempName);
    }

    bool result = m_url.isValid() 
        && !m_newVersion.isEmpty()
        && !m_currentVersion.isEmpty() 
        && m_newVersion > m_currentVersion;
    if (result)
        qDebug() << "update" << m_newVersion << "at" << m_url << "found";
    else
        qDebug() << "no update found";
    return result;
}

void InstallerUpdate::setUrl(const QUrl &url)
{
    m_url = url;
    QString installerExePath = QCoreApplication::applicationFilePath();
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
#ifdef Q_OS_WIN32
        QString processID = QString::number(GetCurrentProcessId());
#else
        QString processID = QString::number(getpid());
#endif
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
    qDebug() << "oldPath" << oldPath;
    qDebug() << "oldPid" << oldPid;

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
