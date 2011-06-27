#ifndef PUBLISHERFIELD_H
#define PUBLISHERFIELD_H

#include "buildservice.h"
#include "atticaclient_export.h"

namespace Attica {

class ATTICA_EXPORT PublisherField
{
public:
    typedef QList<PublisherField> List;
    class Parser;

    PublisherField();
    PublisherField(const PublisherField& other);
    PublisherField& operator=(const PublisherField& other);
    ~PublisherField();

    void setName(const QString& value);
    QString name() const;

    void setType(const QString& value);
    QString type() const;

    void setData(const QString& value);
    QString data() const;

    bool isValid() const;

private:
  class Private;
  QSharedDataPointer<Private> d;
};

} // namespace Attica

#endif // PUBLISHERFIELD_H
