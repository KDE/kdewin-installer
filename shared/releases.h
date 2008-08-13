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

typedef QList<ReleaseType> ReleaseTypeList;

class Releases
{
    public:
    
        Releases();
        ~Releases();

        // fetch list of releases
        bool fetch(const QUrl &baseURL);
        void clear() { m_releases.clear(); }
        void add(const ReleaseType &release) { m_releases.append(release); }
        ReleaseTypeList &releases() { return m_releases; }
        static Releases &instance();
    protected:

        /**
         parse mirror list from a local file. The mirror list is accessable 
         by the mirrors() method. 
        
         @param filename
         @return true if parse was performed successfully, false otherwise
        */
        bool parse(const QString &fileName, const QUrl &url, ReleaseType::Type type);
        /**
         parse releases list from a QByteArray. The releases list is accessable 
         by the releases() method. 
         
         @param data - QByteArry instance 
         @return true if parse was performed successfully, false otherwise
        */
        bool parse(const QByteArray &data, const QUrl &url, ReleaseType::Type type);
        /**
         parse releases list from an QIODevice instance. The releases list is accessable 
         by the releases() method. 
         
         @param data - QIODevice instance 
         @return true if parse was performed successfully, false otherwise
        */
        bool parse(QIODevice *ioDev, const QUrl &url, ReleaseType::Type type);

        ReleaseTypeList m_releases;
};


QDebug &operator<<(QDebug &,const ReleaseTypeList &);

#endif
