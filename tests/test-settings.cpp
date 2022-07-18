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
#include <QDir>

#include "settings.h"

int main(int argc, char *argv[])
{
    QCoreApplication app(argc, argv);

    Settings settings;

    settings.setInstallDir(QDir::currentPath());
    if (settings.installDir() != QDir::currentPath())
        qDebug() << "error reading config file";

    return 0;
}

