/****************************************************************************
**
** Copyright (C) 2006 Ralf Habacker. All rights reserved.
**
** This file is part of the KDE installer for windows
**
** This file may be used under the terms of the GNU General Public
** License version 2.0 as published by the Free Software Foundation
** and appearing in the file LICENSE.GPL included in the packaging of
** this file.  Please review the following information to ensure GNU
** General Public Licensing requirements will be met:
** http://www.trolltech.com/products/qt/opensource.html
**
** If you are unsure which license is appropriate for your use, please
** review the following information:
** http://www.trolltech.com/products/qt/licensing.html or contact the
** sales department at sales@trolltech.com.
**
** This file is provided AS IS with NO WARRANTY OF ANY KIND, INCLUDING THE
** WARRANTY OF DESIGN, MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE.
**
****************************************************************************/

#ifndef GLOBALCONFIG_H
#define GLOBALCONFIG_H

#include "site.h"
#include "package.h"
class Downloader;

#define APROPERTY(type,read,write)  private: \
type m_##read; \
public:  \
const type & read() { return m_##read; } \
void write(const type &read) { m_##read = read; } \

#define DUMPPROPERTY(read)  qDebug() << "m_"##read << " " << m_##read


/**
 holds global configuration downloaded from a remote site and/or local config file. 
*/
#if 0
class ConfigItem
{
    APROPERTY (QString,name,setName);
    APROPERTY (QString,url,setURL);
    APROPERTY (QString,description,setDescription);
public:
    void dump()
    {
        DUMPPROPERTY(name);
        qDebug() << "m_name  " << m_name;  
        qDebug() << "m_url   " << m_url;   
        qDebug() << "m_description " << m_description;   
    }
};


class LinkConfigItem : public ConfigItem {
};        

class PackageConfigItem : public ConfigItem {
    APROPERTY (QStringList,require,setRequire);
    APROPERTY (QStringList,exclude,setExclude);
    public: 
        void dump()
        {
            ConfigItem::dump();
            qDebug() << "m_require " << m_require;  
            qDebug() << "m_exclude " << m_exclude;
        }
};        

class SiteConfigItem : public ConfigItem {
    public:
        enum SiteType {SourceForge, ApacheModIndex};

    APROPERTY (SiteType,Type,Type);
    APROPERTY (QString,mirror,setMirror);
    public:
        void dump()
        {
            ConfigItem::dump();
            qDebug() << "m_Type  " << m_Type;  
            qDebug() << "m_mirror" << m_mirror;
        }
};

class SiteConfigItem : public Site {
};
#endif

class GlobalConfig {
    public:
        GlobalConfig(const QString &url, Downloader &downloader);
        ~GlobalConfig();
        QList<Site*> *sites()  { return &m_sites;   } 
        QList<Package*> *packages() { return &m_packages; }
    protected:
        bool parseFromFile(const QString &fileName);
        bool parseFromByteArray(const QByteArray &ba);
        bool parse(QIODevice *ioDev);

    private:
        QList <Site*> m_sites; 
        QList <Package *> m_packages;
};

#endif
