/****************************************************************************
**
** Copyright (C) 2006 Ralf Habacker. All rights reserved.
**
** This file is part of the KDE installer for windows
**
** This file may be used under the terms of the GNU General Public
** License version 2.0 as published by the Free Software Foundation
** and appearing in the file LICENSE.GPL included in the packaging of
** this file.  Please review the following information to ensure GNU
** General Public Licensing requirements will be met:
** http://www.trolltech.com/products/qt/opensource.html
**
** If you are unsure which license is appropriate for your use, please
** review the following information:
** http://www.trolltech.com/products/qt/licensing.html or contact the
** sales department at sales@trolltech.com.
**
** This file is provided AS IS with NO WARRANTY OF ANY KIND, INCLUDING THE
** WARRANTY OF DESIGN, MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE.
**
****************************************************************************/

#ifndef SITE_H
#define SITE_H

#include <QString>
#include <QStringList>
#include <QHash>

/**
 holds a site definition 
*/
class Site
{

public:
    enum SiteType {SourceForge, ApacheModIndex};

    const QString &name() const { return m_name; }
    void setName(const QString &name)
    {
        m_name = name;
    }

    const QString &url() const { return m_url; }
    void setURL(const QString &url)
    {
        m_url = url;
    }

    const QStringList &mirrors() const { return m_mirrors; }
    void addMirror(const QString &mirror)
    {
        if(!m_mirrors.contains(mirror))
            m_mirrors += mirror;
    }

    SiteType Type() const { return m_type; }
    void setType(SiteType type)
    {
        m_type = type;
    }

    void addDependencies(const QString &package, const QStringList &deps);
    QStringList getDependencies(const QString &package);

    void addExcludes(const QStringList &excludes);
    bool isExclude(const QString &package);

    void dump(const QString &title=QString());
private:
    QString     m_name;
    QString     m_url;
    SiteType    m_type;
    QStringList m_mirrors;
    QStringList m_excludes;
    QHash<QString, QStringList> m_dependencies;
};

#endif
