/****************************************************************************
**
** Copyright (C) 2009 Ralf Habacker. All rights reserved.
**
** This file is part of the KDE installer for windows
**
** SPDX-License-Identifier: LGPL-2.0-only
**
****************************************************************************/

/** 
 \file Qt Input/output system releated stuff like qout, qerr, qprintf
*/ 

#ifndef QIO_H
#define QIO_H

#include <QTextStream>

/// cout replacement - with QString support
extern QTextStream qout;

/// cerr replacement - with QString support
extern QTextStream qerr;

/**
    printf replacement 
*/
void qprintf(const char *msg, ...);

#endif
