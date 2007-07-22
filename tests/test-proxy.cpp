/****************************************************************************
**
** Copyright (C) 2005-2007  Ralf Habacker. All rights reserved.
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

#include <QtDebug>
#include <QCoreApplication>
#include <stdio.h>

#include "settings.h"

int main(int argc, char *argv[])
{
    QCoreApplication app(argc, argv);

    QString http("http://www.heise.de");
    QString ftp("ftp://www.heise.de");
    QString host; 
    int port; 
    bool ret;
    Settings &s = Settings::getInstance();

    s.setProxyMode(Settings::InternetExplorer);
    ret = s.getProxySettings(http, host, port);
    qDebug() << "IE settings for " << http << ":" << ret << host << port;
    ret = s.getProxySettings(ftp, host, port);
    qDebug() << "IE settings for " << ftp << ":" << ret << host << port;

    s.setProxyMode(Settings::FireFox);
    ret = s.getProxySettings(http, host, port);
    qDebug() << "Firefox settings for " << http << ":" << ret << host << port;
    ret = s.getProxySettings(ftp, host, port);
    qDebug() << "Firefox settings for " << ftp << ":" << ret << host << port;

    s.setProxyMode(Settings::Manual);
    ret = s.getProxySettings(http, host, port);
    qDebug() << "Manual settings for " << http << ":" << ret << host << port;
    ret = s.getProxySettings(ftp, host, port);
    qDebug() << "Manual settings for " << ftp << ":" << ret << host << port;

    return 0;
}

