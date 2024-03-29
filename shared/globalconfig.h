/****************************************************************************
**
** Copyright (C) 2006-2010 Ralf Habacker. All rights reserved.
**
** This file is part of the KDE installer for windows
**
** SPDX-License-Identifier: LGPL-2.0-only
**
****************************************************************************/

#ifndef GLOBALCONFIG_H
#define GLOBALCONFIG_H

#include "hash.h"
#include "site.h"
#include "package.h"

#include <QDateTime>
#include <QFileInfo>

class Downloader;

class GlobalConfig {
    public:
        class Mirror {
            public: 
                QString url;
                QString location;
                friend QDebug operator <<(QDebug ,const GlobalConfig::Mirror &);
        };

        GlobalConfig();
        ~GlobalConfig();
        /// fetch local or remote config files 
        QStringList fetch(const QString &baseURL);
        /// parse list of config files
        bool parse(const QStringList &configFiles);

        /// return list of site definitions
        QList<Site*> *sites()  { return &m_sites; } 

        /// return list of package definitions 
        QList<Package*> *packages() { return &m_packages; }

        /// return list of mirrors
        QList<Mirror*> *mirrors() { return &m_mirrors; }
        
        /// return category notes
        QHash <QString,QString> &categoryNotes() { return m_categoryNotes; }
        
        /// return category packages relations
        QHash <QString,QStringList> &categoryPackages() { return m_categoryPackages; }

        /// return meta packages relations
        QHash <QString,QStringList> &metaPackages() { return m_metaPackages; }

        /// return package orientated news
        QHash<QString, QString> *news() { return &m_news; }

        /// return category displayed in enduser mode
        QStringList &endUserCategories() { return m_endUserCategories; }
        
        /// returns the category for a specific package
        QString packageCategory(const QString &package);

        QDateTime &timeStamp() { return m_timestamp; }
        void clear(); 
        /// return version of required installer
        const QByteArray &minimalInstallerVersion() { return m_minimalInstallerVersion; }
        void setBaseURL(const QUrl &url) { m_baseURL = url.toString(); }

        /// check if a config-remote.txt is located in download directory
        static bool isRemoteConfigAvailable();
        /// return path of remote config file
        static const QFileInfo remoteConfigFile();

        Hash &hashType() { return m_hashType; }

    protected:
        bool parseFromFile(const QString &fileName);
        bool parseFromByteArray(const QByteArray &ba);
        bool parse(QIODevice *ioDev);

    private:
        QList <Site*> m_sites; 
        QList <Package*> m_packages;
        // package news <key,value> where key = <name>-<version>
        QHash <QString,QString> m_news;
        QHash <QString,QString> m_categoryNotes;
        QHash <QString,QStringList> m_categoryPackages;
        QHash <QString,QStringList> m_metaPackages;
        QList <Mirror*> m_mirrors;
        QString m_baseURL;
        QDateTime m_timestamp;
        QByteArray m_minimalInstallerVersion;
        QStringList m_endUserCategories;
        Hash m_hashType;

    Q_DISABLE_COPY(GlobalConfig)

    friend QDebug operator<<(QDebug, const GlobalConfig &);
};

#endif
