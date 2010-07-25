/****************************************************************************
**
** Copyright (C) 2005-2010 Ralf Habacker. All rights reserved.
**
** This file is part of the KDE installer for windows
**
** This library is free software; you can redistribute it and/or
** modify it under the terms of the GNU Library General Public
** License version 2 as published by the Free Software Foundation.
**
** This library is distributed in the hope that it will be useful,
** but WITHOUT ANY WARRANTY; without even the implied warranty of
** MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU
** Library General Public License for more details.
**
** You should have received a copy of the GNU Library General Public License
** along with this library; see the file COPYING.LIB.  If not, write to
** the Free Software Foundation, Inc., 51 Franklin Street, Fifth Floor,
** Boston, MA 02110-1301, USA.
**
****************************************************************************/

#ifndef PROXYSETTINGS_H
#define PROXYSETTINGS_H

#include <QString>

/**
 holds system related proxy settings
*/
class ProxySettings
{
public:
    typedef enum {None = 0, InternetExplorer, FireFox, Environment, Manual, AutoDetect} ProxyMode;
    ProxySettings();

    /// read in proxy settings depending on mode and url based
    bool from(ProxyMode mode, const QString &url=QString());

    /// save settings
    bool save();

    /// convert to cleartext output
    QString toString();

    QString hostname;
    int port;
    QString user;
    QString password;
    ProxyMode mode;

protected:
#ifdef Q_WS_WIN
    bool fromInternetExplorer(const QString &url);
#endif
    bool fromFireFox(const QString &url);
    bool fromEnvironment(const QString &url);
    bool fromAutoDetect(const QString &url);

};

#endif // PROXYSETTINGS_H
