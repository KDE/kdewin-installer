/****************************************************************************
**
** Copyright (C) 2006-2007 Ralf Habacker. All rights reserved.
**
** This file is part of the KDE installer for windows
**
** SPDX-License-Identifier: LGPL-2.0-only
**
****************************************************************************/

#ifndef SITE_H
#define SITE_H

#include "hash.h"

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
    /// @deprecated we use the autoparser 
    enum SiteType {ApacheModIndex, Ftp, JenkinsXml, SourceForge, SourceForgeMirror };

    Site();

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

    QUrl listURL() const { return m_listUrl; }
    void setListURL(const QUrl &url)
    {
        m_listUrl = url;
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

    bool setType(const QString &type);

    Hash &hashType() { return m_hashType; }

    void addDependencies(const QString &package, const QStringList &deps);
    QStringList getDependencies(const QString &package);

    void setCategoryNotes(const QString &category, const QString &notes);
    QString &categoryNotes(const QString &category);

    void setPackageHomeUrl(const QString &package, const QString &url);
    QString packageHomeUrl(const QString &package);

    void setPackageNote(const QString &package, const QString &notes);
    QString packageNote(const QString &package);

    void setPackageLongNotes(const QString &package, const QString &note);
    QString packageLongNotes(const QString &package);

    void setPackageCategories(const QString &package, const QStringList &categories);
    const QStringList &packageCategories(const QString &package);

    void addExcludes(const QStringList &excludes);
    bool isExclude(const QString &package);

    QString notes() const { return m_notes; }
    void setNotes(const QString &notes)
    {
        m_notes = notes;
    }

private:
    QString     m_name;
    QUrl        m_listUrl; 
    QUrl        m_url;
    QString     m_notes;
    SiteType    m_type;
    QUrlList    m_mirrors;
    QStringList m_excludes;
    QStringList m_copies;
    Hash        m_hashType;
    QHash<QString, QStringList> m_dependencies;
    QHash<QString, QStringList> m_packageCategories;
    QHash<QString, QString> m_packageNotes;
    QHash<QString, QString> m_packageLongNotes;
    QHash<QString, QString> m_packageHomeUrl;
    friend QDebug &operator<<(QDebug&, const Site &);
    friend QDebug &operator<<(QDebug&, const SiteType &);
};




#endif
