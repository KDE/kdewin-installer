/****************************************************************************
**
** Copyright (C) 2008 Ralf Habacker. All rights reserved.
**
** This file is part of the KDE installer for windows
**
** SPDX-License-Identifier: LGPL-2.0-only
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

#if 0
// wrapper for original q... calls 
#define qDebug() _qDebug(__FILE__,__LINE__)
#define qInfo() _qInfo(__FILE__,__LINE__)
#define qWarning() _qWarning(__FILE__,__LINE__)
#define qCritical() _qCritical(__FILE__,__LINE__)
#define qFatal() _qFatal(__FILE__,__LINE__)
#endif

/// return stdout stream
QTextStream &qOut();
/// return stderr stream
QTextStream &qError();


/// set message handler
void setMessageHandler(const QString &baseName="kdewin-installer");

/// close message handler
void closeMessageHandler();

/// return log file name
QString logFileName();

/// return log file path and name as url
QString logFileNameAsURL();

/// return log data
QByteArray *log();

/**
macros for creating qDebug class support

QDebug operator<<(QDebug out, const XmlPart &c)
{
    QDEBUG_CLASS_START(out,XmlPart,c)
    QDEBUG_CLASS_MEMBER(name)
    QDEBUG_CLASS_LIST(XmlFiles,fileList)
    QDEBUG_CLASS_END()
}
*/

#ifndef NO_QDEBUG_CLASS_PRINT
extern char *qDebug_indentBuf;
extern int qDebug_indention;

inline char *qDebug_indent()
{
    return qDebug_indentBuf+strlen(qDebug_indentBuf)-(qDebug_indention-1)*2;
}

inline char *qDebug_indentIncrease()
{
    qDebug_indention++;
    return qDebug_indent();
}

inline char *qDebug_indentdecrease()
{
    qDebug_indention--;
    return qDebug_indent();
}
#define QDEBUG_CLASS_START(out,_class, object) QDebug &_out =out;  const _class &_c = object; _out << qDebug_indentIncrease() << #_class << " ("
#define QDEBUG_CLASS_MEMBER(member) << #member << ":" << _c.member
#define QDEBUG_CLASS_LIST(_class,member) ; _out << #member << ":\n"; foreach(_class *m, _c.member) _out << *m; 
#define QDEBUG_CLASS_END() ; _out << qDebug_indentdecrease() << "  )\n";  return _out;
#else
#define QDEBUG_CLASS_START(out,_class, object) return out;
#define QDEBUG_CLASS_MEMBER(member)
#define QDEBUG_CLASS_LIST(_class,member)
#define QDEBUG_CLASS_END()
#endif

#endif
