/*
    This file is part of KDE.

    Copyright (c) 2010 Intel Corporation
    Author: Mateu Batle Sastre <mbatle@collabora.co.uk>

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


#include "homepagetype.h"

using namespace Attica;

class HomePageType::Private :public QSharedData
{
public:
    int id;
    QString name;
    
    Private() :id(-1)
    {}
};

HomePageType::HomePageType()
    :d(new Private)
{
}

HomePageType::HomePageType(const Attica::HomePageType& other)
    :d(other.d)
{
}

HomePageType& HomePageType::operator=(const Attica::HomePageType& other)
{
    d = other.d;
    return *this;
}

HomePageType::~HomePageType()
{}

uint HomePageType::id() const
{
    return d->id;
}

void HomePageType::setId(uint id)
{
    d->id = id;
}

QString HomePageType::name() const
{
    return d->name;
}

void HomePageType::setName(const QString& name)
{
    d->name = name;
}

