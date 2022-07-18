/****************************************************************************
**
** Copyright (C) 2006-2007 Ralf Habacker. All rights reserved.
**
** This file is part of the KDE installer for windows
**
** SPDX-License-Identifier: LGPL-2.0-only
**
****************************************************************************/

#ifndef SETTINGS_H
#define SETTINGS_H

#include "misc.h"
#include "typehelper.h"
#include "proxysettings.h"

#include <QDir>
#include <QObject>
#include <QSettings>
#include <QStringList>
#include <QUrl>

/**
 holds global options, which are stored in a local user specific config file
*/
class Settings : public QObject
{
    Q_OBJECT
public:
    Settings();
    ~Settings();

    // Place where the packages should be installed to
    QString installDir() const;
    void setInstallDir(const QString &dir, bool persistent=true);

    // Place where the packages should be downloaded to
    QString downloadDir() const;
    void setDownloadDir(const QString &dir, bool persistent=true);

    // download mirror base url 
    QString mirror() const;
    void setMirror(const QString &mirror);

    // url of last used mirror url 
    QUrl mirrorWithReleasePath() const { return m_settings->value("MirrorWithReleasePath", "").toString(); }
    void setMirrorWithReleasePath(const QUrl &url) { m_settings->setValue("MirrorWithReleasePath", url.toString()); sync(); } 

    // additional download mirrors
    QStringList localMirrors() const;
    void addLocalMirror(const QString &locMirror);
    void setLocalMirrors(const QStringList& locMirrors);

    // show title page
    bool showTitlePage() const;
    void setShowTitlePage(bool bShow);

    bool isSkipBasicSettings() const { return m_settings->value("SkipBasicSettings", false).toBool(); }
    void setSkipBasicSettings(bool mode) { m_settings->setValue("SkipBasicSettings", mode); sync(); }

    // create start menu entries fomr .desktop files
    bool createStartMenuEntries();
    void setCreateStartMenuEntries(bool bCreate);

    // true on first run
    bool isFirstRun() const { return m_settingsMain->value("FirstRun", true).toBool(); }
    void setFirstRun(bool bFirstRun) { m_settingsMain->setValue("FirstRun", bFirstRun); sync(); }

    // package Manager mode 
    bool isPackageManagerMode() const { return m_settings->value("PackageManagerMode", m_settingsMain->value("PackageManagerMode",false).toBool()).toBool(); }
    void setPackageManagerMode(bool mode) { m_settings->setValue("PackageManagerMode", mode); sync(); }

    // property of the packagerManagerMode
    const bool isDeveloperInstallMode() { return false; }
    void setDeveloperInstallMode() {  }

    /// specify the source mode of proxy settings
    ProxySettings::ProxyMode proxyMode() const { return (ProxySettings::ProxyMode)m_settingsMain->value("proxyMode",0).toInt(); }
    void setProxyMode(ProxySettings::ProxyMode mode) { m_settingsMain->setValue("proxyMode", mode); }

    /// return proxy settings
    bool proxy(ProxySettings &proxy)
    {
        proxy.hostname = proxyHost();
        proxy.port = proxyPort();
        proxy.user = proxyUser();
        proxy.password = proxyPassword();
        return true;
    }

    /// save proxy settings
    void setProxy(const ProxySettings &proxy)
    {
        m_settingsMain->setValue("proxyHost",proxy.hostname);
        m_settingsMain->setValue("proxyPort",proxy.port);
        m_settingsMain->setValue("proxyUser",proxy.user);
        m_settingsMain->setValue("proxyPassword",proxy.password);
        sync();
    }

    bool installDetails() const { return m_settings->value("installDetails", false).toBool(); }
    void setInstallDetails(bool state) { m_settings->setValue("installDetails", state); sync(); }
    
    bool autoNextStep() const { return m_settingsMain->value("autoNextStep", true).toBool(); }
    void setAutoNextStep(bool state) { m_settingsMain->setValue("autoNextStep", state); sync(); }

    bool installDebugPackages() const { return m_settings->value("installDebugPackages", false).toBool(); }
    void setInstallDebugPackages(bool state) { m_settings->setValue("installDebugPackages", state); sync(); }

    CompilerTypes::Type compilerType() const { return (CompilerTypes::Type) (m_settings->value("compilerType",m_settingsMain->value("compilerType",0).toInt()).toInt()); }
    void setCompilerType(CompilerTypes::Type type);

    QString logFile(const QString &baseName="kdewin-installer") { return downloadDir() + QDir::separator() + baseName + ".log"; }
    
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
    void settingsChanged();
    void compilerTypeChanged();

protected:
    QString proxyHost() const { return m_settingsMain->value("proxyHost").toString(); }
    int proxyPort() const { return m_settingsMain->value("proxyPort").toInt(); }
    QString proxyUser() const { return m_settingsMain->value("proxyUser").toString(); }
    QString proxyPassword() const { return m_settingsMain->value("proxyPassword").toString(); }

private:
    // stored in user profile
    QSettings *m_settingsMain;
    // stored in installroot 
    QSettings *m_settings;

    QString m_installDir;
    QString m_downloadDir;

    QString debug(void) { return m_settings->value("debug", "").toString(); }
    friend QDebug operator<<(QDebug, Settings &);
};


#include <QDebug>

inline QDebug operator<<(QDebug out, ProxySettings &c)
{
    out << "host:" << c.hostname << "port:" << c.port 
        << "user:" << c.user << "password:" << (c.password.isEmpty() ? "<empty>" : "****");
    return out;
}


#endif
