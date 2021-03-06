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

#include <QProcess>
#include "package.h"
#include "installerprogress.h"
#include "typehelper.h"

class Database;
class PackageList;
class Uninstaller;
class Unpacker;

class Installer : public QObject
{
    Q_OBJECT

public:
    enum InstallerType { Standard, GNUWIN32 };
    Installer(QObject *parent = 0);
    virtual ~Installer();
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

    /// install package item with specified type
    bool install(Package *pkg, const FileTypes::Type type);

    bool uninstall(const QString &pathToManifest);

    InstallerProgress *progress();

    void setRoot(const QString &root);
    const QString root() const { return m_root; }
    void cancel();

    bool handlePostInstall(const QString &postInstallFile);

Q_SIGNALS:
    void error(const QString &);
    
protected slots:
    void slotError(const QString &);

protected:
    bool installExecutable(Package *pkg, FileTypes::Type type);
    bool installMsiPackage(Package *pkg, FileTypes::Type type);
    bool createManifestFile();
    bool createManifestFileForExecutable();
    bool createQtConfigFile();

    QString m_root;
    QString configFile;
    InstallerType m_type;
    QStringList m_files;
    Database *m_database;

    Package* m_packageToInstall;
    FileTypes::Type m_installType;
};

#endif


