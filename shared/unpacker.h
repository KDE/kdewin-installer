/****************************************************************************
**
** Copyright (C) 2008 Christian Ehrlicher <ch.ehrlicher@gmx.de>
** All rights reserved.
**
** This file is part of the KDE installer for windows
**
** SPDX-License-Identifier: LGPL-2.0-only
**
****************************************************************************/

#ifndef UNPACKER_H
#define UNPACKER_H

#include "installerprogress.h"
#include "package.h"  // StringHash

#include <QObject>

class QEventLoop;
class UPThread;

/** 
\brief The class Unpacker provides unpackaging of downloaded package files 


*/
class Unpacker : public QObject
{
    Q_OBJECT
public:
    /// dtor
    virtual ~Unpacker();

    /// singleton
    static Unpacker *instance();

    /// for user interaction
    void setProgress ( InstallerProgress *progress );

    /// returns currently used Installer process instance
    InstallerProgress *progress();

    /// unpack file
    bool unpackFile ( const QString &fn, const QString &destpath, const StringHash &pathRelocations=StringHash() );

    /// cancel file unpack
    void cancel();

    /// get unpacked files
    QStringList unpackedFiles() const;

    /// get last error string
    QString lastError() const;
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
    QEventLoop *m_loop;
    QString m_lastError;
private:
    Unpacker ();

    friend class UnpackerSingleton;
};

#endif  // UNPACKER_H
