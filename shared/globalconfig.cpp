/****************************************************************************
**
** Copyright (C) 2006-2008 Ralf Habacker ralf.habacker@freenet.de>. All rights reserved.
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

#include "config.h"
#include "debug.h"
#include "externalinstallercontrol.h"
#include "downloader.h"
#include "globalconfig.h"

#include <QFileInfo>
#include <QBuffer>
#include <QUrl>
#ifdef USE_GUI
#include <QMessageBox>
#endif


QStringList packageTypes = QStringList() << "msvc" << "mingw" << "vc90" << "vc100" << "mingw4" << "x86-mingw4";

GlobalConfig::GlobalConfig()
{}

bool GlobalConfig::isRemoteConfigAvailable()
{
    QFileInfo cfr(Settings::instance().downloadDir()+"/config-remote.txt");
    return cfr.exists();
}

const QFileInfo GlobalConfig::remoteConfigFile()
{
    return QFileInfo(Settings::instance().downloadDir()+"/config-remote.txt");
}

QStringList GlobalConfig::fetch(const QString &baseURL)
{
    m_baseURL = baseURL;
    if(!m_baseURL.endsWith('/'))
      m_baseURL += '/';

    QStringList configFiles;
    QUrl url(m_baseURL + "config.txt");

    if (url.scheme() == "file")
    {
        QFileInfo cfr(url.toLocalFile());
        if (cfr.exists())
        {
            configFiles << cfr.absoluteFilePath();
        }
        else
        {
            qCritical() << "local config file does not exist" <<  cfr.absoluteFilePath();    
        }
        return configFiles;
    }
    // remote config file
    else if (url.scheme() == "http" || url.scheme() == "ftp")
    {
        QFileInfo cfr(Settings::instance().downloadDir()+"/config-remote.txt");
        qDebug() << "Check if a copy of the remote config file is available at" << cfr.absoluteFilePath() << (cfr.exists() ? "... found" : "... not found");
        if (cfr.exists())
        {
            configFiles << cfr.absoluteFilePath();
        }
        else
        {
            QFileInfo cfi(Settings::instance().downloadDir()+"/config.txt");
            bool ret = Downloader::instance()->fetch(url,cfi.absoluteFilePath());
            qDebug() << "download remote config file from" <<  url << "to" << cfi.absoluteFilePath() << "..." << (ret ? "okay" : "failure") ;
            if (ret)
                configFiles << cfi.absoluteFilePath();
            else 
                return configFiles;
        }

        QFileInfo fi(Settings::instance().downloadDir()+"/config-local.txt");
        qDebug() << "Check if a local config file is available at" << fi.absoluteFilePath() << (fi.exists() ? "... found" : "... not found");
        if (fi.exists())
            configFiles << fi.absoluteFilePath();

    }
    qDebug() << "using config file(s)" << configFiles;
    
    return configFiles;
}

bool GlobalConfig::parse(const QStringList &configFiles)
{
    bool ret = true;
    Q_FOREACH(const QString &configFile, configFiles) 
    {
        if( !parseFromFile(configFile) )
           ret = false;
        if (Settings::hasDebug("GlobalConfig"))
            qDebug() << "parse config file " << configFile << (ret ? "okay" : "failure") ;
    }
    return ret;
}

GlobalConfig::~GlobalConfig()
{
    clear();
}

bool GlobalConfig::parseFromFile(const QString &_fileName)
{
    QString fileName(_fileName);
    QFileInfo fi(fileName);
    if(!fi.isAbsolute())
        fileName = fi.absoluteFilePath();
    QFile configFile(fileName);
    if (!configFile.exists())
    {
#ifdef USE_GUI
        QMessageBox::warning(NULL, QString("File not found"), QString("Can't find %1").arg(fileName));
#else
        if (Settings::hasDebug("GlobalConfig"))
            qDebug() << __FUNCTION__ << "file not found " << fileName;
#endif

        return false;
    }

    configFile.open(QIODevice::ReadOnly | QIODevice::Text);

    return parse(&configFile);
}

bool GlobalConfig::parseFromByteArray(const QByteArray &_ba)
{
    QByteArray ba(_ba);
    QBuffer buf(&ba);

    if (!buf.open(QIODevice::ReadOnly | QIODevice::Text))
        return false;
    return parse(&buf);
}

bool GlobalConfig::parse(QIODevice *ioDev)
{
    bool inSite = false;
    bool inPackage = false;
    Site *site = 0;
    Package *pkg = 0;
    Mirror *mirror;
    int lineNr = 0; 
    while (!ioDev->atEnd())
    {
        QByteArray line = ioDev->readLine().replace('\x0a', ""); // support unix format file
        lineNr++;
        if (line.startsWith(';'))
            continue;
        else if (line.size() < 2)
        {
            if (inPackage)
            {
                if (Settings::hasDebug("GlobalConfig"))
                    qDebug() << __FUNCTION__ << *pkg;
                inPackage=false;
            }
            else if (inSite)
            {
                if (Settings::hasDebug("GlobalConfig"))
                    qDebug() << __FUNCTION__ << *site;
                inSite=false;
            }
            continue;
        }
        else
        {
            QStringList cmd = QString::fromUtf8(line).split(' ');
            const QList<QByteArray> cmdBA = line.split(' ');
            QString keyword;
            if (cmd[0].startsWith('@'))
                keyword = cmd[0].mid(1).toAscii();
            else
                continue;
           
            QString col2;
            if (cmd.size() == 3)
                col2 = cmd[2];

            if (keyword == "format")
                ;
            else if (keyword == "minversion")
                m_minimalInstallerVersion = cmdBA[1];
            else if (keyword == "newversion")
            {
                // deprecated
            }
            else if (keyword == "timestamp")
            {
                const QStringList patterns = QStringList() << "yyyyMMddHHmm" << "yyyyMMddHHmmss";
                Q_FOREACH(const QString &pattern, patterns) {
                    if (cmd[1].size() == pattern.size()) {
                        m_timestamp = QDateTime::fromString(cmd[1],pattern);
                        break;
                    }
                }
                qDebug() << m_timestamp;
            }
            else if (keyword == "mirror")
            {
                mirror = new Mirror;
                mirror->url = cmd[1];
                mirror->location = cmd[2];
                m_mirrors.append(mirror);
            }
            else if (keyword == "news")
            {
                QString date = cmd[1];
                QString package = cmd[2];
                cmd.removeFirst();
                cmd.removeFirst();
                cmd.removeFirst();
                m_news[date+"-"+package] = cmd.join(" ");
            }
            else if (keyword == "categorynotes")
            {
                QString category = cmd[1];
                cmd.removeFirst();
                cmd.removeFirst();
                m_categoryNotes[category] = cmd.join(" ");
            }
            else if (keyword == "categorypackages")
            {
                QString category = cmd[1];
                cmd.removeFirst();
                cmd.removeFirst();
                m_categoryPackages[category] << cmd;
            }
            else if (keyword == "metapackage")
            {
                QString metapackage = cmd[1];
                cmd.removeFirst();
                cmd.removeFirst();
                m_metaPackages[metapackage] << cmd;
            }
            else if (keyword == "endusercategories")
            {
                cmd.removeFirst();
                m_endUserCategories << cmd;
            }
            else if (!inSite && keyword == "hashtype")
            {
                if (!m_hashType.setType(cmd[1]))
                    qCritical() << "line" << lineNr << "illegal hash type defined" << cmd[1];
            }
            else if(inPackage)
            {
                if(keyword == "version")
                    pkg->setVersion(cmd[1]);
                else if(keyword.startsWith("url-")) 
                {
                    FileTypes::FileType type;
                    if(keyword == "url-bin")
                        type = FileTypes::BIN;
                    else if(keyword == "url-lib")
                        type = FileTypes::LIB;
                    else if(keyword == "url-doc")
                        type = FileTypes::DOC;
                    else if(keyword == "url-src")
                        type = FileTypes::SRC;
                    else if(keyword == "url-dbg")
                        type = FileTypes::DBG;
                    else
                        continue;
                    QUrl url(cmd[1]);
                    QString fn = col2;
                    if (url.scheme().isEmpty())
                        url = QUrl(m_baseURL + cmd[1]);
                    if (!pkg->hasType(type))
                    {
                        Package::PackageItem item(type);
                        if (item.setUrlAndFileName(url,fn))
                            pkg->add(item);
                    }
                    else
                        pkg->item(type).setUrlAndFileName(url,fn);
                }
                else if(keyword.startsWith("filename-")) 
                {
                    FileTypes::FileType type;
                    if(keyword == "filename-bin")
                        type = FileTypes::BIN;
                    else if(keyword == "filename-lib")
                        type = FileTypes::LIB;
                    else if(keyword == "filename-doc")
                        type = FileTypes::DOC;
                    else if(keyword == "filename-src")
                        type = FileTypes::SRC;
                    else if(keyword == "filename-dbg")
                        type = FileTypes::DBG;
                    else
                        continue;
                    if (!pkg->hasType(type))
                    {
                        Package::PackageItem item(type);
                        item.setFileName(cmd[1]);
                        pkg->add(item);
                    }
                    else
                        pkg->item(type).setFileName(cmd[1]);
                }
                else if(keyword.startsWith("nomd5"))
                {
                    pkg->hashType().setType(Hash::None);
                }
                else if(keyword.startsWith("hashtype"))
                {
                    if (!pkg->hashType().setType(cmd[1]))
                        qCritical() << "line" << lineNr << "illegal hash type defined" << cmd[1];
                }
                else if(keyword.startsWith("hash-")) 
                {
                    // detect case when no type is set
                    if (pkg->hashType().type() == Hash::None)
                    {
                        qCritical() << "line" << lineNr << "there is no hash type specified before";
                        continue;
                    }
                    FileTypes::FileType type;
                    if(keyword == "hash-bin")
                        type = FileTypes::BIN;
                    else if(keyword == "hash-lib")
                        type = FileTypes::LIB;
                    else if(keyword == "hash-doc")
                        type = FileTypes::DOC;
                    else if(keyword == "hash-src")
                        type = FileTypes::SRC;
                    else if(keyword == "hash-dbg")
                        type = FileTypes::DBG;
                    else
                        continue;
                    if (!pkg->hasType(type))
                    {
                        Package::PackageItem item(type);
                        item.setCheckSum(cmd[1]);
                        pkg->add(item);
                    }
                    else
                        pkg->item(type).setCheckSum(cmd[1]);
                }
                else if(keyword.startsWith("md5-")) 
                {
                    pkg->hashType().setType(Hash::MD5);
                    FileTypes::FileType type;
                    if(keyword == "md5-bin")
                        type = FileTypes::BIN;
                    else if(keyword == "md5-lib")
                        type = FileTypes::LIB;
                    else if(keyword == "md5-doc")
                        type = FileTypes::DOC;
                    else if(keyword == "md5-src")
                        type = FileTypes::SRC;
                    else if(keyword == "md5-dbg")
                        type = FileTypes::DBG;
                    else
                        continue;
                    if (!pkg->hasType(type))
                    {
                        Package::PackageItem item(type);
                        item.setCheckSum(cmd[1]);
                        pkg->add(item);
                    }
                    else
                        pkg->item(type).setCheckSum(cmd[1]);
                }
                else if(keyword == "require")
                {
                    cmd.removeFirst();
                    pkg->addDeps(cmd);
                }
                else if(keyword == "notes") {
                    cmd.removeFirst();
                    pkg->setNotes(cmd.join(" "));
                }
                else if(keyword == "details") {
                    cmd.removeFirst();
                    QString notes = cmd.join(" ").replace("\\n","\n");
                    pkg->setLongNotes(notes);
                }
                else if(keyword == "homeurl") 
                {
                    cmd.removeFirst();
                    pkg->setHomeURL(cmd.join(" "));
                }
                else if(keyword == "category")
                {
                    cmd.removeFirst();
                    pkg->addCategories(cmd);
                }
                else if(keyword == "relocate")
                    pkg->addPathRelocation(cmd[1],col2);
                else if(keyword == "control")
                {
                    cmd.removeFirst();
                    InstallerControlType control;
                    if (control.parse(cmd.join(" "))) 
                    {
                        InstallerControlTypeList *l;
                        if (pkg->userData(0))
                            l = (InstallerControlTypeList *)pkg->userData(0);
                        else 
                        {
                            l = new InstallerControlTypeList;
                            pkg->setUserData(0,(void*)l);
                        }
                        l->append(control);
                    }
                    else
                        qCritical() << "line" << lineNr << "invalid InstallerControlType definition" << cmd;
                }
            }
            else if (inSite)
            {
                if(keyword == "siteurl" || keyword == "url")
                {
                    QUrl url(cmd[1]);
                    if (url.scheme().isEmpty()) {
                        QString u = m_baseURL;
                        if( cmd[1] != QLatin1String(".") )
                            u += cmd[1];
                        url = QUrl(u);
                    }
                    site->setURL(url);
                }
                else if(keyword == "url-list")
                {
                    QUrl url(cmd[1]);
                    if (url.scheme().isEmpty()) {
                        QString u = m_baseURL;
                        if( cmd[1] != QLatin1String(".") )
                            u += cmd[1];
                        url = QUrl(u);
                    }
                    site->setListURL(url);
                }
                else if(keyword == "sitetype" || keyword == "type")
                {
                    if (!site->setType(cmd[1]))
                        qCritical() << "unknown site type" << cmd[1];
                }
                else if(keyword == "mirrorurl") 
                {
                    QUrl url(cmd.join(" "));
                    site->addMirror(url);
                }
                else if(keyword == "deps" || keyword == "require") {
                    QString pkg = cmd[1];
                    cmd.removeFirst();
                    cmd.removeFirst();
                    site->addDependencies(pkg, cmd);
                }
                else if(keyword == "exclude") 
                {
                    cmd.removeFirst();
                    site->addExcludes(cmd);
                }
                else if(keyword == "copy") 
                {
                    cmd.removeFirst();
                    site->addCopy(cmd.join(" "));
                }
                else if(keyword == "notes") 
                {
                    cmd.removeFirst();
                    site->setNotes(cmd.join(" "));
                }
                else if(keyword == "pkghomeurl") 
                {
                    QString pkg = cmd[1];
                    QString url = cmd[2];
                    if (pkg.contains("-*"))  
                    {
                        pkg.replace("-*","-%1");
                        foreach(const QString type, packageTypes)
                            site->setPackageHomeUrl(pkg.arg(type),url);
                        pkg.replace("-%1","");
                        site->setPackageHomeUrl(pkg,url);
                    }
                }
                else if(keyword == "pkgnotes") 
                {
                    QString pkg = cmd[1];
                    cmd.removeFirst();
                    cmd.removeFirst();
                    if (pkg.contains("-*")) 
                    {
                        pkg.replace("-*","-%1");
                        foreach(const QString type, packageTypes)
                            site->setPackageNote(pkg.arg(type),cmd.join(" "));
                        site->setPackageNote(pkg,cmd.join(" "));
                    }
                    else
                        site->setPackageNote(pkg,cmd.join(" "));

                }
                else if(keyword == "pkgdetails") {
                    QString pkg = cmd[1];
                    cmd.removeFirst();
                    cmd.removeFirst();
                    QString details = cmd.join(" ").replace("\\n","\n");
                    if (pkg.contains("-*")) 
                    {
                        pkg.replace("-*","-%1");
                        foreach(const QString type, packageTypes)
                            site->setPackageLongNotes(pkg.arg(type),details);
                    }
                    else
                        site->setPackageLongNotes(pkg,details);
                }
                else if(keyword == "pkgcategory") 
                {
                    QString pkg = cmd[1];
                    cmd.removeFirst();
                    cmd.removeFirst();
                    site->setPackageCategories(pkg,cmd);
                }
                else if (keyword == "hashtype")
                {
                    if (!site->hashType().setType(cmd[1]))
                        qCritical() << "line" << lineNr << "illegal hash type defined" << cmd[1];
                }
            }
            else if(keyword == "site")
            {
                site = new Site;
                m_sites.append(site);
                cmd.removeFirst();
                site->setName(cmd.join(" "));
                inSite = true;
            }
            else if(keyword == "package")
            {
                 pkg = new Package;
                 m_packages.append(pkg);
                 cmd.removeFirst();
                 pkg->setName(cmd.join(" "));
                 inPackage=true;
            }
            else
                qWarning() << "unknown tag in config file" << cmd;
        }
    }
    return true;
}

QString GlobalConfig::packageCategory(const QString &package)
{
    Q_FOREACH(const QString& category, m_categoryPackages.keys())
    {
        if(m_categoryPackages[category].contains(package)) return category;
    }
    
    return QString();
}

void GlobalConfig::clear()
{
    qDeleteAll(m_sites);
    m_sites.clear();

    m_news.clear();

    qDeleteAll(m_mirrors);
    m_mirrors.clear();

    qDeleteAll(m_packages);
    m_packages.clear();
    
    m_endUserCategories.clear();
}


QDebug operator<<(QDebug out, const GlobalConfig &c)
{
    out << "GlobalConfig ("
        << "m_baseURL" << c.m_baseURL
        << "m_sites";
    Q_FOREACH(const Site* s, c.m_sites)
        out << *s;

    out << "m_packages";
    Q_FOREACH(const Package* p, c.m_packages)
        out << *p;

    out << "m_mirrors";
    Q_FOREACH(const GlobalConfig::Mirror* m, c.m_mirrors)
        out << *m;

    return out;
}


QDebug operator <<(QDebug out, const GlobalConfig::Mirror &c)
{
    out << " Mirror (" << "url" << c.url << "location" << c.location << ")";
    return out;
}

