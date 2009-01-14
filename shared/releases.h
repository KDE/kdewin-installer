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
#ifndef RELEASES_H
#define RELEASES_H

#include <QtCore/QString>
#include <QtCore/QUrl>
class QByteArray;

/**    
   \brief The ReleaseType class provides access to the details of a single release from a given mirror. 
   */
class ReleaseType {
    public:
        typedef enum { Stable, Unstable } Type;

        ReleaseType() { }
        const QString toString() const 
        { 
            QString t = type == Stable ? "stable " : "unstable ";
            return t + name; 
        }
        QUrl url;
        QString name; 
        Type type;   
};
QDebug &operator<<(QDebug &out, const ReleaseType &c);

/**    
   The ReleaseTypeList type specifies a list of releases. 
    */
typedef QList<ReleaseType> ReleaseTypeList;

/**    
   \brief The Releases class provides access to all releases located on a specific mirror.  
   
   Releases are fetched from a mirror by fetch(). The detected releases are accessable through the releases() method.
    */
class Releases
{
    public:
    
        Releases();
        ~Releases();

        /// fetch list of releases from the given url
        bool fetch(const QUrl &baseURL);
        /// clear list of previously fetched releases 
        void clear() { m_releases.clear(); }
        ///manually add a release to the list of releases
        void add(const ReleaseType &release) { m_releases.append(release); }
        /// return list of detected releases
        ReleaseTypeList &releases() { return m_releases; }

        /**
            return the really used base url because the base url given to fetch 
            may contain release informations, the return value is only valid 
            after calling @ref fetch().
        */
        const QUrl &realBaseURL() const { return m_baseURL; }

        /// return singleton instance
        static Releases &instance();
    protected:
        /// convert old style mirror url to ReleaseType based 
        bool convertFromOldMirrorUrl(QUrl &url);
        /// remove release path from old style mirror url 
        bool useOldMirrorUrl(const QUrl &url);
        /// check win32 in release path for kde mirrors
        bool patchReleaseUrls(const QUrl &url);

        /**
         parse release list from a local file. The release list is accessable by the releases() method. 
        
         @param filename
         @param url base mirror url 
         @param type type of release list provided by filename
         @return true if parse was performed successfully, false otherwise
        */
        bool parse(const QString &fileName, const QUrl &url, ReleaseType::Type type);
        /**
         parse releases list from a QByteArray. The release list is accessable by the releases() method. 
         
         @param data QByteArry instance 
         @param url base mirror url 
         @param type type of release list provided by data
         @return true if parse was performed successfully, false otherwise
        */
        bool parse(const QByteArray &data, const QUrl &url, ReleaseType::Type type);
        /**
         parse releases list from an QIODevice instance. The releases list is accessable 
         by the releases() method. 
         
         @param data QIODevice instance 
         @param url base mirror url 
         @param type type of release list provided by data
         @return true if parse was performed successfully, false otherwise
        */
        bool parse(QIODevice *ioDev, const QUrl &url, ReleaseType::Type type);

        ReleaseTypeList m_releases;
        QUrl m_baseURL;
};

QDebug &operator<<(QDebug &,const ReleaseTypeList &);

#endif
