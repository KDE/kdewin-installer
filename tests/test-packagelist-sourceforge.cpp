/****************************************************************************
**
** Copyright (C) 2005-2006  Ralf Habacker. All rights reserved.
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

    qDebug() << "trying to download sourceforge related package list";
    downloader->fetch(QUrl("http://sourceforge.net/project/showfiles.php?group_id=23617"),"packages.html");

    if (!packageList.readFromFile("packages.html",PackageList::SourceForge))
    {
        qDebug() << "... failed ";
        return false;
    }
    packageList.listPackages();
    return 0;
}

