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

#ifndef UNINSTALLER_P_H
#define UNINSTALLER_P_H

#include <QThread>

class UIThread : public QThread
{
    Q_OBJECT
public:
    UIThread ( QObject *parent = 0 );
    virtual ~UIThread();

    void uninstallPackage(const QString &manifest, const QString &root);
    void cancel();
    bool retCode() const;
Q_SIGNALS:
    void progress ( const QString &filename );
    void warning ( const QString &warning );
    void error ( const QString &error );
protected:
    struct FileItem {
        QString fileName;
        QByteArray hash;  

        FileItem(const QString &fn, const QByteArray &h) { fileName = fn; hash = h; }
    };
protected:
    virtual void run();
    bool readManifestFile(QList<FileItem> &fileList);
protected:
    QString m_manifest;
    QString m_root;
    bool m_bCancel;
    bool m_bRet;
private:
    void start ( Priority priority = InheritPriority );
};

#endif  // UNINSTALLER_P_H
