/*
    This file is part of KDE.

    Copyright (c) 2009 Eckhart Wörner <ewoerner@kde.org>
    Copyright (c) 2011 Laszlo Papp <djszapi@archlinux.us>

    This library is free software; you can redistribute it and/or
    modify it under the terms of the GNU Lesser General Public
    License as published by the Free Software Fo1undation; either
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

#include "qtplatformdependent.h"

#include <QtCore/QUrl>
#include <QtCore/QStringList>
#include <QtCore/QDebug>

using namespace Attica;

// TODO actually save and restore providers!
QList<QUrl> Attica::QtPlatformDependent::getDefaultProviderFiles() const
{
    return QList<QUrl>();
}

void QtPlatformDependent::addDefaultProviderFile(const QUrl&)
{
    qDebug() << "attica-qt does not support default providers yet";
}

void QtPlatformDependent::removeDefaultProviderFile(const QUrl&)
{
}

void QtPlatformDependent::enableProvider(const QUrl& baseUrl, bool enabled) const
{
    qDebug() << "attica-qt does not support disabling of providers yet";
}

bool QtPlatformDependent::isEnabled(const QUrl& baseUrl) const
{
    return true;
}

QNetworkReply* QtPlatformDependent::post(const QNetworkRequest& request, const QByteArray& data)
{
    return m_qnam.post(request, data);
}


QNetworkReply* QtPlatformDependent::post(const QNetworkRequest& request, QIODevice* data)
{
    return m_qnam.post(request, data);
}

QNetworkReply* QtPlatformDependent::put(const QNetworkRequest& request, const QByteArray& data)
{
    return m_qnam.put(request, data);
}

QNetworkReply* QtPlatformDependent::put(const QNetworkRequest& request, QIODevice* data)
{
    return m_qnam.put(request, data);
}

QNetworkReply* QtPlatformDependent::get(const QNetworkRequest& request)
{
    return m_qnam.get(request);
}

QNetworkReply* QtPlatformDependent::deleteResource(const QNetworkRequest& request)
{
    return m_qnam.deleteResource(request);
}

bool QtPlatformDependent::hasCredentials(const QUrl& baseUrl) const
{
    return m_passwords.contains(baseUrl.toString());
}


bool QtPlatformDependent::saveCredentials(const QUrl& baseUrl, const QString& user, const QString& password)
{
    m_passwords[baseUrl.toString()] = QPair<QString, QString> (user, password);
    return true;
}


bool QtPlatformDependent::loadCredentials(const QUrl& baseUrl, QString& user, QString& password)
{
    if (!hasCredentials(baseUrl)) {
        return false;
    }
    QPair<QString, QString> userPass = m_passwords.value(baseUrl.toString());
    user = userPass.first;
    password = userPass.second;
    return true;
}

bool Attica::QtPlatformDependent::askForCredentials(const QUrl& baseUrl, QString& user, QString& password)
{
    return false;
}

QNetworkAccessManager* Attica::QtPlatformDependent::nam()
{
    return &m_qnam;
}
