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

#ifndef INSTALLER_H
#define INSTALLER_H

#include "package.h"

class Database;
class PackageList;
class InstallerProgress;

class Installer : public QObject
{
    Q_OBJECT

public:
    enum InstallerType { Standard, GNUWIN32 };
    Installer(InstallerProgress *progress=0);
    ~Installer();
    InstallerType Type() const
    {
        return m_type;
    }
    void setType(InstallerType type)
    {
        m_type = type;
    }

    Database *database() { return m_database; }
    void setDatabase(Database *database) { m_database = database; }
    bool install(const QString &fileName, const StringHash &pathRelocations=StringHash());
    // installPackage(Package *pkg)
    //  bool readFromFile(QString const &fileName);
    //  bool writeToFile(QString const &fileName);
    //bool loadConfig(const QString &destdir=QString());
    bool loadConfig();

    bool isEnabled();
    void setRoot(const QString &root);
    const QString root() const { return m_root; }

public slots:
    void updatePackageList();

protected:
    bool unzipFile(const QString &destpath, const QString &zipFile, const StringHash &pathRelocations=StringHash());
    bool un7zipFile(const QString &destpath, const QString &zipFile);
    void setError(const QString &str);
    bool createQtConfigFile();

    QString m_root;
    QString configFile;
    InstallerProgress *m_progress;
    InstallerType m_type;
    QStringList m_files;
    Database *m_database;
};

#endif


