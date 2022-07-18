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
