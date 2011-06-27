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


#include "distributionparser.h"

using namespace Attica;

QStringList Distribution::Parser::xmlElement() const
{
    return QStringList(QLatin1String( "distribution" ));
}

Distribution Distribution::Parser::parseXml(QXmlStreamReader& xml)
{
    Distribution item;

    while ( !xml.atEnd() ) {
        xml.readNext();
        if ( xml.isStartElement() ) {
            if ( xml.name() == "id" ) {
                item.setId( xml.readElementText().toInt() );
            } else if ( xml.name() == "name" ) {
                item.setName( xml.readElementText() );
            }
        }
        if (xml.isEndElement() && xml.name() == "distribution") {
            break;
        }
    }
    return item;
}
