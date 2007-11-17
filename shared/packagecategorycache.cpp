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

#include "packagecategorycache.h"

PackageCategoryCache::PackageCategoryCache()
{
}

void PackageCategoryCache::setNote(const QString &category, const QString &notes)
{
    m_data[category].notes = notes;
}

void PackageCategoryCache::addPackage(Package *pkg)
{
    foreach(QString category, pkg->categories())
        m_data[category].packages.append(pkg->name());
}

QStringList PackageCategoryCache::categories()
{
    QStringList keys;
    QMap<QString, CategoryListType>::ConstIterator i = m_data.begin();
    for (;i != m_data.end(); ++i) 
    {
        // @TODO fix me empoty categories should not be added
        if (!i.key().isEmpty())
            keys << i.key() + ":" + m_data[i.key()].notes;
    }
    return keys;
}

QList <Package *>PackageCategoryCache::packages(const QString &category, PackageList &list)
{
    QList <Package *> packages;
    if (!m_data.contains(category))
        return packages;
    foreach(QString pkgName, m_data[category].packages) 
    {
        Package *p = list.getPackage(pkgName);
        if (p)
            packages.append(p);
    }
    return packages;
}

void PackageCategoryCache::clear()
{
    m_data.clear();
}


QDebug &operator<<(QDebug &out, const PackageCategoryCache &c)
{
    out << "PackageCategoryCache ("
        << "size:" << c.m_data.size();
    QMap<QString, PackageCategoryCache::CategoryListType>::ConstIterator i = c.m_data.begin();
    for (;i != c.m_data.end(); ++i) 
        out << "( category:" << i.key() << "notes:" << c.m_data[i.key()].notes << "packages:" << c.m_data[i.key()].packages << ")";
    out << ")";
    return out;
}

PackageCategoryCache categoryCache;
