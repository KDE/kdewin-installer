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

GlobalConfig::GlobalConfig(const QString &url, Downloader &downloader)
{
    bool ret; 
    // FIXME: place config files into package download path
    QFileInfo cfr("config-remote.txt");
    qDebug() << "Check if a copy of the remote config file is available at" << cfr.absoluteFilePath() << (cfr.exists() ? "... found" : "... not found");
    if (cfr.exists())
    {
        qDebug() << "skip downloading and parse the copy of the remote config file for parsing";
        ret = parseFromFile("config-remote.txt");
    }
    else 
    {
        QFileInfo cfi("config.txt");
        qDebug() << "download remote config file to" << cfi.absoluteFilePath();
        // FIXME uses version related config file to have more room for format changes
        downloader.start(url,cfi.fileName());
    
        ret = parseFromFile("config.txt");
        qDebug() << "parsing remote config file ... " << (ret == true ? "okay" : "failure") ;
    }
    QFileInfo fi("config-local.txt");
    qDebug() << "Check if a local config file is available at" << fi.absoluteFilePath() << (fi.exists() ? "... found" : "... not found");
    if (fi.exists()) 
    {
        ret = parseFromFile(fi.absoluteFilePath());
        qDebug() << "parse local config file ... " << (ret == true ? "okay" : "failure") ;
    }
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

    while (!ioDev->atEnd())
    {
        QByteArray line = ioDev->readLine().replace("\x0a",""); // support unix format file
        if (line.startsWith(';'))
            continue;
        else if (line.size() < 2)
        { 
           if (inPackage)
           {
			   if (Settings::hasDebug("globalconfig"))
			        pkg->dump(__FUNCTION__);
               inPackage=false;
           }
           else if (inSite)
             inSite=false;
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
            else if (cmd[0] == "@news")
            {
                QString name = cmd[1]; 
                QString version = cmd[2]; 
                cmd.removeFirst();
                cmd.removeFirst();
                cmd.removeFirst();
                m_news[name+"-"+version] = cmd.join(" ");        
            }
            else if(inPackage)
            {
                if(cmd[0] == "@version")
                    pkg->setVersion(cmd[1]);
                else if(cmd[0] == "@url-bin")
                {    
                    Package::PackageItem item;
                    if (item.set(cmd[1],col2,Package::BIN))
                        pkg->add(item);
                }
                else if(cmd[0] == "@url-lib")
                {    
                    Package::PackageItem item;
                    if (item.set(cmd[1],col2,Package::LIB))
                        pkg->add(item);
                }
                else if(cmd[0] == "@url-doc")
                {    
                    Package::PackageItem item;
                    if (item.set(cmd[1],col2,Package::DOC))
                        pkg->add(item);
                }
                else if(cmd[0] == "@url-src")
                {    
                    Package::PackageItem item;
                    if (item.set(cmd[1],col2,Package::SRC))
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
                else if(cmd[0] == "@category")
                    pkg->setCategory(cmd[1]);
                else if(cmd[0] == "@relocate")
                    pkg->addPathRelocation(cmd[1],col2);
            }
            else if (inSite)
            {
                if(cmd[0] == "@siteurl" || cmd[0] == "@url")
                    site->setURL(cmd[1]);
                else if(cmd[0] == "@sitetype" || cmd[0] == "@type")
                    site->setType(cmd[1] == "apachemodindex" ? Site::ApacheModIndex : Site::SourceForge );
                else if(cmd[0] == "@mirrorurl")
                    site->addMirror(cmd[1]);
                else if(cmd[0] == "@deps") {
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
            }
            else if(cmd[0] == "@site")
            {
                site = new Site;
                m_sites.append(site);
                site->setName(cmd[1]);
                inSite = true;
            }
            else if(cmd[0] == "@package") 
            {
                 pkg = new Package;
                 m_packages.append(pkg);
                 pkg->setName(cmd[1]);
                 inPackage=true;
            }
        }
    }
    return true;
}

void GlobalConfig::dump(const QString &title)
{
    DUMP_HEADER(title);
    for (QList<Site*>::iterator s = sites()->begin(); s != sites()->end(); s++)
        (*s)->dump(title);    
  
    for (QList<Package*>::iterator p = packages()->begin(); p != packages()->end(); p++)
        (*p)->dump(title);
    DUMP_FOOTER(title);
}
