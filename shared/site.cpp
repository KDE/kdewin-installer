/****************************************************************************
**
** Copyright (C) 2007 Christian Ehrlicher <Ch.Ehrlicher@gmx.de>
** All rights reserved.
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
