/****************************************************************************
**
** Copyright (C) 2005-2006  Ralf Habacker. All rights reserved.
**
** This file is part of the KDE installer for windows
**
** SPDX-License-Identifier: GPL-2.0-only
****************************************************************************/

#include <QCoreApplication>

#include "packagelist.h"
#include "downloader.h"
#include "installer.h"


int main(int argc, char *argv[])
{
    QCoreApplication app(argc, argv);

    testDownloadApacheModIndex();
    testDownloadSourceForge();

    return 0;

}
