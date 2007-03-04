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
class Site;

QStringList filterPackageFiles(const QStringList &list,const QString &mode);

class PackageList : public QObject
{
    Q_OBJECT

public:
    enum Type {SourceForge, ApacheModIndex};

    PackageList(Downloader *downloader=NULL);
    virtual ~PackageList();
    void addPackage(const Package &package);
    void listPackages(const QString &title=QString());
    bool readFromFile(const QString &_fileName=QString());
    bool syncWithFile(const QString &_fileName=QString());
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
    bool downloadPackage(const QString &pkgName, Package::Types types=Package::ALL);
    bool installPackage(const QString &pkgName, Package::Types types=Package::ALL);

    void dump(const QString &title=QString());

signals:
    void configLoaded();

private:
    bool readHTMLInternal(QIODevice *ioDev, PackageList::Type type, bool append=false);
private:
    QList<Package*> m_packageList;
    QString m_root;
    QString m_configFile;
    Downloader *m_downloader;
    Installer *m_installer;
    QString m_name;
    QString m_baseURL;
    Site *m_curSite;

    friend class Installer;
};

#endif
