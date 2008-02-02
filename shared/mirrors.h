/****************************************************************************
**
** Copyright (C) 2006-2007 Ralf Habacker. All rights reserved.
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
#ifndef MIRRORS_H
#define MIRRORS_H

#include <QObject>
#include <QStringList>
#include <QUrl>
class QString;
class QStringList;
class QByteArray;
class QFile;

//http://webdev.cegit.de/snapshots/kde-windows;webdev.cegit.de;Europe;Germany,Essen

class MirrorType
{
    public:
        QUrl url;
        QString name;
        QString continent;
        QString country;

        friend QDebug &operator<<(QDebug &,const MirrorType &);
        QString toString() const
        {
            return continent + "," + country + " (" + name + ")";
        }
};

typedef QList<MirrorType> MirrorTypeList;

class Mirrors /* : public QObject */
{
    //Q_OBJECT
    public:
        enum Type { KDE = 1 ,Cygwin= 2 };

        Mirrors();
        bool fetch(Type type, QUrl url);
        MirrorTypeList &mirrors() { return m_mirrors; }
        static Mirrors &instance();

    protected:
        bool parse(const QString &fileName);
        bool parse(const QByteArray &data);
        bool parse(QIODevice *ioDev);

        Type m_type;
        QList<MirrorType> m_mirrors;
};

QDebug &operator<<(QDebug &,const MirrorTypeList &);

#endif
