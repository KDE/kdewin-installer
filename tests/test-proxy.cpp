/****************************************************************************
**
** Copyright (C) 2005-2007  Ralf Habacker. All rights reserved.
**
** This file is part of the KDE installer for windows
**
** SPDX-License-Identifier: GPL-2.0-only
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
    bool ret;
    ProxySettings ps;

    ret = ps.from(ProxySettings::InternetExplorer, http);
    qDebug() << "IE settings for " << http << ":" << ret << ps;
    ret = ps.from(ProxySettings::InternetExplorer, ftp);
    qDebug() << "IE settings for " << ftp << ":" << ret << ps;

    ret = ps.from(ProxySettings::FireFox, http);
    qDebug() << "Firefox settings for " << http << ":" << ret << ps;
    ret = ps.from(ProxySettings::FireFox, ftp);
    qDebug() << "Firefox settings for " << ftp << ":" << ret << ps;

    ret = ps.from(ProxySettings::Manual, http);
    qDebug() << "Manual settings for " << http << ":" << ret << ps;
    ret = ps.from(ProxySettings::Manual, ftp);
    qDebug() << "Manual settings for " << ftp << ":" << ret << ps;

    return 0;
}

