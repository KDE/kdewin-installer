/****************************************************************************
**
** Copyright (C) 2007 Christian Ehrlicher <Ch.Ehrlicher@gmx.de>
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

#include <QDebug>

#include "config.h"
#include "site.h"

void Site::addDependencies(const QString &_package, const QStringList &deps)
{
    QString package = _package.toLower();
    QStringList sl;

    if(m_dependencies.contains(package))
        sl = m_dependencies.find(package).value();

    QStringList::ConstIterator it = deps.constBegin();
    for( ; it != deps.constEnd(); ++it) {
        QString s = (*it).toLower();
        if(!sl.contains(s))
            sl += s;
    }
    m_dependencies.insert(package, sl);
}

QStringList Site::getDependencies(const QString &_package)
{
    QString package = _package.toLower();
    if(m_dependencies.contains(package))
        return m_dependencies.find(package).value();
    return QStringList();
}

void Site::setPackageNote(const QString &package, const QString &note)
{
    m_packageNotes[package] = note;
}

QString Site::packageNote(const QString &package)
{
    return m_packageNotes[package];
}

void Site::setPackageLongNotes(const QString &package, const QString &note)
{
    m_packageLongNotes[package] = note;
}

QString Site::packageLongNotes(const QString &package)
{
    return m_packageLongNotes[package];
}

void Site::addExcludes(const QStringList &excludes)
{
    QStringList::ConstIterator it = excludes.constBegin();
    for( ; it != excludes.constEnd(); ++it) {
        QString s = (*it).toLower();
        if(!m_excludes.contains(s))
            m_excludes += s;
    }
}

bool Site::isExclude(const QString &_package)
{
    QString package = _package.toLower();
    return (m_excludes.contains(package));
}

QDebug &operator<<(QDebug &out, const Site::SiteType &c)
{
    switch(c) {
        case Site::SourceForge:    out << "SourceForge"; break;
        case Site::ApacheModIndex: out << "ApacheModIndex"; break;
        default:             out << c;
    }
    return out;
}

QDebug &operator<<(QDebug &out, const Site &c)
{
    out << "Site ("
        << "m_name" << c.m_name
        << "m_url" << c.m_url
        << "m_type" << c.m_type;

    Q_FOREACH(const QUrl &url, c.m_mirrors)
        out << "m_mirror" << url;

    out << "m_excludes" << c.m_excludes.join(" ")
        << "m_dependencies" << c.m_dependencies
        << ")";
    return out;
}

