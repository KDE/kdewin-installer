/****************************************************************************
**
** Copyright (C) 2005-2010 Ralf Habacker. All rights reserved.
**
** This file is part of the KDE installer for windows
**
** SPDX-License-Identifier: LGPL-2.0-only
**
****************************************************************************/
#ifndef PACKAGE_H
#define PACKAGE_H

#include "hash.h"
#include "misc.h"
#include "settings.h"
#include "typehelper.h"

#include <QString>
#include <QUrl>
#include <QStringList>
#include <QHash>
#include <QtDebug>

class QTextStream;
class Downloader;
class Installer;
class QFile;

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
            /// lower equal operator 
            bool operator<=(const PackageVersion &other) const;
            /// greater than operator 
            bool operator>(const PackageVersion &other) const;
            /// greater equal operator 
            bool operator>=(const PackageVersion &other) const;
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
            PackageItem() { }
            PackageItem(FileTypes::Type type) { m_contentType = type; }
            PackageItem(const QString &type) { setContentType(type); }
            
            bool setContentType(const QString &type);
            FileTypes::Type contentType() const { return m_contentType; }
            
            bool setFileName(const QString &_fileName) 
            { 
                m_fileName = _fileName; 
                return true; 
            }
            const QString &fileName() const { return m_fileName; }
            
            bool setUrl(const QUrl &_url) 
            { 
                if (!_url.isValid()) 
                {
                    qCritical() << __FUNCTION__ << "invalid url" << _url.toString();
                    return false;
                }
                m_url = _url; 
                return true; 
            }
            const QUrl &url() const { return m_url; }

            bool setUrlAndFileName(const QUrl &url, const QString &fn);

            void setCheckSum(const QString &hash) { m_hash = hash; }
            const QString &checkSum() const { return m_hash; }

            void setInstalled(bool state) { m_installed = state; }
            bool installed() const { return m_installed; }
            
            PackageVersion version; // package item version
            friend QDebug &operator<<(QDebug &, const Package::PackageItem &);

        protected:
            QString m_fileName;       // filename only
            FileTypes::Type    m_contentType;    // BIN / LIB / DOC / SRC
            bool    m_installed;      // true if already installed
            QString m_hash;           /// hash sum 
            QUrl    m_url;            // complete download url
            QT_DEPRECATED bool set(const QUrl &url, const QString &fn, FileTypes::Type contentType=FileTypes::NONE, bool bInstalled = false);
            QT_DEPRECATED bool set(const QUrl &url, const QString &fn, const QByteArray &contentType, bool bInstalled = false);

    };
public:
    typedef QHash<FileTypes::Type, PackageItem> PackageItemType;
    
    Package();
    Package(const Package &other);
    Package(const QUrl &file);

    // base package related methods 

    /// return name - the name contains the base name and the compiler
    QString name() const { return m_name; }
    /// set name 
    void setName(QString const &name) { m_name = name; }

    /// return version
    const PackageVersion &version() const { return m_version.isEmpty() ? m_installedversion : m_version; }
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

    /// return home url  
    QString homeURL() const { return m_homeUrl; }
    /// return home url 
    void setHomeURL(const QString &url) { m_homeUrl = url; }

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
    const StringHash &pathRelocations() const { return m_pathRelocs; }
    /// add path relocations
    void addPathRelocation(const QString &key, const QString &value) { m_pathRelocs[key] = value; }

    /// package item related 
    /// -> probably better return pointer to packageitem and access its methods 
    
    /// return state of a specific item type is available
    bool hasType(FileTypes::Type contentType) const;

    /// return modifiable package item
    Package::PackageItem &item(FileTypes::Type contentType);
    
    /// add a package item to this package
    bool add(const PackageItem &item);

    /// check if specific content is already installed
    QT_DEPRECATED bool isInstalled(FileTypes::Type type) const;
    /// set that a specific content is already installed
    QT_DEPRECATED void setInstalled(FileTypes::Type type);

    /// returns local fileName of package item e.g. xyz-1.2.3-bin.zip
    QString localFileName(FileTypes::Type type) const;

    /// return full local path and filename for a package type in the download  directory. If bCreateDir is true creates path 
    QString localFilePath(FileTypes::Type type, bool bCreateDir = false);

    /// returns complete url of package item e.g. http://host/path.../fileName
    QUrl getUrl(FileTypes::Type type) const;
    /// set url of package item e.g. http://host/path.../fileName
    bool setUrl(FileTypes::Type type, const QUrl &url);

    /// download a package item
    bool downloadItem(FileTypes::Type type);
    /// install a package item
    bool installItem(Installer *installer, FileTypes::Type type);
    /// uninstall a package item
    bool removeItem(Installer *installer, FileTypes::Type type);

    /// set Install state of a package type (e.g. from gnuwin32 manifests)
    QT_DEPRECATED void setInstalled(const Package &other);

    Hash &hashType() { return m_hashType; } 

    /// save package to stream
    bool write(QTextStream &out) const;
    /// load package from stream
    bool read(QTextStream &in);

    /// dump package content 
    QT_DEPRECATED void dump(const QString &title=QString()) const;

    bool handled() const { return m_handled; }
    void setHandled(bool state) { m_handled = state; }

    /// return last error in cleartext form
    const QString &error() const { return m_lastError; }

    /// generate manifest file name
    QString manifestFileName(const FileTypes::Type type) const;

    /// generate version file name
    QString versionFileName(const FileTypes::Type type) const;
    
    /// user specific data pointer (currently used for ExternalInstallerControl)
    bool setUserData(int index,void *data) 
    { 
        if (index >= 0 && index <= 1) 
        {
            m_userData[index] = data;
            return true;
        }
        return false;
    }
        
    /// return user specific data pointer (currently used for ExternalInstallerControl)
    void *userData(int index) 
    { 
        if (index >= 0 && index <= 1) 
            return m_userData[index]; 
        return 0;
    }

protected:    
    bool setError(const QString &text) { m_lastError = text; qCritical() << text; return false; }

    PackageItemType m_packages;
    QString m_name;     // base name (a2ps)
    PackageVersion m_version;  // base version (4.13b-1)
    PackageVersion m_installedversion;
    QString m_notes;    // notes from package.notes
    QString m_homeUrl;  // home url of package
    QString m_longNotes;// notes from package.notes
    QStringList m_categories;
    QStringList m_deps;
    StringHash m_pathRelocs;
    bool       m_handled;      // marker for several operations
    Hash       m_hashType;     // contains the hash type for this package
    QString    m_lastError;    
    void       *m_userData[2];
    static QString m_packageRoot;
    friend QDebug &operator<<(QDebug &, const Package &);
};
QDebug &operator<<(QDebug &, const FileTypes::Type);
QDebug &operator<<(QDebug &, const Package::PackageItem &);
QDebug &operator<<(QDebug &, const Package &);
QDebug &operator<<(QDebug &, const QList<Package*> &);


#endif
