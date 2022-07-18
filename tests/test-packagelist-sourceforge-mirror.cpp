/****************************************************************************
**
** Copyright (C) 2007 Ralf Habacker <ralf.habacker@freenet.de>
** All rights reserved.
**
** This file is part of the KDE installer for windows
**
** SPDX-License-Identifier: GPL-2.0-only
****************************************************************************/

#include <QtDebug>
#include <QCoreApplication>

#include "packagelist.h"
#include "downloader.h"
#include "installer.h"


int main(int argc, char ** argv)
{
    QCoreApplication app(argc,argv);

    Downloader *downloader = Downloader::instance();
    PackageList packageList;
    packageList.setBaseURL("http://www.mirrorservice.org/sites/download.sourceforge.net/pub/sourceforge/k/kd/kde-cygwin/");

    qDebug() << "trying to download sourceforge related package list";
    downloader->fetch(QUrl(packageList.baseURL()),"packages.html");

    if (!packageList.readFromFile("packages.html",PackageList::SourceForgeMirror))
    {
        qDebug() << "... failed ";
        return false;
    }
    packageList.listPackages();
    return 0;
}

