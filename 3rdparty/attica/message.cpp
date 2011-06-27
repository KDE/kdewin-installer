/*
    This file is part of KDE.

    Copyright (c) 2008 Cornelius Schumacher <schumacher@kde.org>

    This library is free software; you can redistribute it and/or
    modify it under the terms of the GNU Lesser General Public
    License as published by the Free Software Foundation; either
    version 2.1 of the License, or (at your option) version 3, or any
    later version accepted by the membership of KDE e.V. (or its
    successor approved by the membership of KDE e.V.), which shall
    act as a proxy defined in Section 6 of version 3 of the license.

    This library is distributed in the hope that it will be useful,
    but WITHOUT ANY WARRANTY; without even the implied warranty of
    MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU
    Lesser General Public License for more details.

    You should have received a copy of the GNU Lesser General Public
    License along with this library.  If not, see <http://www.gnu.org/licenses/>.

*/

#include "message.h"

using namespace Attica;

class Message::Private : public QSharedData {
    public:
        QString m_id;
        QString m_from;
        QString m_to;
        QDateTime m_sent;
        Status m_status;
        QString m_subject;
        QString m_body;

        Private()
            : m_status(Unread)
        {
        }
};


Message::Message()
  : d(new Private)
{
}

Message::Message(const Message& other)
    : d(other.d)
{
}

Message& Message::operator=(const Attica::Message & other)
{
    d = other.d;
    return *this;
}

Message::~Message()
{
}


void Message::setId( const QString &u )
{
  d->m_id = u;
}

QString Message::id() const
{
  return d->m_id;
}

void Message::setFrom( const QString &n )
{
  d->m_from = n;
}

QString Message::from() const
{
  return d->m_from;
}
  
void Message::setTo( const QString &n )
{
  d->m_to = n;
}

QString Message::to() const
{
  return d->m_to;
}
    
void Message::setSent( const QDateTime &date )
{
  d->m_sent = date;
}

QDateTime Message::sent() const
{
  return d->m_sent;
}

void Message::setStatus( Message::Status s )
{
  d->m_status = s;
}

Message::Status Message::status() const
{
  return d->m_status;
}

void Message::setSubject( const QString &subject )
{
  d->m_subject = subject;
}

QString Message::subject() const
{
  return d->m_subject;
}

void Message::setBody( const QString &body )
{
  d->m_body = body;
}

QString Message::body() const
{
  return d->m_body;
}


bool Message::isValid() const {
  return !(d->m_id.isEmpty());
}
