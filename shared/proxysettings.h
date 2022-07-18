/****************************************************************************
**
** Copyright (C) 2005-2010 Ralf Habacker. All rights reserved.
**
** This file is part of the KDE installer for windows
**
** SPDX-License-Identifier: LGPL-2.0-only
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
#ifdef Q_OS_WIN32
    bool fromInternetExplorer(const QString &url);
#endif
    bool fromFireFox(const QString &url);
    bool fromEnvironment(const QString &url);
    bool fromAutoDetect(const QString &url);

};

#endif // PROXYSETTINGS_H
