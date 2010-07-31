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

//#define POSTPROCESSING_ALLOW_EVENTS
// allowing events during client application
// running requires special quit handling
// running simply quit() does now work

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
#ifndef POSTPROCESSING_ALLOW_EVENTS
    int noError = QProcess::execute( f.absoluteFilePath(), params) == 0;
#else
    QProcess p;
    p.start(f.absoluteFilePath(), params);
    if (!p.waitForStarted(3000))
        return false;

    while (p.state() == QProcess::Running)
    {
        QCoreApplication::processEvents();
    }
    bool noError = p.exitStatus() == QProcess::NormalExit && p.exitCode() == 0;
#endif
    qDebug() << "application finished";

    emit commandFinished(noError);
    return noError;
}

bool PostProcessing::run()
{
    emit numberOfCommands(5);
    runCommand(0,"updating mime database","update-mime-database",QStringList() << QDir::fromNativeSeparators(Settings::instance().installDir()) + "/share/mime");
    runCommand(1,"updating system configuration database","kbuildsycoca4");
    runCommand(2,"deleting old windows start menu entries","kwinstartmenu",QStringList() <<  "--remove");
    runCommand(3,"creating new windows start menu entries","kwinstartmenu");
    emit finished();
    return true;
}
