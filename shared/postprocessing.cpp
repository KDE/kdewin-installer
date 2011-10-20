/****************************************************************************
**
** Copyright (C) 2005-2011 Ralf Habacker. All rights reserved.
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

#include "postprocessing.h"
#include "installerengine.h"
#include "settings.h"

#include <QListWidget>
#include <QProcess>
#include <QCoreApplication>

PostProcessing::PostProcessing(InstallerEngine *engine, QObject *parent) : QObject(parent), m_singleAppsInstallMode(false), m_engine(engine)
{
}

bool PostProcessing::runCommand(int index, const QString &msg, const QString &app, const QStringList &params)
{
    QFileInfo f(m_engine->root()+"/bin/" + app + ".exe");
    qDebug() << "checking for app " << app << " - "  <<(f.exists() ? "found" : "not found");
    if (!f.exists())
        return false;

    emit commandStarted(index);
    emit commandStarted(msg);

    qDebug() << "running " << app << params;

    QProcess p;
    p.start(f.absoluteFilePath(), params);
    if (!p.waitForStarted(3000))
        return false;

    m_shouldQuit = false;
    while (p.state() == QProcess::Running && !m_shouldQuit)
    {
        QCoreApplication::processEvents();
    }
    bool noError = p.exitStatus() == QProcess::NormalExit && p.exitCode() == 0;

    qDebug() << "application finished" << m_shouldQuit;

    emit commandFinished(noError);
    return noError;
}

/**
 start() fetches from the installed package (which is either a single package or generic kde) the installed version
 and runs kwinstartmenu to save these values persistantly and install the start menu entries 
*/
bool PostProcessing::start()
{
    QStringList kwinStartmenuMainParameters;

    if (m_engine->installedPackages() == 0 && m_engine->removedPackages() > 0)
    {
        emit numberOfCommands(1);
        emit commandStarted(0);
        emit commandStarted("deleting windows start menu entries");
        removeDirectory(m_engine->startMenuRootPath);
    }
    else {
        emit numberOfCommands(4);
        runCommand(0,"updating mime database","update-mime-database",QStringList() << QDir::fromNativeSeparators(m_engine->root()) + "/share/mime");
        if (!m_shouldQuit)
            runCommand(1,"updating system configuration database","kbuildsycoca4", QStringList() << "--noincremental");

        /* removing entries is still here because there were update problems with language switch*/
        if (!m_shouldQuit)
            runCommand(2,"deleting old windows start menu entries","kwinstartmenu",QStringList() << kwinStartmenuMainParameters << "--remove");

        int kwinstartmenuVersion = m_engine->getAppVersion("kwinstartmenu");

        // 4.7.0
        if (kwinstartmenuVersion > 0x00010200)
        {
            if (m_singleAppsInstallMode)
                kwinStartmenuMainParameters << "--disable-categories";
            else
                kwinStartmenuMainParameters << "--enable-categories";
        }
        // 4.5.4
        else if (kwinstartmenuVersion >= 0x00010100)
        {
            if (m_singleAppsInstallMode)
                kwinStartmenuMainParameters << "--nocategories";
        }

        // > 4.7.0
        if (kwinstartmenuVersion >= 0x00010400)
        {
            /* we set the package name and version here to give kwinstartmenu a chance to remove previous entries */
            kwinStartmenuMainParameters << "--set-name-string" << m_packageName;
            if (m_singleAppsInstallMode)
            {
                kwinStartmenuMainParameters << "--set-version-string" << (!m_packageVersion.isEmpty() ?
                    m_packageVersion : m_engine->getAppVersionString(m_packageName, m_packageName));
            }
            else
            {
                kwinStartmenuMainParameters << "--set-version-string" << (!m_packageVersion.isEmpty() ?
                    m_packageVersion :  m_engine->getAppVersionString("kde4-config", "KDE"));
            }
        }
        // 4.7.0
        else if (kwinstartmenuVersion >= 0x00010200)
        {
            kwinStartmenuMainParameters << "--set-root-custom-string" << m_packageName;
        }

        if (!m_shouldQuit)
            runCommand(3,"creating new windows start menu entries","kwinstartmenu", QStringList() << kwinStartmenuMainParameters << "--install");
    }
    emit finished();
    return true;
}

void PostProcessing::stop()
{
    m_shouldQuit = true;
}
