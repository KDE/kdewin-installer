/****************************************************************************
**
** Copyright (C) 2007 Christian Ehrlicher <Ch.Ehrlicher@gmx.de>
** Copyright (C) 2007 Ralf Habacker <ralf.habacker@freenet.de>
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

#include <QDebug>

Site::Site() : m_hashType(Hash::None)
{
}

void Site::addDependencies(const QString &_package, const QStringList &deps)
{
    QString package = _package.toLower();
    QStringList sl;

    if(m_dependencies.contains(package))
        sl = m_dependencies.find(package).value();

    Q_FOREACH( const QString &s, deps ) {
        if(!sl.contains(s.toLower()))
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

void Site::setPackageHomeUrl(const QString &package, const QString &url)
{
    m_packageHomeUrl[package] = url;
}

QString Site::packageHomeUrl(const QString &package)
{
    return m_packageHomeUrl[package];
}

bool Site::setType(const QString &type)
{
    if (type == "apachemodindex")
        setType(Site::ApacheModIndex);
    else if (type == "sourceforge")
        setType(Site::SourceForge);
    else if (type == "sourceforge-mirror")
        setType(Site::SourceForgeMirror);
    else if (type == "ftp")
        setType(Site::Ftp);
    else if (type == "jenkinsxml")
        setType(Site::JenkinsXml);
    else
    {
        setType(Site::ApacheModIndex);
        return false;
    }
    return true;
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

void Site::setPackageCategories(const QString &package, const QStringList &categories)
{
    m_packageCategories[package] = categories;
}

const QStringList &Site::packageCategories(const QString &package)
{
    return m_packageCategories[package];
}

void Site::addExcludes(const QStringList &excludes)
{
    Q_FOREACH( const QString &s, excludes ) {
        if(!m_excludes.contains(s.toLower()))
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
        case Site::ApacheModIndex:    out << "ApacheModIndex"; break;
        case Site::Ftp:               out << "FTP"; break;
        case Site::JenkinsXml:        out << "JenkinsXml"; break;
        case Site::SourceForge:       out << "SourceForge"; break;
        case Site::SourceForgeMirror: out << "SourceForgeMirror"; break;
        default:                      out << "unknown type";
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

