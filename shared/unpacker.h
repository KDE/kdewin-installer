/****************************************************************************
**
** Copyright (C) 2008 Christian Ehrlicher <ch.ehrlicher@gmx.de>
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

#ifndef UNPACKER_H
#define UNPACKER_H

#include <QtCore/QObject>

#include "package.h"  // StringHash

class UPThread;
class InstallerProgress;
class Unpacker : public QObject
{
    Q_OBJECT
public:
    // dtor
    virtual ~Unpacker();
    // singleton
    static Unpacker *instance();
    // for user interaction
    void setProgress(InstallerProgress *progress);
    // unpack file
    bool unpackFile ( const QString &fn, const QString &destpath, const StringHash &pathRelocations );
    /// cancel file unpack
    void cancel();
    // get unpacked files
    QStringList getUnpackedFiles() const;
Q_SIGNALS:
    void done ( bool bOk );
    void error ( const QString &error );
protected Q_SLOTS:
    void setError ( const QString &error );
    void threadFinished ();
    void progressCallback ( const QString &s );
protected:
    InstallerProgress *m_progress;
    UPThread *m_thread;
    bool m_bRet;
    bool m_bFinished;
private:
    Unpacker ();

    friend class UnpackerSingleton;
};

#endif  // UNPACKER_H
