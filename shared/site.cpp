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

void Site::dump(const QString &title)
{
    DUMP_HEADER(title,"Site");
    qDebug() << "m_name  " << m_name;  
    qDebug() << "m_url   " << m_url;   
    qDebug() << "m_type  " << m_type;  
    qDebug() << "m_mirror" << m_mirrors.join(" ");
    qDebug() << "m_excludes" << m_excludes.join(" ");
    // FIXME: dump dependencies 
    DUMP_FOOTER(title,"Site");

}
