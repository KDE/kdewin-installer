/****************************************************************************
**
** Copyright (C) 2007 Ralf Habacker. All rights reserved.
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

#ifndef DATABASE_H
#define DATABASE_H

#include <QObject>
class Package;

class Database: public QObject
{
    Q_OBJECT

public:
    Database();
    virtual ~Database();
    bool readFromDirectory(const QString &_fileName=QString());
    bool syncWithFile(const QString &_fileName=QString());
    void addPackage(const Package &package);
    void listPackages(const QString &title=QString());
    Package *getPackage(const QString &pkgName, const QByteArray &version=QByteArray());

    int size() const
    {
        return m_database.size();
    }
    const QList <Package*> &database() const
    {
        return m_database;
    }
    void dump(const QString &title=QString());
 
signals:
    void loadedConfig();

private:
    QList<Package*> m_database;
};

#endif
