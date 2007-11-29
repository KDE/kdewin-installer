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

#include "config.h"
#include "globalconfig.h"
#include "downloader.h"

#include <QFileInfo>
#include <QBuffer>
#include <QMessageBox>
#include <QUrl>

GlobalConfig::GlobalConfig(Downloader *downloader)
{
    m_downloader = downloader;
}

QStringList GlobalConfig::fetch(const QString &baseURL)
{
    bool ret;
    m_baseURL = baseURL;

    QStringList configFiles;
    // remote config file
    if (baseURL.startsWith("http") || baseURL.startsWith("ftp"))
    {
        QFileInfo cfr(Settings::getInstance().downloadDir()+"/config-remote.txt");
        if (Settings::hasDebug("GlobalConfig"))
            qDebug() << "Check if a copy of the remote config file is available at" << cfr.absoluteFilePath() << (cfr.exists() ? "... found" : "... not found");
        if (cfr.exists())
        {
            configFiles << cfr.absoluteFilePath();
        }
        else
        {
            QFileInfo cfi(Settings::getInstance().downloadDir()+"/config.txt");
            ret = m_downloader->start(baseURL + "/installer/config.txt",cfi.absoluteFilePath());
            if (Settings::hasDebug("GlobalConfig"))
                qDebug() << "download remote config file to" << cfi.absoluteFilePath() << "..." << (ret == true ? "okay" : "failure") ;
            if (ret)
                configFiles << cfi.absoluteFilePath();

        }

        QFileInfo fi(Settings::getInstance().downloadDir()+"/config-local.txt");
        if (Settings::hasDebug("GlobalConfig"))
            qDebug() << "Check if a local config file is available at" << fi.absoluteFilePath() << (fi.exists() ? "... found" : "... not found");
        if (fi.exists())
            configFiles << fi.absoluteFilePath();

    }
    return configFiles;
}

bool GlobalConfig::parse(const QStringList &configFiles)
{
    bool ret;
    foreach(QString configFile, configFiles) {
        ret = parseFromFile(configFile);
        if (Settings::hasDebug("GlobalConfig"))
            qDebug() << "parse config file " << configFile << (ret == true ? "okay" : "failure") ;
    }
    return ret;
}

