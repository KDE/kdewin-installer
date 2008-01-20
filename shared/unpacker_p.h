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

#ifndef UNPACKER_P_H
#define UNPACKER_P_H

#include <QtCore/QDir>
#include <QtCore/QList>
#include <QtCore/QRegExp>
#include <QtCore/QString>
#include <QtCore/QStringList>
#include <QtCore/QThread>

class QFileInfo;
class UPThread : public QThread
{
    Q_OBJECT
public:
    UPThread ( QObject *parent = 0 );
    virtual ~UPThread();

    void unpackFile ( const QString &fn, const QString &destdir, const StringHash &pathRelocations );
    void cancel();
    QStringList getUnpackedFiles() const;
Q_SIGNALS:
    void done ( bool bOk );
    void progress ( const QString &filename );
    void error ( const QString &error );
protected:
    virtual void run();
    bool unzipFile();
    bool unbz2File();
    bool un7zipFile();
    bool unpackExe();
    void relocateFileName ( const QString &in, QString &out );
    bool makeSurePathExists ( const QFileInfo &fi, bool bIsDir );
protected:
    QString m_filename;
    QDir    m_destdir;
    StringHash m_pathRelocations;
    QStringList m_unpackedFiles;
    bool m_bCancel;
private:
    void start ( Priority priority = InheritPriority );
};

#endif  // UNPACKER_P_H
