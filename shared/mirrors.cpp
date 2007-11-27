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

#include "mirrors.h"
#include "downloader.h"
#include "downloaderprogress.h"
#include <QtDebug>
#include <QBuffer>
#include <QFile>
#include <QUrl>
#include <QStringList>

/**
@TODO add  region to mirror list (<TH COLSPAN=5 BGCOLOR="YELLOW">...)
*/

//#define DEBUG

#define MIRROR_BASE_URL_KDE "http://download.kde.org/mirrorstatus.html"
#define MIRROR_BASE_URL_CYGWIN "http://www.cygwin.com/mirrors.lst"

Mirrors::Mirrors() 
    : m_type(Cygwin) 
{
}

/**
 get the list of mirrors
 @return list of mirrors
*/
bool Mirrors::fetch(Type type, QUrl url)
{
    m_type = type;
    DownloaderProgress progress(0);
    Downloader download(true,&progress);
#ifdef DEBUG

    QString out = "mirrors.html";
#else
    QByteArray out;
#endif
    if (!download.start(url,out))
        return false;
    return parse(out);
}

/**
 parse mirror list from a local file

 @param filename
 @return true if parse was performed successfully, false otherwise
*/

bool Mirrors::parse(const QString &fileName)
{
    QFile file(fileName);
    if (!file.exists())
        return false;

    file.open(QIODevice::ReadOnly);
    return parse(&file);
}

bool Mirrors::parse(const QByteArray &data)
{
    QByteArray ba(data);
    QBuffer buf(&ba);

    if (!buf.open(QIODevice::ReadOnly| QIODevice::Text))
        return false;
    return parse(&buf);
}

bool Mirrors::parse(QIODevice *ioDev)
{
    m_mirrors.clear();
    switch (m_type) {
    case KDE:
        {
            char *lineKey = " <TD ALIGN=\"RIGHT\"><A HREF=\"";
            char *fileKeyStart = "<A HREF=\"";
            char *fileKeyEnd = "\">";
            while (!ioDev->atEnd())
            {
                QByteArray line = ioDev->readLine();
                if (line.contains(lineKey))
                {
                    int a = line.indexOf(fileKeyStart) + strlen(fileKeyStart);
                    int b = line.indexOf(fileKeyEnd,a);
                    QByteArray url = line.mid(a,b-a);
                    MirrorType mirror;
                    mirror.url = url;
                    m_mirrors.append(mirror);
                }
            }
        }
        break;

    case Cygwin:
        {
            while (!ioDev->atEnd())
            {
                QByteArray line = ioDev->readLine().replace("\n","");
                if (line.startsWith("#"))
                    continue;
                QList<QByteArray> a = line.split(';');
                MirrorType mirror;
                mirror.url = a[0];
                mirror.name = a[1];
                mirror.continent = a[2];
                mirror.country = a[3];
                m_mirrors.append(mirror);
            }
        }
        break;
    }
    return true;
}

Mirrors &Mirrors::getInstance()
{
    static Mirrors mirrors;
    return mirrors;
}    

QDebug &operator<<(QDebug &out, const MirrorTypeList &c)
{
    out << "QList<MirrorType> (";
    foreach(MirrorType m,c)
        out << m;
    out << ")";
    return out;
}

QDebug &operator<<(QDebug &out, const MirrorType &c)
{
    out << "MirrorType ("
        << "url:" << c.url
        << "name:" << c.name
        << "continent:" << c.continent
        << "country:" << c.country
        << ")";
    return out;
}
