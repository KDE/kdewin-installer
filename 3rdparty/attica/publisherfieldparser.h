#ifndef PUBLISHERFIELDPARSER_H
#define PUBLISHERFIELDPARSER_H

#include "publisherfield.h"
#include "parser.h"

namespace Attica
{
class PublisherField::Parser : public Attica::Parser<PublisherField>
{
private:
    PublisherField parseXml(QXmlStreamReader& xml);
    QStringList xmlElement() const;
};

} // namespace Attica

#endif // PUBLISHERFIELDPARSER_H
