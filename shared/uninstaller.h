/****************************************************************************
**
** Copyright (C) 2007-2008 Christian Ehrlicher <ch.ehrlicher@gmx.de>
** All rights reserved.
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

#ifndef UNINSTALLER_H
#define UNINSTALLER_H

#include <QObject>

/* Uninstall class to uninstall a complete package defined by a manifest file */
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
    void setProgress(InstallerProgress *progress);
    // uninstall a package
    bool uninstallPackage(const QString &pathToManifest, const QString &root);
    // cancel uninstalling
    void cancel();
Q_SIGNALS:
    void done ( bool bOk );
    void error ( const QString &error );
    void warning ( const QString &warning );
protected Q_SLOTS:
    void setError ( const QString &error );
    void setWarning ( const QString &warning );
    void threadFinished ( bool bOk );
    void progressCallback ( const QString &s );
protected:
    InstallerProgress *m_progress;
    UIThread *m_thread;
    bool m_bFinished;
    bool m_bRet;
private:
    Uninstaller();

    friend class UninstallerSingleton;
};

#endif  // UNINSTALL_H
