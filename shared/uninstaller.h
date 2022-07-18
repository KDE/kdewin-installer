/****************************************************************************
**
** Copyright (C) 2007-2008 Christian Ehrlicher <ch.ehrlicher@gmx.de>
** All rights reserved.
**
** This file is part of the KDE installer for windows
**
** SPDX-License-Identifier: LGPL-2.0-only
**
****************************************************************************/

#ifndef UNINSTALLER_H
#define UNINSTALLER_H

#include <QObject>

/* Uninstall class to uninstall a complete package defined by a manifest file */
class QEventLoop;
class UIThread;
class InstallerProgress;
class Uninstaller : public QObject
{
    Q_OBJECT
public:
    // dtor
    virtual ~Uninstaller();
    // singleton
    static Uninstaller *instance();
    // for user interaction
    void setProgress ( InstallerProgress *progress );
    // uninstall a package
    bool uninstallPackage ( const QString &pathToManifest, const QString &root );
    // cancel uninstalling
    void cancel();
Q_SIGNALS:
    void done ( bool bOk );
    void error ( const QString &error );
    void warning ( const QString &warning );
protected Q_SLOTS:
    void setError ( const QString &error );
    void setWarning ( const QString &warning );
    void threadFinished ();
    void progressCallback ( const QString &s );
protected:
    InstallerProgress *m_progress;
    UIThread *m_thread;
    bool m_bRet;
    bool m_bFinished;
    QEventLoop *m_loop;
private:
    Uninstaller();

    friend class UninstallerSingleton;
};

#endif  // UNINSTALL_H
