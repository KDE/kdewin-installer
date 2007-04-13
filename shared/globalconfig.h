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

#ifndef GLOBALCONFIG_H
#define GLOBALCONFIG_H

#include "site.h"
#include "package.h"
class Downloader;

class GlobalConfig {
    public:
        GlobalConfig(const QString &url, Downloader &downloader);
        ~GlobalConfig();
        QList<Site*> *sites()  { return &m_sites;   } 
        QList<Package*> *packages() { return &m_packages; }
        QHash<QString, QString> *news() { return &m_news; }

        void dump(const QString &title=QString());
    protected:
        bool parseFromFile(const QString &fileName);
        bool parseFromByteArray(const QByteArray &ba);
        bool parse(QIODevice *ioDev);

    private:
        QList <Site*> m_sites; 
        QList <Package *> m_packages;
        // package news <key,value> where key = <name>-<version>
        QHash <QString,QString> m_news;
};

#endif
