/****************************************************************************
**
** Copyright (C) 2006-2007 Ralf Habacker. All rights reserved.
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

#ifndef SETTINGS_H
#define SETTINGS_H

#include <QObject>
#include <QSettings>
#include <QNetworkProxy>

/**
 holds global options, which are stored in a local user specific config file
*/

class Settings : public QObject
{
    Q_OBJECT
public:
  struct proxySettings {
    QString hostname;
    int port;
    QString user;
    QString password;
  };
public:
    Settings();
    ~Settings();

    // Place where the packages should be installed to
    QString installDir();
    void setInstallDir(const QString &dir);

    // Place where the packages should be downloaded to
    QString downloadDir();
    void setDownloadDir(const QString &dir);

    // download mirror settings  
    QString mirror();
    void setMirror(const QString &mirror);

    // additional download mirrors
    QStringList localMirrors();
    void addLocalMirror(const QString &locMirror);
    void setLocalMirrors(const QStringList& locMirrors);

    // show title page
    bool showTitlePage();
    void setShowTitlePage(bool bShow);

    bool isSkipBasicSettings() { return m_settings->value("SkipBasicSettings", false).toBool(); }
    void setSkipBasicSettings(bool mode) { m_settings->setValue("SkipBasicSettings", mode); sync(); }

    // create start menu entries fomr .desktop files
    bool createStartMenuEntries();
    void setCreateStartMenuEntries(bool bCreate);

    // true on first run
    bool isFirstRun() { return m_settingsMain->value("FirstRun", true).toBool(); }
    void setFirstRun(bool bFirstRun) { m_settingsMain->setValue("FirstRun", bFirstRun); sync(); }

    // package Manager mode 
    bool isPackageManagerMode() { return m_settings->value("PackageManagerMode", true).toBool(); }
    void setPackageManagerMode(bool mode) { m_settings->setValue("PackageManagerMode", mode); sync(); }

    bool isDeveloperMode() { return m_settings->value("DeveloperMode", m_settingsMain->value("DeveloperMode",true).toBool()).toBool(); }
    void setDeveloperMode(bool bMode) { m_settings->setValue("DeveloperMode", bMode); sync(); }

    enum ProxyMode { None = 0, InternetExplorer, FireFox, Environment, Manual };
    ProxyMode proxyMode() { return (ProxyMode)m_settings->value("proxyMode",0).toInt(); }
    void setProxyMode(ProxyMode mode) 
    {
        m_settingsMain->setValue("proxyMode", mode);
    }
    const QString proxyHost() { return m_settingsMain->value("proxyHost").toString(); }
    int proxyPort() { return m_settingsMain->value("proxyPort").toInt(); }
    const QString proxyUser() { return m_settingsMain->value("proxyUser").toString(); }
    const QString proxyPassword() { return m_settingsMain->value("proxyPassword").toString(); }

    bool proxy(const QString &url, proxySettings &proxy);
    void setProxy(const QString &host, const QString &port) 
    {
        m_settingsMain->setValue("proxyHost",host);
        m_settingsMain->setValue("proxyPort",port);
        sync();
    }
    void setProxy(const proxySettings &proxy)
    {
        m_settingsMain->setValue("proxyHost",proxy.hostname);
        m_settingsMain->setValue("proxyPort",proxy.port);
        m_settingsMain->setValue("proxyUser",proxy.user);
        m_settingsMain->setValue("proxyPassword",proxy.password);
        sync();
    }

    bool installDetails() { return m_settings->value("installDetails", false).toBool(); }
    void setInstallDetails(bool state) { m_settings->setValue("installDetails", state); sync(); }
    
    bool autoNextStep() { return m_settingsMain->value("autoNextStep", true).toBool(); }
    void setAutoNextStep(bool state) { m_settingsMain->setValue("autoNextStep", state); sync(); }

    typedef enum { unspecified=0, MinGW=1, MSVC=2 } CompilerType;
    CompilerType compilerType() { return (CompilerType) (m_settings->value("compilerType",m_settingsMain->value("compilerType",0).toInt()).toInt()); }
    void setCompilerType(CompilerType type);

    // QSettings compatible interface
    void beginGroup(const QString &prefix) { m_settings->beginGroup(prefix); }
    void endGroup() { m_settings->endGroup(); }
    QVariant value(const QString &key, const QVariant &defaultValue = QVariant()) const { return m_settings->value(key,defaultValue); }
    void setValue(const QString &key, const QVariant &value) { m_settings->setValue(key,value); }
    // sync database 
    void sync();

    // static methods
    static Settings &instance();
    static bool hasDebug(const QString area="");

Q_SIGNALS:
    void installDirChanged(const QString &newDir);
    void downloadDirChanged(const QString &newDir);
    void mirrorChanged(const QString &mirror);
    void settingsChanged();
    void compilerTypeChanged();

protected:
private:
    // stored in user profile
    QSettings *m_settingsMain;
    // stored in installroot 
    QSettings *m_settings;
    proxySettings m_proxy;
#ifdef Q_WS_WIN
    bool getIEProxySettings(const QString &url, proxySettings &proxy);
#endif
    bool getFireFoxProxySettings(const QString &url, proxySettings &proxy);
    bool getEnvironmentProxySettings(const QString &url, proxySettings &proxy);
    QString debug(void) { return m_settings->value("debug", "").toString(); }
    friend QDebug &operator<<(QDebug &, Settings &);
};


#include <QDebug>

inline QDebug &operator<<(QDebug &out, Settings::proxySettings &c)
{
    out << "host:" << c.hostname << "port:" << c.port 
        << "user:" << c.user << "password:" << (c.password.isEmpty() ? "<empty>" : "****");
    return out;
}


#endif
