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

#ifndef PACKAGELIST_H
#define PACKAGELIST_H

#include <QObject>
#include "package.h"
#include "site.h"

class QTreeWidget;
class QTreeWidgetItem;
class QIODevice;
class Site;
class Database;

QStringList filterPackageFiles(const QStringList &list,const QString &mode);

class PackageList : public QObject
{
    Q_OBJECT

public:
    enum Type {SourceForge, ApacheModIndex};

    PackageList();
    virtual ~PackageList();
    void addPackage(const Package &package);
    void listPackages(const QString &title=QString());
    bool readFromFile(const QString &_fileName=QString());
    bool syncWithFile(const QString &_fileName=QString());
	bool syncWithDatabase(Database &database);
	bool readHTMLFromFile(const QString &fileName, PackageList::Type type=PackageList::SourceForge, bool append=false);
    bool readHTMLFromByteArray(const QByteArray &ba, PackageList::Type type=PackageList::SourceForge, bool append=false);
    bool writeToFile(QString const &fileName=QString::null);
    Package *getPackage(QString const &pkgName, const QByteArray &version = QByteArray());
    bool setInstalledPackage(const Package &pkg);

    int size() const
    {
        return m_packageList.size();
    }
    const QList <Package*> &packageList() const
    {
        return m_packageList;
    }
    void setConfigFileName(const QString &file)
    {
        m_configFile = '/' + file;
    }
    const QString &Name() const
    {
        return m_name;
    }
    void setName(const QString &name)
    {
        m_name = name;
    }
    const QString &notes() const
    {
        return m_notes;
    }
    void setNotes(const QString &notes)
    {
        m_notes = notes;
    }
    const QString &BaseURL() const
    {
        return m_baseURL;
    }
    void setBaseURL(const QString &baseURL)
    {
        m_baseURL = baseURL;
    }

    // sets the current Site to get access to dep & deny lists
    void setCurrentSite(Site *s)
    {
        m_curSite = s;
    }

    // 0.5.3
    bool hasConfig();
    void dump(const QString &title=QString());

signals:
    void configLoaded();

private:
    bool readHTMLInternal(QIODevice *ioDev, PackageList::Type type, bool append=false);
private:
    QList<Package*> m_packageList;
    QString m_root;
    QString m_configFile;
    QString m_name;
    QString m_baseURL;
    Site *m_curSite;
	QString m_notes;

    friend class Installer;
};

#endif
