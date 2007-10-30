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

/**
@TODO add  region to mirror list (<TH COLSPAN=5 BGCOLOR="YELLOW">...)
*/

//#define DEBUG

#define MIRROR_BASE_URL_KDE "http://download.kde.org/mirrorstatus.html"
#define MIRROR_BASE_URL_CYGWIN "http://www.cygwin.com/mirrors.lst"

/// @param type of mirror
Mirrors::Mirrors(MirrorType type)  : m_type(type)
{}

Mirrors::~Mirrors()
{}

/**
 get the list of mirrors
 @return list of mirrors
*/
QStringList Mirrors::get
    ()
{
    DownloaderProgress progress(0);
    Downloader download(true,&progress);
    QUrl url(MIRROR_BASE_URL_KDE);
#ifdef DEBUG

    QString out = "mirrors.html";
    int ret = download.start(url,out);
    return parse(out) ? m_mirrorList : QStringList();
#else

    QByteArray ba;
    int ret = download.start(url,ba);
    return parse(ba) ? m_mirrorList : QStringList();
#endif
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
#ifdef DEBUG
    qDebug() << "1"  << m_type;
#endif

    switch (m_type)
    {
    case KDE:
        char *lineKey = " <TD ALIGN=\"RIGHT\"><A HREF=\"";
        char *fileKeyStart = "<A HREF=\"";
        char *fileKeyEnd = "\">";
        while (!ioDev->atEnd())
        {
            QByteArray line = ioDev->readLine();
#ifdef DEBUG

            qDebug() << "2"  << line << " " << lineKey;
#endif

            if (line.contains(lineKey))
            {
                int a = line.indexOf(fileKeyStart) + strlen(fileKeyStart);
                int b = line.indexOf(fileKeyEnd,a);
                QByteArray url = line.mid(a,b-a);
#ifdef DEBUG

                qDebug() << "3"  << url;
#endif

                m_mirrorList << url;
            }
        }
        return true;
    }
    return false;

}
