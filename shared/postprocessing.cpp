/****************************************************************************
**
** Copyright (C) 2005-2010 Ralf Habacker. All rights reserved.
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
#include "settings.h"

#include <QListWidget>
#include <QProcess>
#include <QCoreApplication>

PostProcessing::PostProcessing(QObject *parent) : QObject(parent), m_singleAppsInstallMode(false)
{
}

bool PostProcessing::checkKWinStartMenuVersion(const QByteArray &required)
{
    QStringList versions;
    QFileInfo f(Settings::instance().installDir()+"/bin/kwinstartmenu.exe");
    QProcess p;
    p.start(f.absoluteFilePath(), QStringList() << "--version");
    if (!p.waitForStarted(3000))
        return false;

    m_shouldQuit = false;
    while (p.state() == QProcess::Running && !m_shouldQuit)
    {
        QCoreApplication::processEvents();
    }
    bool noError = p.exitStatus() == QProcess::NormalExit && p.exitCode() == 0;
    if (!noError)
        return false;
        
    QByteArray out = p.readAllStandardOutput();
    QByteArray kwinStartMenuVersion; 
    QList<QByteArray> lines = out.split('\n');
    for (int i = 0; i < lines.size(); ++i) {
        if (lines.at(i).contains("winstartmenu"))
        {
            QList<QByteArray> pair = lines.at(i).split(':');
            kwinStartMenuVersion = pair[1].trimmed();
            break;
        }
    }
    if (kwinStartMenuVersion.isEmpty())
        return false;
    return required >= kwinStartMenuVersion;
}

bool PostProcessing::runCommand(int index, const QString &msg, const QString &app, const QStringList &params)
{
    QFileInfo f(Settings::instance().installDir()+"/bin/" + app + ".exe");
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
    emit numberOfCommands(4);
    QStringList kwinStartmenuMainParameters;
    if (m_singleAppsInstallMode && checkKWinStartMenuVersion("1.1"))
        kwinStartmenuMainParameters << "--nocategories";

    if (!m_singleAppsInstallMode && checkKWinStartMenuVersion("1.2"))
        kwinStartmenuMainParameters << "--set-root-custom-string" << CompilerTypes::toString(Settings::instance().compilerType());
        
    runCommand(0,"updating mime database","update-mime-database",QStringList() << QDir::fromNativeSeparators(Settings::instance().installDir()) + "/share/mime");
    if (!m_shouldQuit)
        runCommand(1,"updating system configuration database","kbuildsycoca4", QStringList() << "--noincremental");
    if (!m_shouldQuit)
        runCommand(2,"deleting old windows start menu entries","kwinstartmenu",QStringList() << kwinStartmenuMainParameters << "--remove");
    if (!m_shouldQuit)
        runCommand(3,"creating new windows start menu entries","kwinstartmenu", QStringList() << kwinStartmenuMainParameters << "--install");
    emit finished();
    return true;
}

void PostProcessing::stop()
{
    m_shouldQuit = true;
}
