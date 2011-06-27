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

#include "buildservicejob.h"


using namespace Attica;

class BuildServiceJob::Private : public QSharedData {
    public:
        QString id;
        QString name;
        int status;
        qreal progress;
        QString projectId;
        QString target;
        QString buildServiceId;
        QString url;
        QString message;

        Private()
        {
        }
};


BuildServiceJob::BuildServiceJob()
  : d(new Private)
{
}

BuildServiceJob::BuildServiceJob(const BuildServiceJob& other)
    : d(other.d)
{
}

BuildServiceJob& BuildServiceJob::operator=(const Attica::BuildServiceJob & other)
{
    d = other.d;
    return *this;
}

BuildServiceJob::~BuildServiceJob()
{
}

void BuildServiceJob::setId( const QString &u )
{
    d->id = u;
}

QString BuildServiceJob::id() const
{
    return d->id;
}

void BuildServiceJob::setName( const QString &u )
{
    d->name = u;
}

QString BuildServiceJob::name() const
{
    return d->name;
}

void BuildServiceJob::setProgress( const qreal p )
{
    d->progress = p;
}

qreal BuildServiceJob::progress() const
{
    return d->progress;
}

void BuildServiceJob::setStatus( const int status )
{
    d->status = status;
}

bool BuildServiceJob::isRunning() const
{
    return d->status == 1;
}

bool BuildServiceJob::isCompleted() const
{
    return d->status == 2;
}

bool BuildServiceJob::isFailed() const
{
    return d->status == 3;
}

void BuildServiceJob::setUrl( const QString &u )
{
    d->url = u;
}

QString BuildServiceJob::url() const
{
    return d->url;
}

void BuildServiceJob::setMessage( const QString &u )
{
    d->message = u;
}

QString BuildServiceJob::message() const
{
    return d->message;
}

void BuildServiceJob::setProjectId( const QString &u )
{
    d->projectId = u;
}

QString BuildServiceJob::projectId() const
{
    return d->projectId;
}

void BuildServiceJob::setTarget( const QString &u )
{
    d->target = u;
}

QString BuildServiceJob::target() const
{
    return d->target;
}

void BuildServiceJob::setBuildServiceId( const QString &u )
{
    d->buildServiceId = u;
}

QString BuildServiceJob::buildServiceId() const
{
    return d->buildServiceId;
}

bool BuildServiceJob::isValid() const
{
    return !(d->id.isEmpty());
}
