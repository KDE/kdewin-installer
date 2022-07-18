/****************************************************************************
**
** Copyright (C) 2009 Ralf Habacker. All rights reserved.
**
** This file is part of the KDE installer for windows
**
** SPDX-License-Identifier: LGPL-2.0-only
**
****************************************************************************/

#include "qio.h"

#include <QTextStream>

#include <stdio.h>

static const int QT_BUFFER_LENGTH = 8192;       // internal buffer length

void qprintf(const char *msg, ...)
{
    char buf[QT_BUFFER_LENGTH];
    buf[QT_BUFFER_LENGTH - 1] = '\0';
    va_list ap;
    va_start(ap, msg);                        // use variable arg list
    if (msg)
        vsnprintf(buf, QT_BUFFER_LENGTH - 1, msg, ap);
    va_end(ap);
    fputs(buf,stdout);
}

QTextStream qout(stdout);

QTextStream qerr(stderr);
