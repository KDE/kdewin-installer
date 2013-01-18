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

#include "proxysettings.h"
#include "settings.h"
#include "downloader.h"

static QString testUrl = "http://www.winkde.org";

ProxySettings::ProxySettings() : port(0)
{
}

bool ProxySettings::from(ProxyMode _mode, const QString &url)
{
    // FIXME: add support for different ftp proxy settings
    mode = _mode;
    switch(_mode) {
#ifdef Q_OS_WIN32
        case ProxySettings::InternetExplorer:
            return fromInternetExplorer(url);
#endif
        case ProxySettings::FireFox:
            return fromFireFox(url);

        case ProxySettings::Manual:
            Settings::instance().proxy(*this);
            return true;

#ifndef Q_OS_WIN32
        case ProxySettings::Environment:
            return fromEnvironment(url);
#endif
        case ProxySettings::AutoDetect:
            return fromAutoDetect(url);

        case ProxySettings::None:
        default:
            hostname = QString();
            port = 0;
            user = QString();
            password = QString();
            break;
    }
    return false;
}

bool ProxySettings::save()
{
    Settings &s = Settings::instance();
    s.setProxyMode(mode);
    if (mode == ProxySettings::Manual)
        s.setProxy(*this);
    return true;
}

#ifdef Q_OS_WIN
bool ProxySettings::fromInternetExplorer(const QString &url)
{
    // @TODO how to retrieve user and passwort
    bool ok;
    quint32 enable = getWin32RegistryValue(hKEY_CURRENT_USER,"Software\\Microsoft\\Windows\\CurrentVersion\\Internet Settings","ProxyEnable",&ok).toUInt();
    if (!ok)
        return false;

    if (enable == 0)
        return false;

    const QString proxyServer = getWin32RegistryValue(hKEY_CURRENT_USER,"Software\\Microsoft\\Windows\\CurrentVersion\\Internet Settings","ProxyServer",&ok).toString();
    if (!ok)
        return false;

    const QStringList parts = proxyServer.split(':');
    if(parts.count() != 2)
        return false;

    hostname = parts[0];
    port = parts[1].toInt();
    user = QString();
    password = QString();
    mode = InternetExplorer;
    return true;
}
#endif

bool ProxySettings::fromEnvironment(const QString &_url)
{
    QUrl url(qgetenv("http_proxy"));

    if(url.isValid())
    {
        hostname = url.host();
        port = url.port(8080);
        user = url.userName();
        password = url.password();
        mode = Environment;
        return true;
    }
    return false;
}

bool ProxySettings::fromFireFox(const QString &url)
{
    static QHash<QString,QString> prefs;
    static bool prefsRead = false;

    if (!prefsRead)
    {
        QString mozillaProfileDir = qgetenv("APPDATA");///lb4kyocy.default";
        QDir d(mozillaProfileDir + "/Mozilla/Firefox/Profiles");

        d.setFilter(QDir::NoDotAndDotDot | QDir::AllDirs);
        d.setNameFilters(QStringList("*.default"));
        d.setSorting(QDir::Name);

        QFileInfoList list = d.entryInfoList();
        if(!list.count())
            return false;
        const QFileInfo &fi = list[0];
        QString prefsFile = d.absolutePath()+"/"+fi.fileName() + "/prefs.js";
        QFile f(prefsFile);
        f.open(QIODevice::ReadOnly | QIODevice::Text);
        QTextStream in(&f);
        while (!in.atEnd())
        {
            QString line = in.readLine();
            if (line.startsWith("user_pref("))
            {
                const QString data = line.mid(10,line.length()-11-1);
                const QStringList attr = data.split(',');
                const QString key = attr[0].mid(1,attr[0].length()-2);
                const QString value = attr[1].mid(1,attr[1].length()-1).remove('\"');
                prefs[key] = value;
            }
        }
        prefsRead = true;
    }
    if (prefs.contains("network.m_type"))
    {
        int mode = prefs["network.m_type"].toInt();
        // 1 manual proxy
        // 2 use autoconfig url -> network.m_autoconfig_url
        // 4 automatic detection, how to do ?
        if (mode == 1)
        {
            if (url.startsWith("http") ||
                       prefs.contains("network.m_share_proxy_settings")
                    && prefs["network.m_share_proxy_settings"] == "true")
            {
                hostname = prefs["network.m_http"];
                port = prefs["network.m_http_port"].toInt();
                /// @TODO: get username and passwort from firefox settings
                user = QString();
                password = QString();
            }
            else
            {
                hostname = prefs["network.m_ftp"];
                port = prefs["network.m_ftp_port"].toInt();
                /// @TODO: get username and passwort from firefox settings
                user = QString();
                password = QString();

            }
            mode = FireFox;
            return true;
        }
    }
    return false;
}


bool ProxySettings::fromAutoDetect(const QString &url)
{
    Settings &s = Settings::instance();
    QByteArray data;

    if (from(InternetExplorer,url) && !hostname.isEmpty())
    {
        s.setProxyMode(mode);
        if (Downloader::instance()->fetch(testUrl,data))
        {
            return true;
        }
    }
    if (from(FireFox,url) && !hostname.isEmpty())
    {
        s.setProxyMode(mode);
        if (Downloader::instance()->fetch(testUrl,data))
        {
            return true;
        }
    }
    if (from(Environment,url) && !hostname.isEmpty())
    {
        s.setProxyMode(mode);
        if (Downloader::instance()->fetch(testUrl,data))
        {
            return true;
        }
    }
    mode = ProxySettings::None;
    s.setProxyMode(mode);
    if (Downloader::instance()->fetch(testUrl,data))
    {
        return true;
    }
    return false;
}


QString ProxySettings::toString()
{
    switch(mode)
    {
        case ProxySettings::InternetExplorer:
            return "using IE proxy " + hostname;
        case ProxySettings::FireFox:
            return "using Firefox proxy " + hostname;
        case ProxySettings::Manual:
            return "using manual set proxy " + hostname;

        case ProxySettings::Environment:
            return "using proxy from environment " + hostname;

        case ProxySettings::None:
        default:
            return "using no proxy";
    }
}
