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
    int ret = QProcess::execute( f.absoluteFilePath(), params);
    emit commandFinished(ret == 0);
    return ret == 0;
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
