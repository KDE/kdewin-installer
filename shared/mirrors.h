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
#ifndef MIRRORS_H
#define MIRRORS_H

#include <QtCore/QString>
#include <QtCore/QUrl>
class QByteArray;

/**
 holds all attributes for a single mirror 
*/

class MirrorType
{
    public:
        typedef enum {Unspecified, Local, Other } URLType;
        MirrorType(const QUrl &_url=QUrl(), const QString _name=QString(), const QString &_continent=QString(), const QString &_country=QString(), URLType _type=Unspecified)
        {
            url = _url;
            name = _name;
            continent = _continent;
            country   = _country;  
            type      = _type;     
        }                

        QUrl url;
        QString name;
        QString continent;
        QString country;
        URLType type;

        friend QDebug &operator<<(QDebug &,const MirrorType &);
        QString toString() const
        {
            QString result;
            if (type == Local)
                result += "Local ";
            else if (type == Other)
                result += "Other ";
            if ( continent.isEmpty() )
                return result + name;
            else 
                return result + continent + "," + country + " (" + name + ")";
        }
};

typedef QList<MirrorType> MirrorTypeList;

/**
  This class provides access to a list of download mirror locations, which could be fetched from a 
  remote http or ftp site. The mirror locations are accessable as a list of MirrorType instances.
  Currently there are three formats of mirror list files supported. 
    KDE - mirrors defined in this format are containing one mirror on each line in the form 
    
          <protocol> <2-digit country code> <url><eol>

    cygwin - this format is used by the cygwin setup application with one mirror on each line 
             using the following form

              <url>;<name>;<continent>;<country><eol>

    KDE_HTML - this format is platin html text where the urls are extracted from the href attribut 
               of an <a> tag 
*/

class Mirrors
{
    public:
        enum Type { KDE = 1, KDE_HTML = 2, Cygwin= 3 };

    /** 
     holds several options required by Mirrors::fetch()
    */
    class Config {
        public:
            Config() {}
            Config(const QUrl &url, Type type, const QString &releasePath, const QString &exclude)
                : url(url), type(type), releasePath(releasePath), excludePattern(exclude) 
            {}
            Config(const QString &url, Type type, const QString &releasePath, const QString &exclude)
                : url(QUrl(url)), type(type), releasePath(releasePath), excludePattern(exclude) 
            {}
            QUrl url;
            Type type;
            QString releasePath;
            /// contains exclude pattern of hosts to which the release Path isn't appended 
            QString excludePattern; 
    };

    public:
        Mirrors();
        Mirrors(const Config &config);
        ~Mirrors();
        void setConfig(const Config &config) { m_config = config; }

        // fetch mirror list 
        bool fetch();
        void clear() { m_mirrors.clear(); }
        void add(const MirrorType &mirror) { m_mirrors.append(mirror); }
        MirrorTypeList &mirrors() { return m_mirrors; }
        static Mirrors &instance();
    protected:

        /**
         parse mirror list from a local file. The mirror list is accessable 
         by the mirrors() method. 
        
         @param filename
         @return true if parse was performed successfully, false otherwise
        */
        bool parse(const QString &fileName);
        /**
         parse mirror list from a QByteArray. The mirror list is accessable 
         by the mirrors() method. 
         
         @param data - QByteArry instance 
         @return true if parse was performed successfully, false otherwise
        */
        bool parse(const QByteArray &data);
        /**
         parse mirror list from an QIODevice instance. The mirror list is accessable 
         by the mirrors() method. 
         
         @param data - QIODevice instance 
         @return true if parse was performed successfully, false otherwise
        */
        bool parse(QIODevice *ioDev);
        void initCountries();

        QList<MirrorType> m_mirrors;
        static QHash<QString,QString> m_countries;
        static QHash<QString,QString> m_continents;
        Config m_config;
};


QDebug &operator<<(QDebug &,const MirrorTypeList &);

#endif
