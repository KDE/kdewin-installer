/****************************************************************************
**
** Copyright (C) 2008 Ralf Habacker. All rights reserved.
**
** This file is part of the KDE installer for windows
**
** This library is free software; you can redistribute it and/or
** modify it under the terms of the GNU Library General Public
** License version 2 as published by the Free Software Foundation.
**
** This library is distributed in the hope that it will be useful,
** but WITHOUT ANY WARRANTY; without even the implied warranty of
** MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU
** Library General Public License for more details.
**
** You should have received a copy of the GNU Library General Public License
** along with this library; see the file COPYING.LIB.  If not, write to
** the Free Software Foundation, Inc., 51 Franklin Street, Fifth Floor,
** Boston, MA 02110-1301, USA.
**
****************************************************************************/

#include "debug.h"

QDebug _qDebug(const char *file, int line) 
{ 
    QDebug a(QtDebugMsg); 
    a << file << line; 
    return a; 
}

QDebug _qInfo(const char *file, int line) 
{ 
    QDebug a(QtDebugMsg); 
    a << file << line; 
    return a; 
}
QDebug _qWarning(const char *file, int line) 
{ 
    QDebug a(QtWarningMsg); 
    a << file << line; 
    return a; 
}
QDebug _qCritical(const char *file, int line) 
{ 
    QDebug a(QtCriticalMsg); 
    a << file << line; 
    return a; 
}
QDebug _qFatal(const char *file, int line) 
{ 
    QDebug a(QtFatalMsg); 
    a << file << line; 
    return a; 
}
