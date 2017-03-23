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

#ifdef Q_OS_WIN32
#include <windows.h>
#define EOL "\r\n"
#else
#define EOL "\n"
#endif

static int fileOffset = 0;

QDebug _qDebug(const char *file, int line) 
{ 
    QDebug a(QtDebugMsg); 
    a << file+fileOffset << line; 
    return a; 
}

QDebug _qInfo(const char *file, int line) 
{ 
    QDebug a(QtDebugMsg); 
    a << file+fileOffset << line; 
    return a; 
}
QDebug _qWarning(const char *file, int line) 
{ 
    QDebug a(QtWarningMsg); 
    a << file+fileOffset << line; 
    return a; 
}
QDebug _qCritical(const char *file, int line) 
{ 
    QDebug a(QtCriticalMsg); 
    a << file+fileOffset << line; 
    return a; 
}
QDebug _qFatal(const char *file, int line) 
{ 
    QDebug a(QtFatalMsg); 
    a << file+fileOffset << line; 
    return a; 
}

QTextStream &qOut() 
{ 
    static QTextStream a(stdout);
    return a; 
}

QTextStream &qError() 
{ 
    static QTextStream a(stderr);
    return a; 
}

static QFile *logFile = 0;
static QByteArray *logData = 0;

#if QT_VERSION > 0x050000
void myMessageOutput(QtMsgType type, const QMessageLogContext& context, const QString& msg)
#else
void myMessageOutput(QtMsgType type, const char *msg)
#endif
{
    const char *msgtype;
    const char *msgColor;
    switch (type) {
     case QtDebugMsg: 
         msgtype = "Debug: ";
         msgColor = "black";
         break;
     case QtWarningMsg:
         msgtype = "Warning: ";
         msgColor = "GoldenRod";
         break;
     case QtCriticalMsg:
         msgtype = "Critical: ";
         msgColor = "Red";
         break;
     case QtFatalMsg:
         msgtype = "Fatal: ";
         msgColor = "DarkRed";
         break;
    }
    QString date = QDateTime::currentDateTime().toString("[yyyy-MM-dd hh:mm:ss] ");
    logData->append(QString("<font color=\"gray\">%1</font><font color=\"%2\">%3</font><br/>").arg(date).arg(msgColor).arg(msg));
    QString data = date + QLatin1String(msgtype) + msg + QLatin1String(EOL);
#if defined(Q_OS_WIN32)
    OutputDebugString(data.toLocal8Bit().data());
#else
#if defined(QT_DEBUG)
    fprintf(stderr, "%s", data.toLocal8Bit().constData());
#endif
#endif
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
void setMessageHandler(const QString &baseName)
{
#ifndef MISC_SMALL_VERSION
    // prepare filename part to remove from debug messages
    QString file = __FILE__; 
    QString key = "kdewin-installer";
    int i = file.indexOf(key);
    if (i != -1)
        fileOffset = i + key.size() + 1;

    QString fileName = Settings::instance().logFile(baseName);
    // make backup of last log file
    if (QFile::exists(fileName))
    {
        QFile::remove(fileName + ".old");
        QFile::rename(fileName, fileName + ".old");
    }
    logFile = new QFile(fileName);
    logFile->remove();
    logFile->open(QIODevice::WriteOnly);

#if QT_VERSION > 0x050000
    qInstallMessageHandler(myMessageOutput);
#else
    qInstallMsgHandler(myMessageOutput);
#endif
#endif
    logData = new QByteArray;
}

void closeMessageHandler()
{
#ifndef MISC_SMALL_VERSION
    if (logFile)
    {
        qDebug() << "closed log file";
        logFile->close();
        delete logData;
    }
#endif
}

QString logFileName()
{
    return logFile ? logFile->fileName() : QString();
}

QByteArray *log()
{
    static QByteArray empty;
    return logData ? logData : &empty;
}

QString logFileNameAsURL()
{
    return QUrl::fromLocalFile(logFileName()).toEncoded();
}

int qDebug_indention = 0;
char *qDebug_indentBuf = (char*)"                                                                                                               ";
 