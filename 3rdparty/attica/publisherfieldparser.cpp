#include "publisherfieldparser.h"

using namespace Attica;

PublisherField PublisherField::Parser::parseXml(QXmlStreamReader& xml)
{
    PublisherField fld;
    while (!xml.atEnd()) {
        xml.readNextStartElement();
        if (xml.isStartElement()) {
            if (xml.name() == "fieldtype") {
                fld.setType(xml.readElementText());
            } else if (xml.name() == "name") {
                fld.setName(xml.readElementText());
            } else if (xml.name() == "data") {
                fld.setData(xml.readElementText());
            }
        } else if (xml.isEndElement() && (xml.name() == "field")) {
            xml.readNext();
            break;
        }
    }
    return fld;
}

QStringList PublisherField::Parser::xmlElement() const {
    return QStringList(QLatin1String("field"));
}
