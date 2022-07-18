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

#ifndef UNPACKER_P_H
#define UNPACKER_P_H

#include "package.h"

#include <QDir>
#include <QList>
#include <QRegExp>
#include <QString>
#include <QStringList>
#include <QThread>

class QFileInfo;
class UPThread : public QThread
{
    Q_OBJECT
public:
    UPThread ( QObject *parent = 0 );
    virtual ~UPThread();

    void unpackFile ( const QString &fn, const QString &destdir, const StringHash &pathRelocations );
    void cancel();
    bool retCode() const;
    QStringList getUnpackedFiles() const;
Q_SIGNALS:
    void progress ( const QString &filename );
    void error ( const QString &error );
protected:
    virtual void run();
    bool unzipFile();
    bool unbz2File();
    bool un7zipFile();
    void relocateFileName ( const QString &in, QString &out );
    bool makeSurePathExists ( const QFileInfo &fi, bool bIsDir );
    bool openNewFile( QFile &fi, const QString &newFileName );
protected:
    QString m_filename;
    QDir    m_destdir;
    StringHash m_pathRelocations;
    QStringList m_unpackedFiles;
    bool m_bCancel;
    bool m_bRet;
private:
    void start ( Priority priority = InheritPriority );
};

#endif  // UNPACKER_P_H
