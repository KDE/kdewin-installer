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
#include <stdio.h>

#include "misc.h"

int main(int argc, char *argv[])
{
    QCoreApplication app(argc, argv);

    createStartMenuEntries(argv[1],argv[2]);

    return 0;
}

