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

#ifndef POSTPROCESSING_H
#define POSTPROCESSING_H

#include <QObject>
#include <QStringList>

class InstallerEngine;

/**
 handle postprocessing 
*/ 
class PostProcessing : public QObject
{
    Q_OBJECT
public:
    PostProcessing(InstallerEngine *engine, QObject *parent = 0);
    bool start();
    void stop();

signals:
    void numberOfCommands(int count);
    void commandStarted(int index);
    void commandStarted(const QString &text);
    void commandFinished(bool failed);
    void finished();

public slots:


protected:
    bool runCommand(int index, const QString &msg, const QString &app, const QStringList &params=QStringList());
    bool checkKWinStartMenuVersion(const QByteArray &required);

    bool m_shouldQuit;
    bool m_singleAppsInstallMode;
    InstallerEngine *m_engine;
};

#endif // POSTPROCESSING_H
