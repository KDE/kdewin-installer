/****************************************************************************
**
** Copyright (C) 2006-2009 Ralf Habacker. All rights reserved.
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

#ifndef HASH_H
#define HASH_H

class QFile;

#include <QByteArray>

class Hash {
    public:
        typedef enum { None, MD5, SHA1 } Type; 
        
        Hash();
        Hash(Type type);
        static Hash &instance();
        void setType(Type type) { m_type = type; }
        bool setType(const QString &type);
        Hash::Type type() { return m_type; }
        QByteArray hash(QFile &f);
        QByteArray hash(const QString &file);

        static bool isHash (const QByteArray &str);
        static Hash::Type isType(const QByteArray &str);
        
        static QByteArray md5(QFile &f);
        static QByteArray md5(const QString &f);
        static QByteArray sha1(QFile &f);
        static QByteArray sha1(const QString &f);
    protected: 
        Type m_type;
};

#endif
