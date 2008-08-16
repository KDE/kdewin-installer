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
#include "settings.h"

#include <QDateTime>
#include <QFile>

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

static QFile *logFile = 0;

void myMessageOutput(QtMsgType type, const char *msg)
{
    char *msgtype;
    switch (type) {
     case QtDebugMsg: 
         msgtype = "Debug: ";
         break;
     case QtWarningMsg:
         msgtype = "Warning: ";
         break;
     case QtCriticalMsg:
         msgtype = "Critical: ";
         break;
     case QtFatalMsg:
         msgtype = "Fatal: ";
         break;
    }
    QString data = QDateTime::currentDateTime().toString("[yyyy-MM-dd hh:mm:ss] ") + QLatin1String(msgtype) + msg + QLatin1String("\n");

    ///@ TODO in debug mode write additional to OutputDebugString
    if (logFile)
    {
        logFile->write(data.toLocal8Bit().data());
        logFile->flush();
    }
    switch (type) {
     case QtFatalMsg:
        abort();
        break;
    }
}

/**
 redirect all Qt debug, warning and error messages to a file
*/
void setMessageHandler()
{
    logFile = new QFile(Settings::instance().logFile());
    logFile->remove();
    logFile->open(QIODevice::WriteOnly);

    qInstallMsgHandler(myMessageOutput);
}
