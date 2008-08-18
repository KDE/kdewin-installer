/****************************************************************************
**
** Copyright (C) 2006-2007 Ralf Habacker. All rights reserved.
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
#ifndef INSTALLERUPDATE_H
#define INSTALLERUPDATE_H

#include <QtCore/QString>
#include <QtCore/QUrl>

/**    
   \brief The InstallerUpdate class provides access to new installer versions.
    */
class InstallerUpdate
{
    public:
        InstallerUpdate();
        ~InstallerUpdate();

        /// set current installer version
        void setCurrentVersion(const QString version);
        /// set new installer version
        void setNewVersion(const QString version);
        /// set url for new update
        void setUrl(const QUrl &url);
        /// check if the provided version is newer 
        bool isUpdateAvailable();
        /// fetch new installer 
        bool fetch();

        /// run new installer 
        bool run();
        /// return singleton instance
        static InstallerUpdate &instance();
    protected:
        QUrl m_url;
        QString m_newVersion;
        QString m_currentVersion;
        QString m_localFilePath;
    friend QDebug &operator<<(QDebug &out, const InstallerUpdate &c);
};


#endif
