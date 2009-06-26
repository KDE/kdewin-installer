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
#define QDEBUG_CLASS_START(out,_class, object) QDebug &_out =out;  const _class &_c = object; _out << qDebug_indentIncrease() << #_class##" ("
#define QDEBUG_CLASS_MEMBER(member) << #member##":" << _c.member
#define QDEBUG_CLASS_LIST(_class,member) ; _out << #member##":\n"; foreach(_class *m, _c.member) _out << *m; 
#define QDEBUG_CLASS_END() ; _out << qDebug_indentdecrease() << "  )\n";  return _out;
#else
#define QDEBUG_CLASS_START(out,_class, object) return out;
#define QDEBUG_CLASS_MEMBER(member)
#define QDEBUG_CLASS_LIST(_class,member)
#define QDEBUG_CLASS_END()
#endif

#endif
