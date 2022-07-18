/****************************************************************************
**
** Copyright (C) 2006-2007 Ralf Habacker. All rights reserved.
**
** This file is part of the KDE installer for windows
**
** SPDX-License-Identifier: LGPL-2.0-only
**
****************************************************************************/
#ifndef INSTALLERUPDATE_H
#define INSTALLERUPDATE_H

#include <QString>
#include <QUrl>

/**    
   \brief The InstallerUpdate class provides access to new installer versions.
    */
class InstallerUpdate
{
    public:
        InstallerUpdate();
        ~InstallerUpdate();

        /// check if the provided version is newer 
        bool isUpdateAvailable();
        
        /// fetch new installer 
        bool fetch();

        /// run new installer 
        bool run();

        /// finish update - delete old installer executable
        static bool finish(const QStringList &args, int startIndex);

        /// return singleton instance
        static InstallerUpdate &instance();
        
    protected:
        /// set current installer version
        void setCurrentVersion(const QString &version);
        /// set new installer version
        void setNewVersion(const QString &version);
        /// set url for new update
        void setUrl(const QUrl &url);

        QUrl m_url;
        QString m_newVersion;
        QString m_currentVersion;
        QString m_localFilePath;
        QString m_currentInstallerFilePath;
    friend QDebug &operator<<(QDebug &out, const InstallerUpdate &c);
};


#endif
