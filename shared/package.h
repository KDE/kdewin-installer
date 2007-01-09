/****************************************************************************
**
** Copyright (C) 2005 Ralf Habacker. All rights reserved.
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
#ifndef PACKAGE_H
#define PACKAGE_H

#include <QObject>
#include <QString>

class Package
{

public:
    enum Type { BIN = 1 ,LIB = 2 ,DOC = 4 ,SRC = 8 };

    Package();
    Package(QString const &_name, QString const &_version);

    // FIXME: better use Q_PROPERTY here?
    const QString &Name() const
    {
        return name;
    }
    const QString &Version() const
    {
        return version;
    }
    void setName(QString const &_name)
    {
        name = _name;
    }
    void setVersion(QString const &_version)
    {
        version = _version;
    }
    void setPackageType(QString const &_type)
    {
        packagetype = _type;
    }
    void setType(const QString &typeString);
    bool setFromVersionFile(const QString &verString);
    QString toString(bool mode=false, const QString &delim="-");
    const QString getTypeAsString();

    const QString getFileName(Package::Type type);
    const QString getURL(Package::Type type, QString baseURL="");
    bool isEmpty()
    {
        return name == "";
    }
    void addInstalledTypes(const Package &pkg);
    static QString baseURL;

    // 0.5.3
    bool isInstalled(Package::Type type);

private slots:
    void logOutput();

private:
    QString name;
    QString version;
    QString packagetype;
    bool installedLIB;
    bool installedBIN;
    bool installedDOC;
    bool installedSRC;
};

#endif
