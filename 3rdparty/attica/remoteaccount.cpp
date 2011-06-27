/*
    This file is part of KDE.

    Copyright 2010 Sebastian Kügler <sebas@kde.org>

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

#include "remoteaccount.h"


using namespace Attica;

class RemoteAccount::Private : public QSharedData {
    public:
        QString id;
        QString type;
        QString remoteServiceId;
        QString data;
        QString login;
        QString password;

        Private()
        {
        }
};


RemoteAccount::RemoteAccount()
  : d(new Private)
{
}

RemoteAccount::RemoteAccount(const RemoteAccount& other)
    : d(other.d)
{
}

RemoteAccount& RemoteAccount::operator=(const Attica::RemoteAccount & other)
{
    d = other.d;
    return *this;
}

RemoteAccount::~RemoteAccount()
{
}

void RemoteAccount::setId( const QString &u )
{
    d->id = u;
}

QString RemoteAccount::id() const
{
    return d->id;
}

void RemoteAccount::setType( const QString &arg )
{
    d->type = arg;
}

QString RemoteAccount::type() const
{
    return d->type;
}

void RemoteAccount::setRemoteServiceId( const QString &arg )
{
    d->remoteServiceId = arg;
}

QString RemoteAccount::remoteServiceId() const
{
    return d->remoteServiceId;
}

void RemoteAccount::setData( const QString &arg )
{
    d->data = arg;
}

QString RemoteAccount::data() const
{
    return d->data;
}

void RemoteAccount::setLogin( const QString &arg )
{
    d->login = arg;
}

QString RemoteAccount::login() const
{
    return d->login;
}

void RemoteAccount::setPassword( const QString &arg )
{
    d->password = arg;
}

QString RemoteAccount::password() const
{
    return d->password;
}


bool RemoteAccount::isValid() const
{
    return !(d->id.isEmpty());
}
