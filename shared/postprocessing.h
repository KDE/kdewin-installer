/****************************************************************************
**
** Copyright (C) 2005-2011 Ralf Habacker. All rights reserved.
**
** This file is part of the KDE installer for windows
**
** SPDX-License-Identifier: LGPL-2.0-only
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
    void setSingleApplicationMode(const bool mode) { m_singleAppsInstallMode = mode; }
    /** for single application mode only */
    void setPackageName(const QString &name) { m_packageName = name; }
    /** for single application mode only */
    void setVersion(const QString &version) { m_packageVersion = version; }
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
    QString m_packageName;
    QString m_packageVersion;
    InstallerEngine *m_engine;
};

#endif // POSTPROCESSING_H
