/****************************************************************************
**
** Copyright (C) 2006-2008 Ralf Habacker <ralf.habacker@freenet.de>. 
** All rights reserved.
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

#include "downloader.h"
#include "downloaderprogress.h"
#include "releases.h"
#include "settings.h"

#include <QtDebug>
#include <QBuffer>
#include <QFile>
#include <QUrl>
#include <QStringList>
#include <QFileInfo>

Releases::Releases()
{
}

Releases::~Releases()
{
    clear();
}

bool Releases::fetch(const QUrl &baseURL)
{
#ifdef DEBUG
    QString out = Settings::instance().downloadDir() + "/releases.html";
#else
    QByteArray out;
#endif
    QFileInfo cfr(Settings::instance().downloadDir()+"/releases-remote.html");
    if (Settings::hasDebug("Releases"))
        qDebug() << "Check if a copy of the remote config file is available at" << cfr.absoluteFilePath() << (cfr.exists() ? "... found" : "... not found");
    if (cfr.exists())
    {
#ifdef DEBUG
        out = cfr.absolutePath();
#else
        QFile f(cfr.absoluteFilePath());
        if (f.open(QFile::ReadOnly)) 
        {
            out = f.readAll();
            f.close();
            return true;
        }
        else 
        {
          qCritical() << "could not open" << cfr.absoluteFilePath() << "with error" << f.errorString();
          return false;
        }
#endif
    }   
    m_releases.clear();
    QUrl url = baseURL.toString() + "/stable";
    if (!Downloader::instance()->fetch(url,out))
        return false;
    if (!parse(out,url,ReleaseType::Stable))
        return false;

    url = baseURL.toString() + "/unstable";
    if (!Downloader::instance()->fetch(url,out))
        return false;
    if (!parse(out,url,ReleaseType::Unstable))
        return false;

    return true;
}

bool Releases::parse(const QString &fileName, const QUrl &url, ReleaseType::Type type)
{
    QFile file(fileName);
    if (!file.exists())
        return false;

    if (!file.open(QIODevice::ReadOnly))
        return false;
    return parse(&file,url,type);
}

bool Releases::parse(const QByteArray &data, const QUrl &url, ReleaseType::Type type)
{
    QByteArray ba(data);
    QBuffer buf(&ba);

    if (!buf.open(QIODevice::ReadOnly| QIODevice::Text))
        return false;
    return parse(&buf,url,type);
}

bool Releases::parse(QIODevice *ioDev, const QUrl &url, ReleaseType::Type type)
{
    if (url.scheme() == "http") 
    {
        while (!ioDev->atEnd())
        {
            const QString line = QString::fromUtf8(ioDev->readLine().replace("\n",""));
            if ( line.startsWith(QLatin1Char('#')) ) 
                continue;
            if ( !line.contains("href=\""))
                continue;
            int start = line.indexOf(QRegExp("href=\"[0-9]+\\.[0-9]+\\.[0-9]+/"), 0);   
            if (start == -1)
                continue;
                
            int end = line.indexOf("\"",start+6);
            QString x = line.mid(start+6,end-start-6);
            qDebug() << line << x;
            ReleaseType release;
            release.url = url.toString() + "/" + x;
            release.name = x.replace("/","");
            release.type = type;
            m_releases.append(release);
        }
    }
    else if (url.scheme() == "ftp") 
    {
        // parse ftp listing
    } 

    qDebug() << m_releases;
    return true;
}

Releases &Releases::instance()
{
    static Releases Releases;
    return Releases;
}

QDebug &operator<<(QDebug &out, const ReleaseTypeList  &c)
{
    out << "QList<ReleaseType> (";
    Q_FOREACH(const ReleaseType &m,c)
        out << m;
    out << ")";
    return out;
}

QDebug &operator<<(QDebug &out, const ReleaseType &c)
{
    out << "ReleaseType ("
        << "url:" << c.url
        << "name:" << c.name
        << ")";
    return out;
}
