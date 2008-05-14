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
#include <QUrl>
#include <QStringList>
#include <QHash>
#include <QDebug>

#include "settings.h"

class QTextStream;
class Downloader;
class Installer;

typedef QHash<QString, QString> StringHash;

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
    enum Type { NONE = 0, BIN = 1 ,LIB = 2 ,DOC = 4 ,SRC = 8, ALL = 15, ANY = 16};
    Q_DECLARE_FLAGS(Types,Type);

    class PackageVersion {
		public:
            ///  constructor
			PackageVersion(const QString &version=QString());
            ///  assignment operator 
			PackageVersion &operator=(const PackageVersion &other);
            ///  equal operator 
			bool operator==(const PackageVersion &other) const;
            /// not equal operator 
			bool operator!=(const PackageVersion &other) const;
            /// lower than operator 
			bool operator<(const PackageVersion &other) const;
            /// greater than operator 
			bool operator>(const PackageVersion &other) const;
            /// equal string operator 
			bool operator==(const QString &other) const;
            /// not equal string operator 
			bool operator!=(const QString &other) const;
            /// check if version is empty 
			bool isEmpty() const;
            /// return version in string representation
			QString toString() const;
            /// debug operator 
			friend QDebug &operator<<(QDebug &, const PackageVersion &);
		protected:
            /** internal compare function, used by operators
                                @param other Packageversion instance 
                                @return -1 this < other, 1 this > other, 0 this = other
                            */
            int compare(const PackageVersion &other) const;
		private:
			QString m_version; 
 	};

    class PackageItem {
        public:
            bool set(const QUrl &url, const QString &fn, Package::Type contentType, bool bInstalled = false);
            bool set(const QUrl &url, const QString &fn, const QByteArray &contentType, bool bInstalled = false);
            bool setContentType(const QString &type);

            QUrl    url;            // complete download url
            QString fileName;       // filename only
            QString packageType;    // zip / msi / ...
            Type    contentType;    // BIN / LIB / DOC / SRC
            bool    bInstalled;     // true if already installed
            PackageVersion version; // package item version
    friend QDebug &operator<<(QDebug &, const Package::PackageItem &);
    };
public:
    typedef QHash<Type, PackageItem> PackageItemType;
    
    Package();
    Package(const Package &other);

    // base package related methods 

    /// return name 
    QString name() const { return m_name; }
    /// set name 
    void setName(QString const &name) { m_name = name; }

    /// return version
    PackageVersion &version()  { return m_version.isEmpty() ? m_installedversion : m_version; }
    /// set version
    void setVersion(const QString &version) { m_version = PackageVersion(version); }

    /// return installed version
    PackageVersion installedVersion() const { return m_installedversion; }
    /// set version
    void setInstalledVersion(const QString &version) { m_installedversion = PackageVersion(version); }
    void setInstalledVersion(PackageVersion version) { m_installedversion = version; }

    /// return notes 
    QString notes() const { return m_notes; }
   
    /// return long notes 
    void setNotes(const QString &notes) { m_notes = notes; }

    /// return long notes 
    QString longNotes() const { return m_longNotes; }
    /// set long notes 
    void setLongNotes(const QString &notes) { m_longNotes = notes; }

    /// return package categories
    const QStringList &categories() const { return m_categories; }
    /// set package categories
    void addCategories(const QStringList &cat);
    /// set package categories
    void addCategories(const QString &cat);

    /// return package dependencies
    const QStringList &deps() const { return m_deps; }
    /// add package dependencies 
    void addDeps(const QStringList &addDeps);

    QString toString(bool installed=false, const QString &delim = "-") const;
    QString getTypeAsString(bool requiredIsInstalled=false, const QString &delim = " ") const;

    /**
     return path relocations - path relocations are used to install files from an archive into 
     a different location 
    */
    StringHash &pathRelocations() { return m_pathRelocs; }
    /// add path relocations
    void addPathRelocation(const QString &key, const QString &value) { m_pathRelocs[key] = value; }

    /// package item related 
    /// -> probably better return pointer to packageitem and access its methods 
    
    /// return state of a specific item type is available
    bool hasType(Package::Type contentType) const;

    /// return modifiable package item
    //PackageItem &get(Package::Type contentType);

    /// add a package item to this package
    bool add(const PackageItem &item);

    /// check if specific content is already installed
    QT_DEPRECATED bool isInstalled(Package::Type type) const;
    /// set that a specific content is already installed
    QT_DEPRECATED void setInstalled(Package::Type type);

    /// returns fileName of package item e.g. xyz-1.2.3-bin.zip
    QString getFileName(Package::Type contentType);

    /// returns complete url of package item e.g. http://host/path.../fileName
    QUrl getUrl(Package::Type type) const;
    /// set url of package item e.g. http://host/path.../fileName
    bool setUrl(Package::Type type, const QUrl &url);

    /// download a package item
    bool downloadItem(Package::Type type);
    /// install a package item
    bool installItem(Installer *installer, Package::Type type);
    /// uninstall a package item
    bool removeItem(Installer *installer, Package::Type type);

    /// return specific type as string
    static QString typeToString(Package::Type type);

    /// set Install state of a package type (e.g. from gnuwin32 manifests)
    QT_DEPRECATED void setInstalled(const Package &other);

    /// save package to stream
    bool write(QTextStream &out) const;
    /// load package from stream
    bool read(QTextStream &in);

    /// dump package content 
    QT_DEPRECATED void dump(const QString &title=QString()) const;

    bool handled() const { return m_handled; }
    void setHandled(bool state) { m_handled = state; }

    /// generate manifest file name
    QString manifestFileName(const Package::Type type);

    /// generate version file name
    QString versionFileName(const Package::Type type);

    /// separate package name, version, type and file format from a filename
    static bool fromFileName(const QString &fileName, QString &pkgName, QString &pkgVersion, QString &pkgType, QString &pkgFormat);

    /// separate package name and version from a string
    static bool fromString(const QString &astring, QString &pkgName, QString &pkgVersion);

    /// generate manifest file name
    static QString manifestFileName(const QString &pkgName, const QString &pkgVersion, const Package::Type type);

    /// generate version file name
    static QString versionFileName(const QString &pkgName, const QString &pkgVersion, const Package::Type type);

private slots:
    void logOutput();

protected:
    // Creates the full path + filename for a package type, creates path when wanted
    QString makeFileName(Package::Type type, bool bCreateDir = false);

    PackageItemType m_packages;
    QString m_name;     // base name (a2ps)
    PackageVersion m_version;  // base version (4.13b-1)
    PackageVersion m_installedversion;
    QString m_notes;    // notes from package.notes
    QString m_longNotes;// notes from package.notes
    QStringList m_categories;
    QStringList m_deps;
    StringHash m_pathRelocs;
    bool       m_handled;      // marker for several operations

    friend QDebug &operator<<(QDebug &, const Package &);
};
QDebug &operator<<(QDebug &, const Package::Type);
QDebug &operator<<(QDebug &, const Package::PackageItem &);
QDebug &operator<<(QDebug &, const Package &);
QDebug &operator<<(QDebug &, const QList<Package*> &);

Q_DECLARE_OPERATORS_FOR_FLAGS(Package::Types);

#define PackageInfo Package
#endif
