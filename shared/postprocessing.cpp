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

PostProcessing::PostProcessing(QObject *parent) : QObject(parent)
{
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
    emit numberOfCommands(5);
    runCommand(0,"updating mime database","update-mime-database",QStringList() << QDir::fromNativeSeparators(Settings::instance().installDir()) + "/share/mime");
    if (!m_shouldQuit)
        runCommand(1,"updating system configuration database","kbuildsycoca4");
    if (!m_shouldQuit)
        runCommand(2,"deleting old windows start menu entries","kwinstartmenu",QStringList() <<  "--remove");
    if (!m_shouldQuit)
        runCommand(3,"creating new windows start menu entries","kwinstartmenu");
    emit finished();
    return true;
}

void PostProcessing::stop()
{
    m_shouldQuit = true;
}
