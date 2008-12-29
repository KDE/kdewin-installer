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

bool Releases::convertFromOldMirrorUrl(QUrl &url)
{
    // in case the url contains already a release path, remove this part as the following code will detect the releases by itself
    QString path = url.path();
    int i = path.indexOf(QRegExp("/(unstable|stable)/(latest|[0-9]{1,2}\\.[0-9]{1,2}\\.[0-9]{1,2})"));
    if (i != -1)
    {
        url.setPath(path.left(i) + '/');
        return false;
    }
    return false;
}

bool Releases::useOldMirrorUrl(const QUrl &url)
{
    // in case the url contains already a release path, transform the url to the ReleaseTyp 
    QString path = url.path();
    QString aString = "/stable/";
    int i = path.indexOf(aString);
    if (i != -1)
    {
        int k = path.indexOf('/',i+aString.size()+1);
        QString version = path.mid(i+aString.size(),k-i-aString.size());
        ReleaseType release;
        release.name = version;
        release.url = url;
        release.type = ReleaseType::Stable;
        m_releases.append(release);
        return true;
    }    
    aString = "/unstable/";
    i = path.indexOf(aString);
    if (i != -1)
    {
        int k = path.indexOf('/',i+aString.size()+1);
        QString version = path.mid(i+aString.size(),k-i-aString.size());
        ReleaseType release;
        release.name = version;
        release.url = url;
        release.type = ReleaseType::Unstable;
        m_releases.append(release);
        return true;
    }    
    return false;
}

bool Releases::patchReleaseUrls(const QUrl &url)
{
    // wwww.winkde.org and sf uses flat directory structure below the version dir, so no extra action is required here 
    if (url.host() == "www.winkde.org" 
        || url.host() == "sourceforge.net" 
        || url.host() == "sf.net" 
        || url.host() == "downloads.sourceforge.net" )
        return true;

    // kde mirrors uses a win32 subdir which has to be checked for the existance of a win32 release
    /// @TODO add a fetchSilent() method 
    DownloaderProgress *old = Downloader::instance()->progress();
    Downloader::instance()->setProgress(0);

    ReleaseTypeList temp = m_releases;
    m_releases.clear();
    QByteArray out;

    Q_FOREACH( ReleaseType r, temp )
    {
        QUrl u = r.url.toString() + "win32/config.txt";
        if (Downloader::instance()->fetch(u,out)) 
        {
            r.url = r.url.toString() + "win32/";
            m_releases.append(r);
        }
    }
    Downloader::instance()->setProgress(old);
    return true;
}

bool Releases::fetch(const QUrl &_url)
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
    QUrl baseURL = _url;

    if (convertFromOldMirrorUrl(baseURL))
        return true;

    QUrl url = baseURL.toString() + "stable/";
    qWarning() << "baseURL1:" << url;
    if (!Downloader::instance()->fetch(url,out))
    {
        qWarning() << "could not fetch stable versions from" << url;
    }
    else if (!parse(out,url,ReleaseType::Stable))
    {
        qWarning() << "could not extract stable versions from directory list fetched from" << url;
    }

    url = baseURL.toString() + "unstable/";
    if (!Downloader::instance()->fetch(url,out))
    {
        qWarning() << "could not fetch unstable versions from" << url;
    }
    else if (!parse(out,url,ReleaseType::Unstable))
    {
        qWarning() << "could not extract unstable versions from directory list fetched from" << url;
    }

    return patchReleaseUrls(baseURL);
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
                
            int end = line.indexOf('\"',start+6);
            QString version = line.mid(start+6,end-start-6).remove('/');
            qDebug() << "line:" << line << version;
            ReleaseType release;
            release.url = url.toString() + version + '/';
            release.name = version;
            release.type = type;
            m_releases.append(release);
        }
    }
    else if (url.scheme() == "ftp") 
    {
        // parse ftp listing
        //drwxr-xr-x    4 emoenke  ftp          4096 May  6 22:06 4.0.4
        while (!ioDev->atEnd())
        {
            const QString line = QString::fromUtf8(ioDev->readLine().replace("\n", "").replace("\r", ""));
            if (!line.startsWith(QLatin1Char('d')) ) 
                continue;
            QStringList a = line.split(" ",QString::SkipEmptyParts);
            if (a.size() < 9)
                continue;
            QString version = a[8].remove('/');
            
            // check syntax  x.y.z 
            if (version.indexOf(QRegExp("^[0-9]+\\.[0-9]+\\.[0-9]+$"), 0) == -1)   
                continue;
                
            ReleaseType release;
            release.url = url.toString() + version + '/';
            release.name = version;
            release.type = type;
            m_releases.append(release);
        }

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
        << "type:" << (c.type == ReleaseType::Stable ? "stable" : "unstable")
        << ")";
    return out;
}
