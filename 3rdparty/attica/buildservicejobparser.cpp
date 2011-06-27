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

#include "buildservicejobparser.h"
#include <qdebug.h>

using namespace Attica;

BuildServiceJob BuildServiceJob::Parser::parseXml(QXmlStreamReader& xml)
{
    BuildServiceJob buildservicejob;

    // For specs about the XML provided, see here:
    // http://www.freedesktop.org/wiki/Specifications/open-collaboration-services-draft#BuildServiceJobs
    while (!xml.atEnd()) {
        //qDebug() << "XML returned:" << xml.text().toString();
        xml.readNext();

        if (xml.isStartElement()) {
            if (xml.name() == "id") { // FIXME: server should give "id" here ...
                buildservicejob.setId(xml.readElementText());
            } else if (xml.name() == "project") {
                buildservicejob.setProjectId(xml.readElementText());
            } else if (xml.name() == "buildservice") {
                buildservicejob.setBuildServiceId(xml.readElementText());
            } else if (xml.name() == "target") {
                buildservicejob.setTarget(xml.readElementText());
            } else if (xml.name() == "name") {
                buildservicejob.setName(xml.readElementText());
            } else if (xml.name() == "status") {
                int status = xml.readElementText().toInt();
                buildservicejob.setStatus(status);
            } else if (xml.name() == "progress") {
                qreal progress = (qreal)(xml.readElementText().toFloat());
                buildservicejob.setProgress(progress);
            } else if (xml.name() == "message") {
                buildservicejob.setMessage(xml.readElementText());
            } else if (xml.name() == "url") {
                buildservicejob.setUrl(xml.readElementText());
            }
        } else if (xml.isEndElement() && (xml.name() == "buildjob" || xml.name() == "user")) {
            break;
        }
    }
    return buildservicejob;
}


QStringList BuildServiceJob::Parser::xmlElement() const {
    return QStringList(QLatin1String("buildjob")) << QLatin1String("user");
}
