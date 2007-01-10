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

#ifndef PACKAGELIST_H
#define PACKAGELIST_H

#include <QObject>
#include "package.h"
#include "site.h"

class Downloader;
class Installer;
class QTreeWidget;
class QTreeWidgetItem;
class QIODevice;

QStringList filterPackageFiles(const QStringList &list,const QString &mode);

class PackageList : public QObject
{
    Q_OBJECT

public:
    enum Type {SourceForge, ApacheModIndex};

    PackageList(Downloader *downloader= NULL);
    virtual ~PackageList();

    void addPackage(Package const &package);
    bool updatePackage(Package &pkg);
    Package *getPackage(QString const &pkgName, const QByteArray &version = QByteArray());

    void listPackages(const QString &title=QString::null);

    bool readFromFile(const QString &_fileName=QString::null);
    bool readHTMLFromFile(const QString &fileName, PackageList::Type type=PackageList::SourceForge);
    bool readHTMLFromByteArray(const QByteArray &ba, PackageList::Type type=PackageList::SourceForge);

    bool writeToFile(QString const &fileName=QString::null);

    bool downloadPackage(const QString &pkgName);
    bool installPackage(const QString &pkgName);

    int size();

    QList <Package> *packageList()
    {
        return m_packageList;
    }

    bool hasConfig();
    void setConfigFileName(const QString &file)
    {
        m_configFile = "/" + file;
    }

    QString Name()
    {
        return m_name;
    }
    void setName(const QString &name)
    {
        m_name = name;
    }

    QString BaseURL()
    {
        return m_baseURL;
    }
    void setBaseURL(const QString &baseURL)
    {
        m_baseURL = baseURL;
    }

    void dump(const QString &title="");

signals:
    void loadedConfig();

private:
    bool readHTMLInternal(QIODevice *ioDev, PackageList::Type type);
private:
    QList<Package> *m_packageList;
    QString root;
    QString m_configFile;
    Downloader *downloader;
    Installer *installer;
    QString m_name;
    QString m_baseURL;

    friend class Installer;
};

#endif
