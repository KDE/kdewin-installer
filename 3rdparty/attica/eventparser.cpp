/*
    This file is part of KDE.

    Copyright (c) 2009 Eckhart Wörner <ewoerner@kde.org>

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

#include "eventparser.h"

#include <QtCore/QRegExp>


using namespace Attica;

Event Event::Parser::parseXml(QXmlStreamReader& xml)
{
    Event event;

    while (!xml.atEnd()) {
        xml.readNext();

        if (xml.isStartElement()) {
            if (xml.name() == "id") {
                event.setId(xml.readElementText());
            } else if (xml.name() == "name") {
                event.setName(xml.readElementText());
            } else if (xml.name() == "description") {
                event.setDescription(xml.readElementText());
            } else if (xml.name() == "user") {
                event.setUser(xml.readElementText());
            } else if (xml.name() == "startdate") {
                QString date = xml.readElementText().remove(QRegExp(QLatin1String( "\\+.*$" )));
                event.setStartDate(QDate::fromString(date, Qt::ISODate));
            } else if (xml.name() == "enddate") {
                QString date = xml.readElementText().remove(QRegExp(QLatin1String( "\\+.*$" )));
                event.setEndDate(QDate::fromString(date, Qt::ISODate));
            } else if (xml.name() == "latitude") {
                event.setLatitude(xml.readElementText().toFloat());
            } else if (xml.name() == "longitude") {
                event.setLongitude(xml.readElementText().toFloat());
            } else if (xml.name() == "homepage") {
                event.setHomepage(xml.readElementText());
            } else if (xml.name() == "country") {
                event.setCountry(xml.readElementText());
            } else if (xml.name() == "city") {
                event.setCity(xml.readElementText());
            } else {
                event.addExtendedAttribute(xml.name().toString(), xml.readElementText());
            }
        } else if (xml.isEndElement() && xml.name() == "event") {
            break;
        }
    }

    return event;
}


QStringList Event::Parser::xmlElement() const {
    return QStringList(QLatin1String( "event" ));
}
