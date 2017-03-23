/****************************************************************************
**
** Copyright (C) 2006-2009 Ralf Habacker <ralf.habacker@freenet.de>. 
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

const char *versionRegex = "/(latest|[0-9]{1,2}\\.[0-9]{1,2}\\.[0-9]{1,2}|[0-9]{8})";
const char *branchRegex = "/(Attic|unstable|stable|nightly)";

Releases::Releases()
{
}

Releases::~Releases()
{
    clear();
}

/**
 * check if a single release is mentioned
 * @param url
 * @return true if single release is specified in the url
 * @return false if single release is not specified in the url
 */
bool Releases::isSingleRelease(QUrl &url)
{
    QRegExp rx(versionRegex);
    int i = rx.indexIn(url.path());
    return i != -1;
}

QString Releases::singleRelease(QUrl &url)
{
    QRegExp rx(versionRegex);
    int i = rx.indexIn(url.path());
    return i != -1 ? rx.cap(1) : "";
}

bool Releases::isBranchRelease(QUrl &url)
{
    QRegExp rx(branchRegex);
    int i = rx.indexIn(url.path());
    return i != -1;
}

QString Releases::branchRelease(QUrl &url)
{
    QRegExp rx(branchRegex);
    int i = rx.indexIn(url.path());
    return i != -1 ? rx.cap(1) : "";
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
        MirrorReleaseType release;
        release.name = version;
        release.url = url;
        release.type = Stable;
        m_releases.append(release);
        return true;
    }    
    aString = "/unstable/";
    i = path.indexOf(aString);
    if (i != -1)
    {
        int k = path.indexOf('/',i+aString.size()+1);
        QString version = path.mid(i+aString.size(),k-i-aString.size());
        MirrorReleaseType release;
        release.name = version;
        release.url = url;
        release.type = Unstable;
        m_releases.append(release);
        return true;
    }    
    aString = "/nightly/";
    i = path.indexOf(aString);
    if (i != -1)
    {
        int k = path.indexOf('/',i+aString.size()+1);
        QString version = path.mid(i+aString.size(),k-i-aString.size());
        MirrorReleaseType release;
        release.name = version;
        release.url = url;
        release.type = Nightly;
        m_releases.append(release);
        return true;
    }    
    return false;
}

bool Releases::checkIfReleasesArePresent(const QUrl &url)
{
    // wwww.winkde.org and sf uses flat directory structure below the version dir, so no extra action is required here 
    if (url.host() == "www.winkde.org" 
        || url.host().endsWith("sourceforge.net")
        || url.host() == "sf.net")
        return true;

    // kde mirrors uses a win32 subdir which has to be checked for the existance of a win32 release
    /// @TODO add a fetchSilent() method 
    DownloaderProgress *old = Downloader::instance()->progress();
    Downloader::instance()->setProgress(0);

    MirrorReleaseTypeList temp = m_releases;
    m_releases.clear();
    QByteArray out;

    Q_FOREACH( MirrorReleaseType r, temp )
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
    m_baseURL = _url;

    if (isSingleRelease(m_baseURL))
    {
        MirrorReleaseType release;
        release.url = m_baseURL;
        release.name = singleRelease(m_baseURL);
        release.type = Single;
        m_releases.append(release);
        return true;
    }

    QMap<ReleaseType, QUrl> releases;

    if (isBranchRelease(m_baseURL))
    {
        QString branch = branchRelease(m_baseURL);
        ReleaseType type = toReleaseType(branch);
        releases[type] = m_baseURL;
    }
    else
    {
        releases[Stable] = m_baseURL.toString() + "stable/";
        releases[Unstable] = m_baseURL.toString() + "unstable/";
        releases[Nightly] = m_baseURL.toString() + "nightly/";
        releases[Attic] = m_baseURL.toString() + "Attic/";
    }

    foreach(ReleaseType key, releases.keys())
    {
        const QUrl &url = releases[key];
        if (!Downloader::instance()->fetch(url, out))
        {
            qWarning() << "could not fetch" << toString(key) << "versions from" << url;
        }
        else if (!parse(out, url, key))
        {
            qWarning() << "could not extract" << toString(key) << "versions from directory list fetched from" << url;
        }
    }

    return checkIfReleasesArePresent(m_baseURL);
}

bool Releases::parse(const QString &fileName, const QUrl &url, ReleaseType type)
{
    QFile file(fileName);
    if (!file.exists())
        return false;

    if (!file.open(QIODevice::ReadOnly))
        return false;
    return parse(&file,url,type);
}

bool Releases::parse(const QByteArray &data, const QUrl &url, ReleaseType type)
{
    QByteArray ba(data);
    QBuffer buf(&ba);

    if (!buf.open(QIODevice::ReadOnly| QIODevice::Text))
        return false;
    return parse(&buf,url,type);
}

bool Releases::parse(QIODevice *ioDev, const QUrl &url, ReleaseType type)
{
    if (url.scheme() == "http" || url.scheme() == "https")
    {
        while (!ioDev->atEnd())
        {
            const QString line = QString::fromUtf8(ioDev->readLine().replace("\n",""));
            if ( line.startsWith(QLatin1Char('#')) ) 
                continue;
            if ( !line.contains("href=\""))
                continue;
            int start = line.indexOf(QRegExp("href=\"(latest|[0-9]+\\.[0-9]+\\.[0-9]+/|[0-9]{8}/)"), 0);   
            if (start == -1)
					continue;
                
            int end = line.indexOf('\"',start+6);
            QString version = line.mid(start+6,end-start-6).remove('/');
            qDebug() << "line:" << line << version;
            MirrorReleaseType release;
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
            if (version.indexOf(QRegExp("^([0-9]+\\.[0-9]+\\.[0-9]+|[0-9]{8})$"), 0) == -1)   
                continue;
                
            MirrorReleaseType release;
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

QDebug &operator<<(QDebug &out, const MirrorReleaseTypeList  &c)
{
    out << "QList<ReleaseType> (";
    Q_FOREACH(const MirrorReleaseType &m,c)
        out << m;
    out << ")";
    return out;
}

QDebug &operator<<(QDebug &out, const MirrorReleaseType &c)
{
    out << "ReleaseType ("
        << "url:" << c.url
        << "name:" << c.name
        << "type:" << toString(c.type)
        << ")";
    return out;
}
