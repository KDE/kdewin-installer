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
#ifndef PACKAGE_H
#define PACKAGE_H

#include <QObject>
#include <QString>
#include <QStringList>
#include <QHash>

#include "settings.h"

class QTextStream;
class Downloader;
class Installer;

typedef QHash<QString, QString> StringHash;

/** 
 PackageInfo covers package information related tasks
*/
class PackageInfo 
{
public:
	static bool fromFileName(const QString &fileName, QString &pkgName, QString &pkgVersion, QString &pkgType, QString &pkgFormat=QString());
};


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
    enum Type { NONE = 0, BIN = 1 ,LIB = 2 ,DOC = 4 ,SRC = 8, ALL = 15};
    Q_DECLARE_FLAGS(Types,Type);

    class PackageItem {
        public:
		    bool set(const QString &url, const QString &fn, Package::Type contentType, bool bInstalled = false);
			bool set(const QString &url, const QString &fn, const QByteArray &contentType, bool bInstalled = false);
			bool setContentType(const QString &type);
            // dump content
    	    void dump(const QString &title=QString()) const;
            QString url;            // complete download url 
            QString fileName;  // filename only
            QString packageType;    // zip / msi / ...
            Type    contentType;    // BIN / LIB / DOC / SRC
            bool    bInstalled;     // true if already installed
            QString version;        // package item version
    };
public:

    Package();
    Package(const Package &other);

    const QString &name() const { return m_name; }
    void setName(QString const &name) { m_name = name; }

    const QString &version() const { return m_version; }
    void setVersion(const QString &version) { m_version = version; }

    const QString &notes() const { return m_notes; }
    void setNotes(const QString &notes) { m_notes = notes; }

    QString toString(bool installed=false, const QString &delim = "-");
    QString getTypeAsString(bool requiredIsInstalled=false, const QString &delim = " ");

    // returns fileName of package item e.g. xyz-1.2.3-bin.zip
    QString getFileName(Package::Type contentType);
    // returns complete url of package item e.g. http://host/path.../fileName
    QString getURL(Package::Type type);
    // returns the base Url of all packages (if possible, otherwise QString())
    QString getBaseURL();

	// add a file to this package
	bool add(const PackageItem &item);

	// set Install state of a package type (e.g. from gnuwin32 manifests)
    void setInstalled(const Package &other);
    // return state of a specific item type is available
    bool hasType(Package::Type contentType) const;

    // check if specific content is already installed
    bool isInstalled(Package::Type type) const;
    // set that a specific content is already installed
    void setInstalled(Package::Type type);

    // save package to stream
    bool write(QTextStream &out);
    // load package from stream
    bool read(QTextStream &in);
    
    // dump package content
    void dump(const QString &title=QString()) const;
    
    // download a package item
    bool downloadItem(Downloader *downloader, Package::Type type);
    bool installItem(Installer *installer, Package::Type type);
    bool removeItem(Installer *installer, Package::Type type);

    // package category 
    const QString &category() const { return m_category; }
    void setCategory(const QString &cat);

    // package dependencies
    void addDeps(const QStringList &addDeps);
    const QStringList &deps() const { return m_deps; }

    // filepath relocations for installing 
    void addPathRelocation(const QString &key, const QString &value) { m_pathRelocs[key] = value; }
    StringHash &pathRelocations() { return m_pathRelocs; }

	static QString typeToString(Package::Type type);

	bool handled() { return m_handled; }
	void setHandled(bool state) { m_handled = state; }

private slots:
    void logOutput();

protected:
    // Creates the full path + filename for a package type, creates path when wanted
    QString makeFileName(Package::Type type, bool bCreateDir = false);

	QHash<Type, PackageItem> m_packages;

    QString m_name;     // base name (a2ps)
    QString m_version;  // base version (4.13b-1)
    QString m_notes;    // notes from package.notes
    QString m_category;   
    QStringList m_deps;       
    StringHash m_pathRelocs;
    bool       m_handled;      // marker for several operations
};

Q_DECLARE_OPERATORS_FOR_FLAGS(Package::Types);

#endif
