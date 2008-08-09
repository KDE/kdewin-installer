/****************************************************************************
**
** Copyright (C) 2005-2007 Ralf Habacker. All rights reserved.
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
#ifndef PACKAGECATEGORYCACHE_H
#define PACKAGECATEGORYCACHE_H

#include <QString>
#include <QStringList>
#include <QList>

#include "package.h"
class PackageList;
class Database;


class PackageCategoryCache 
{
    public:
        typedef struct {
            QString notes;
            QList<QString> packages;
        } CategoryListType;
        PackageCategoryCache();
        void addPackage(Package *pkg);
        void addPackage(const QString &category, const QString &pkgName);
        void setNote(const QString &category, const QString &notes);
        QStringList categories();
        QList <QString>packages(const QString &category) { return m_data[category].packages; }
        QList <Package *>packages(const QString &category, PackageList &list);
        QList <Package *>packages(const QString &category, Database &list);
        /// clear all values
        void clear();

    private:
        QMap<QString,CategoryListType> m_data;
    friend QDebug &operator<<(QDebug &, const PackageCategoryCache &);
};

extern PackageCategoryCache categoryCache;

#endif
