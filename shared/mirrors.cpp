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

#include "mirrors.h"
#include "downloader.h"
#include "downloaderprogress.h"
#include <QtDebug>
#include <QFile>
#include <QBuffer>

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
    QString url = MIRROR_BASE_URL_KDE;
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