GlobalConfig::~GlobalConfig()
{
    qDeleteAll(m_sites);
    qDeleteAll(m_packages);
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
    Site *site;
    Package *pkg;
    Mirror *mirror;

    while (!ioDev->atEnd())
    {
        QByteArray line = ioDev->readLine().replace('\x0a', ""); // support unix format file
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
        else if (line.startsWith('@'))
        {
            // TODO: encoding of config file! Currently: ascii
            QString l = line;
            QStringList cmd = l.split(' ');
            QString col2;
            if (cmd.size() == 3)
                col2 = cmd[2];

            if (cmd[0] == "@format")
                ;
            else if (cmd[0] == "@timestamp")
            {
                m_timestamp = QDateTime::fromString(cmd[1],"YYYYMMddHHmm");
            }
            else if (cmd[0] == "@mirror")
            {
                mirror = new Mirror;
                mirror->url = cmd[1];
                mirror->location = cmd[2];
                m_mirrors.append(mirror);
            }
            else if (cmd[0] == "@news")
            {
                QString date = cmd[1];
                QString package = cmd[2];
                cmd.removeFirst();
                cmd.removeFirst();
                cmd.removeFirst();
                m_news[date+"-"+package] = cmd.join(" ");
            }
            else if(inPackage)
            {
                if(cmd[0] == "@version")
                    pkg->setVersion(cmd[1]);
                else if(cmd[0] == "@url-bin")
                {
                  QUrl url(cmd[1]);
                  if (url.scheme().isEmpty())
                    url = QUrl(m_baseURL + '/' + cmd[1]);
                  Package::PackageItem item;
                  if (item.set(url,col2,Package::BIN))
                    pkg->add(item);
                }
                else if(cmd[0] == "@url-lib")
                {
                  QUrl url(cmd[1]);
                  if (url.scheme().isEmpty())
                    url = QUrl(m_baseURL + '/' + cmd[1]);
                  Package::PackageItem item;
                  if (item.set(url,col2,Package::LIB))
                    pkg->add(item);
                }
                else if(cmd[0] == "@url-doc")
                {
                  QUrl url(cmd[1]);
                  if (url.scheme().isEmpty())
                    url = QUrl(m_baseURL + '/' + cmd[1]);
                  Package::PackageItem item;
                  if (item.set(url,col2,Package::DOC))
                    pkg->add(item);
                }
                else if(cmd[0] == "@url-src")
                {
                  QUrl url(cmd[1]);
                  if (url.scheme().isEmpty())
                    url = QUrl(m_baseURL + '/' + cmd[1]);
                  Package::PackageItem item;
                  if (item.set(url,col2,Package::SRC))
                    pkg->add(item);
                }
                else if(cmd[0] == "@require")
                {
                    cmd.removeFirst();
                    pkg->addDeps(cmd);
                }
                else if(cmd[0] == "@notes") {
                    cmd.removeFirst();
                    pkg->setNotes(cmd.join(" "));
                }
                else if(cmd[0] == "@details") {
                    cmd.removeFirst();
                    pkg->setLongNotes(cmd.join(" "));
                }
                else if(cmd[0] == "@category")
                {
                    cmd.removeFirst();
                    pkg->addCategories(cmd);
                }
                else if(cmd[0] == "@relocate")
                    pkg->addPathRelocation(cmd[1],col2);
            }
            else if (inSite)
            {
                if(cmd[0] == "@siteurl" || cmd[0] == "@url")
                {
                    QUrl url(cmd.join(" "));
                    if (url.scheme().isEmpty())
                        url = QUrl(m_baseURL + '/' + cmd[1]);
                    site->setURL(url);
                }
                else if(cmd[0] == "@sitetype" || cmd[0] == "@type")
                    site->setType(cmd[1] == "apachemodindex" ? Site::ApacheModIndex : Site::SourceForge );
                else if(cmd[0] == "@mirrorurl") {
                    QUrl url(cmd.join(" "));
                    site->addMirror(url);
                }
                else if(cmd[0] == "@deps" || cmd[0] == "@require") {
                    QString dep = cmd[1];
                    cmd.removeFirst();
                    cmd.removeFirst();
                    site->addDependencies(dep, cmd);
                }
                else if(cmd[0] == "@exclude") {
                    cmd.removeFirst();
                    site->addExcludes(cmd);
                }
                else if(cmd[0] == "@copy") {
                    cmd.removeFirst();
                    site->addCopy(cmd.join(" "));
                }
                else if(cmd[0] == "@notes") {
                    cmd.removeFirst();
                    site->setNotes(cmd.join(" "));
                }
                else if(cmd[0] == "@pkgnotes") {
                    QString pkg = cmd[1];
                    cmd.removeFirst();
                    cmd.removeFirst();
                    site->setPackageNote(pkg,cmd.join(" "));
                }
                else if(cmd[0] == "@pkgdetails") {
                    QString pkg = cmd[1];
                    cmd.removeFirst();
                    cmd.removeFirst();
                    site->setPackageLongNotes(pkg,cmd.join(" "));
                }
            }
            else if(cmd[0] == "@site")
            {
                site = new Site;
                m_sites.append(site);
                cmd.removeFirst();
                site->setName(cmd.join(" "));
                inSite = true;
            }
            else if(cmd[0] == "@package")
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

void GlobalConfig::clear()
{
    m_sites.clear();
    m_news.clear();
    m_mirrors.clear();
    m_packages.clear();
}


QDebug &operator<<(QDebug &out,GlobalConfig &c)
{
    out << c.m_baseURL;
    for (QList<Site*>::Iterator s = c.m_sites.begin(); s != c.m_sites.end(); s++)
        out << *s;

    for (QList<Package*>::Iterator p = c.m_packages.begin(); p != c.m_packages.end(); p++)
        out << *p;

    for (QList<GlobalConfig::Mirror*>::Iterator p = c.m_mirrors.begin(); p != c.m_mirrors.end(); p++)
        out << *p;

    return out;
}

