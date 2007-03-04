/****************************************************************************
**
** Copyright (C) 2007 Christian Ehrlicher <ch.ehrlicher@gmx.de>
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

#ifndef UNINSTALL_H
#define UNINSTALL_H

#include <QObject>

/* Uninstall class to uninstall a complete package defined by a manifest file */
class Uninstall : public QObject
{
    Q_OBJECT
public:
    Uninstall();
    ~Uninstall();

    bool uninstallPackage(const QString &rootDir, const QString &packName, bool bUseHashWhenPossible=true);
Q_SIGNALS:
    void removed(const QString &fileName);
    void warning(const QString &wrnMsg);
    void error(const QString &errMsg);
private:
    struct FileItem {
        QString fileName;
        QString hash;  

        FileItem(const QString &fn, const QString &h) { fileName = fn; hash = h; }
    };
};

#endif  // UNINSTALL_H
