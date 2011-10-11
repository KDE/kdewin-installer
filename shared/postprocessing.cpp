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

void PostProcessing::setSingleApplicationMode(const QString &customString)
{
    m_singleAppsInstallMode = true;
    m_customString = customString;
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

bool PostProcessing::start()
{
    if (m_engine->installedPackages() == 0 && m_engine->removedPackages() > 0)
    {
        emit numberOfCommands(1);
        emit commandStarted(0);
        emit commandStarted("deleting windows start menu entries");
        removeDirectory(m_engine->startMenuRootPath);
    }
    else {
        emit numberOfCommands(4);
        QStringList kwinStartmenuMainParameters;
        if (m_singleAppsInstallMode && m_engine->getStartMenuGeneratorVersion() >= 0x010100)
        {
            kwinStartmenuMainParameters << "--nocategories";
            if (!m_customString.isEmpty())
                kwinStartmenuMainParameters << "--set-root-custom-string" << m_customString;
        }
#if 0
        if (!m_singleAppsInstallMode && m_engine->getStartMenuGeneratorVersion() >= 0x010200)
            kwinStartmenuMainParameters << "--set-root-custom-string" << CompilerTypes::toString(Settings::instance().compilerType());
#endif
        runCommand(0,"updating mime database","update-mime-database",QStringList() << QDir::fromNativeSeparators(m_engine->root()) + "/share/mime");
        if (!m_shouldQuit)
            runCommand(1,"updating system configuration database","kbuildsycoca4", QStringList() << "--noincremental");
        if (!m_shouldQuit)
            runCommand(2,"deleting old windows start menu entries","kwinstartmenu",QStringList() << kwinStartmenuMainParameters << "--remove");
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
