/****************************************************************************
**
** Copyright (C) 2005-2007 Ralf Habacker. All rights reserved.
**
** This file is part of the KDE installer for windows
**
** SPDX-License-Identifier: LGPL-2.0-only
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


