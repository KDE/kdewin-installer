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

/**
 holds global options, which are stored in a local user specific config file
*/

class Settings : public QObject
{
    Q_OBJECT
public:
    Settings();

    // Place where the packages should be installed to
    QString installDir();
    void setInstallDir(const QString &dir);

    // Place where the packages should be downloaded to
    QString downloadDir();
    void setDownloadDir(const QString &dir);

    // download mirror settings  
    QString mirror();
    void setMirror(QString &mirror);
 
    // show title page
    bool showTitlePage();
    void setShowTitlePage(bool bShow);

    // create start menu entries fomr .desktop files
    bool createStartMenuEntries();
    void setCreateStartMenuEntries(bool bCreate);

    // true on first run
    bool isFirstRun() { return m_settings.value("FirstRun", true).toBool(); }
    void setFirstRun(bool bFirstRun) { m_settings.setValue("FirstRun", bFirstRun); sync(); }

    // use nested download directory tree 
    bool nestedDownloadTree() { return m_settings.value("nestedDownloadTree", false).toBool(); }
    void setNestedDownloadTree(bool value) { m_settings.setValue("nestedDownloadTree", value); sync(); }

    enum ProxyMode { None = 0, InternetExplorer, FireFox, Manual };
    ProxyMode proxyMode() { return (ProxyMode)m_settings.value("proxyMode",0).toInt(); }
    void setProxyMode(ProxyMode mode) 
    {
        m_settings.setValue("proxyMode", mode);
    }
    const QString proxyHost() { return m_settings.value("proxyHost").toString(); }
    int proxyPort() { return m_settings.value("proxyPort").toInt(); }
    void setProxy(const QString &host, const QString &port) 
    {
        m_settings.setValue("proxyHost",host);
        m_settings.setValue("proxyPort",port);
        sync();
    }
    bool getProxySettings(const QString &url, QString &host, int &port);

    bool installDetails() { return m_settings.value("installDetails", true).toBool(); }
    void setInstallDetails(bool state) { m_settings.setValue("installDetails", state); sync(); }
    
    bool autoNextStep() { return m_settings.value("autoNextStep", true).toBool(); }
    void setAutoNextStep(bool state) { m_settings.setValue("autoNextStep", state); sync(); }

    typedef enum { unspecified=0, MinGW=1, MSVC=2 } CompilerType;
    CompilerType compilerType() { return (CompilerType) (m_settings.value("compilerType",0).toInt()); }
    void setCompilerType(CompilerType type);

    // QSettings compatible interface
    void beginGroup(const QString &prefix) { m_settings.beginGroup(prefix); }
    void endGroup() { m_settings.endGroup(); }
    QVariant value(const QString &key, const QVariant &defaultValue = QVariant()) const { return m_settings.value(key,defaultValue); }
    void setValue(const QString &key, const QVariant &value) { m_settings.setValue(key,value); }
    // sync database 
    void sync();

    // static methods
    static Settings &getInstance();
    static bool hasDebug(const QString area="");

Q_SIGNALS:
    void installDirChanged(const QString &newDir);
    void downloadDirChanged(const QString &newDir);
    void mirrorChanged(const QString &mirror);
    void settingsChanged();
    void compilerTypeChanged();

protected:
private:
    QSettings m_settings;
    bool getIEProxySettings(const QString &url, QString &host, int &port);
    bool getFireFoxProxySettings(const QString &url, QString &host, int &port);
    QString debug(void) { return m_settings.value("debug", "").toString(); }
};



#endif
