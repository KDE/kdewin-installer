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

#include "config.h"
#include "globalconfig.h"
#include "downloader.h"
#include "downloaderprogress.h"

int main(int argc, char *argv[])
{
    QCoreApplication app(argc, argv);

    Downloader *downloader = Downloader::instance();

    GlobalConfig globalConfig;
    QStringList files = globalConfig.fetch("http://82.149.170.66/kde-windows/installer");
    globalConfig.parse(files);
    qDebug() << globalConfig;

    GlobalConfig localConfig;
    localConfig.parse(QStringList() << CMAKE_SOURCE_DIR "/tests/data/config-site.txt");
    qDebug() << localConfig;
    return 0;
}

