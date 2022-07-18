/****************************************************************************
**
** Copyright (C) 2005-2007 Ralf Habacker. All rights reserved.
**
** This file is part of the KDE installer for windows
**
** SPDX-License-Identifier: LGPL-2.0-only
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
