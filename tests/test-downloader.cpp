/****************************************************************************
**
** Copyright (C) 2005-2006  Ralf Habacker. All rights reserved.
**
** This file is part of the KDE installer for windows
**
** SPDX-License-Identifier: GPL-2.0-only
****************************************************************************/

#include <QtGlobal>
#include <QCoreApplication>
#include <QtDebug>

#include  "downloader.h"
#include  "downloaderprogress.h"

int main(int argc, char *argv[])
{
    QCoreApplication app(argc, argv);

    Downloader *d = Downloader::instance();

    qDebug() << "download return code" << d->fetch(QUrl("https://download.kde.org/Attic/4.10.2/win32/config.txt"),"config.txt");
    qDebug() << "download return code" << d->fetch(QUrl("http://go.microsoft.com/fwlink/?LinkId=51410&clcid=0x409"),"vcexpress.exe");
    qDebug() << "download return code" << d->fetch(QUrl("ftp://ftp.gwdg.de/pub/linux/kde/Attic/4.10.2/win32/config.txt"),"config-ftp.txt");
    qDebug() << "download return code" << d->fetch(QUrl("http://ftp.gwdg.de/pub/linux/kde/Attic/4.10.2/win32/"),"config-dir.txt");
    qDebug() << "download return code" << d->fetch(QUrl("ftp://ftp.gwdg.de/pub/linux/kde/Attic/4.10.2/win32/"),"config-ftp-dir.txt");
    return 0;
}
