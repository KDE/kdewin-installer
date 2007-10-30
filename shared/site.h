/****************************************************************************
**
** Copyright (C) 2006-2007 Ralf Habacker. All rights reserved.
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

#ifndef SITE_H
#define SITE_H

#include <QString>
#include <QUrl>
#include <QStringList>
#include <QHash>

typedef QList<QUrl> QUrlList;
/**
 holds a site definition
*/
class Site
{

public:
    enum SiteType {SourceForge, ApacheModIndex};

    QString name() const { return m_name; }
    void setName(const QString &name)
    {
        m_name = name;
    }

    QUrl url() const { return m_url; }
    void setURL(const QUrl &url)
    {
        m_url = url;
    }

    const QUrlList &mirrors() const { return m_mirrors; }
    void addMirror(const QUrl &mirror)
    {
        if(!m_mirrors.contains(mirror))
            m_mirrors += mirror;
    }

    QStringList copies() const { return m_copies; }
    void addCopy(const QString &copy)
    {
        m_copies += copy;
    }

    SiteType Type() const { return m_type; }
    void setType(SiteType type)
    {
        m_type = type;
    }

    void addDependencies(const QString &package, const QStringList &deps);
    QStringList getDependencies(const QString &package);

    void setPackageNote(const QString &package, const QString &notes);
    QString packageNote(const QString &package);

	void setPackageLongNotes(const QString &package, const QString &note);
	QString packageLongNotes(const QString &package);

    void addExcludes(const QStringList &excludes);
    bool isExclude(const QString &package);

    QString notes() const { return m_notes; }
    void setNotes(const QString &notes)
    {
        m_notes = notes;
    }

    void dump(const QString &title=QString());
private:
    QString     m_name;
    QUrl        m_url;
    QString     m_notes;
    SiteType    m_type;
    QUrlList    m_mirrors;
    QStringList m_excludes;
    QStringList m_copies;
    QHash<QString, QStringList> m_dependencies;
    QHash<QString, QString> m_packageNotes;
    QHash<QString, QString> m_packageLongNotes;
};

#endif
