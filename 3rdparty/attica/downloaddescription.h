/*
This file is part of KDE.

Copyright (c) 2009 Frederik Gladhorn <gladhorn@kde.org>

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

#ifndef DOWNLOADDESCRIPTION_H
#define DOWNLOADDESCRIPTION_H

#include <QtCore/QSharedData>
#include <QtCore/QString>

#include "atticaclient_export.h"


namespace Attica {

class ATTICA_EXPORT DownloadDescription
{
public:

    enum Type {
        FileDownload = 0,
        LinkDownload,
        PackageDownload
    };

    DownloadDescription();
    DownloadDescription(const DownloadDescription& other);
    
    DownloadDescription& operator=(const DownloadDescription& other);
    ~DownloadDescription();

    /**
      The id of the description - as one Content can have multiple download descriptions associated.
      This will simply be 1, 2, ...
      */
    int id();
    Attica::DownloadDescription::Type type();
    Q_DECL_DEPRECATED bool isDownloadtypLink();
    bool hasPrice();
    QString category();
    QString name();
    QString link();
    QString distributionType();
    QString priceReason();
    QString priceAmount();
    uint size();
    QString gpgFingerprint();
    QString gpgSignature();
    QString packageName();
    QString repository();

    void setId(int id);
    void setType(Attica::DownloadDescription::Type type);
    Q_DECL_DEPRECATED void setDownloadtypLink(bool isLink);
    void setHasPrice(bool hasPrice);
    void setCategory(const QString& category);
    void setName(const QString& name);
    void setLink(const QString& link);
    void setDistributionType(const QString& distributionType);
    void setPriceReason(const QString& priceReason);
    void setPriceAmount(const QString& priceAmount);
    void setSize(uint size);
    void setGpgFingerprint(const QString& fingerprint);
    void setGpgSignature(const QString& signature);
    void setPackageName(const QString& packageName);
    void setRepository(const QString& repository);

private:
    class Private;
    QSharedDataPointer<Private> d;
};

}

#endif // DOWNLOADDESCRIPTION_H
