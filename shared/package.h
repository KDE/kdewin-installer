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
class QTextStream;
class Downloader;

/* This class holds a package with all it's single files
    a2ps-4.13b-1-bin.zip
    a2ps-4.13b-1-dep.zip
    a2ps-4.13b-1-doc.zip
    a2ps-4.13b-1-lib.zip
    a2ps-4.13b-1-src.exe
    a2ps-4.13b-1-src.zip
 */
class Package
{
public:
    enum Type { BIN = 1 ,LIB = 2 ,DOC = 4 ,SRC = 8 };

    class packageDescr {
        public:
            // dump content
    	    void dump(const QString &title="");
            QString path;           // complete url (use QUrl?)
            QString fileName;       // filename only
            QString packageType;    // zip / msi / ...
            Type    contentType;    // BIN / LIB / DOC / SRC
            bool    bInstalled;     // true if already installed
    };
public:

    Package();

    const QString &name() const
    {
        return m_name;
    }
    void setName(QString const &name)
    {
        m_name = name;
    }

    const QString &version() const
    {
        return m_version;
    }
    void setVersion(const QString &version)
    {
        m_version = version;
    }

    QString toString(bool installed=false, const QString &delim = "-");
    QString getTypeAsString(bool requiredIsInstalled=false, const QString &delim = " ");

    // return packageDescr::fileName for specified contentType
    QString getFileName(Package::Type contentType);
    // return packageDescr::path for specified contentType
    QString getURL(Package::Type type);
    // add a file to this package
    void add(const QString &path, Package::Type contentType, bool bInstalled = false);
    void add(const QString &path, const QByteArray &contentType, bool bInstalled = false);

    static QString baseURL;

    // check if specific content is already installed
    bool isInstalled(Package::Type type);
    // set that a specific content is already installed
    void setInstalled(Package::Type type);

    // save package to stream
    bool write(QTextStream &out);
    // load package from stream
    bool read(QTextStream &in);
    
    // dump package content
    void dump(const QString &title="");
    
    // download a package item
    bool download(Downloader *downloader, Package::Type type);

private slots:
    void logOutput();

protected:
    QList<packageDescr> m_packages;

    QString m_name;     // base name (a2ps)
    QString m_version;  // base version (4.13b-1)
};

#endif
