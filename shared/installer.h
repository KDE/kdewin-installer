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
#ifndef INSTALLER_H
#define INSTALLER_H

class PackageList;
class InstallerProgress;

class Installer : public QObject
{
    Q_OBJECT

public:
    enum InstallerType { Standard, GNUWIN32 };
    Installer(PackageList *packageList, InstallerProgress *progress=0);
    ~Installer();
    InstallerType Type() const
    {
        return m_type;
    }
    void setType(InstallerType type)
    {
        m_type = type;
    }
    bool install(const QString &fileName /*, const QString &destdir=""*/);
    // installPackage(Package *pkg)
    //  bool readFromFile(QString const &fileName);
    //  bool writeToFile(QString const &fileName);
    //bool loadConfig(const QString &destdir="");
    bool loadConfig();

    bool isEnabled();
    void setRoot(const QString &root);
    const QString root() const { return m_root; }

public slots:
    void updatePackageList();

protected:
    bool unzipFile(const QString &destpath, const QString &zipFile);
    void setError(const QString &str);

    PackageList *packageList;
    QString m_root;
    QString configFile;
    InstallerProgress *m_progress;
    InstallerType m_type;
};

#endif


