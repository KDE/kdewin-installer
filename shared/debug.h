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

#ifndef DEBUG_H
#define DEBUG_H

#include <QtDebug>

// own debug functions
QDebug _qDebug(const char *file, int line);
QDebug _qInfo(const char *file, int line);
QDebug _qWarning(const char *file, int line);
QDebug _qCritical(const char *file, int line);
QDebug _qFatal(const char *file, int line);

// wrapper for original q... calls 
#define qDebug() _qDebug(__FILE__,__LINE__)
#define qInfo() _qInfo(__FILE__,__LINE__)
#define qWarning() _qWarning(__FILE__,__LINE__)
#define qCritical() _qCritical(__FILE__,__LINE__)
#define qFatal() _qFatal(__FILE__,__LINE__)

/// return stdout stream
QTextStream &qOut();
/// return stderr stream
QTextStream &qError();


/// set message handler
void setMessageHandler();

/// close message handler
void closeMessageHandler();


#endif
