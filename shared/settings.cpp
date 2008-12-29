/****************************************************************************
**
** Copyright (C) 2006-2007 Ralf Habacker. All rights reserved.
**
** This file is part of the KDE installer for windows
**
** This library is free software; you can redistribute it and/or
** modify it under the terms of the GNU Library General Publi
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

#include "settings.h"
#include "misc.h"

#include <QtCore/QDir>
#include <QtCore/QUrl>
#include <QtCore/QtDebug>

#ifdef Q_WS_WIN
#include <windows.h>
#endif

#define SETTINGS_VERSION "2"

Settings::Settings()
 : m_settingsMain( new QSettings(QSettings::IniFormat, QSettings::UserScope, "KDE", "Installer") ), 
   m_settings( new QSettings(installDir()+"/etc/installer.ini",QSettings::IniFormat) )

{
    QString version = m_settings->value("version", "").toString();
    // update to current version
    if (version.isEmpty()) // version 1
    {
        m_settings->setValue("version", SETTINGS_VERSION);
        setPackageManagerMode(m_settings->value("DeveloperMode",false).toBool());
        m_settings->remove("DeveloperMode");
        m_settings->sync();
    }
    else if (version != SETTINGS_VERSION)
    {
        qCritical() << "unknown settings version" << version << "found"; 
    }

#ifdef DEBUG
    qDebug() << "installDir:" << installDir();
    qDebug() << "downloadDir:" << downloadDir();
    qDebug() << "showTitlePage:" << showTitlePage();
    qDebug() << "createStartMenuEntries:" << createStartMenuEntries();
    qDebug() << "isFirstRun" << isFirstRun();
    qDebug() << "proxyMode" << proxyMode();
    qDebug() << "proxyHost" << proxyHost();
    qDebug() << "proxyPort" << proxyPort();
#endif
#ifndef Q_OS_WIN
    setProxyMode(Environment);
#endif
}
Settings::~Settings()
{
    sync();
    delete m_settings;
    delete m_settingsMain;
}

QString Settings::installDir() const
{
    QString dir = m_settingsMain->value("rootdir", "").toString();
    if(dir.isEmpty())
        dir = QString::fromLocal8Bit( qgetenv( "ProgramFiles" ) ) + "/KDE";
    QFileInfo fi(dir);
    if(!fi.exists())
    {
        if(!QDir().mkpath(dir))
        {
            qWarning() << "could not create directory" << dir;
            return QDir::currentPath();
        }
        return dir;
    }
    if(!fi.isDir())
    {
        qWarning() << "rootdir is no directory " << dir;
        return QDir::currentPath();
    }
    return dir;
}

void Settings::setInstallDir(const QString &dir)
{
    if (dir != installDir())
    {
        m_settingsMain->setValue("rootdir", dir);
        m_settingsMain->sync();
        m_settings->sync();
        delete m_settings;
        m_settings = new QSettings(dir+"/etc/installer.ini",QSettings::IniFormat);
        emit installDirChanged(dir);
    }
    }

QString Settings::downloadDir() const
{
    QString result;
    QStringList tempPathes;
    QString path = m_settingsMain->value("tempdir","").toString();

#ifdef Q_WS_WIN
    if (path.isEmpty()) {
        WCHAR *buf = new WCHAR[256];
        int iRet = GetTempPathW(256, buf);
        if( iRet > 255 ) {
           delete[] buf;
           buf = new WCHAR[iRet];
           GetTempPathW(iRet, buf);
        }
        WCHAR *buf2 = new WCHAR[256];
        iRet = GetLongPathNameW(buf, buf2, 256);
        if( iRet > 255 ) {
           delete[] buf2;
           buf2 = new WCHAR[iRet];
           GetLongPathNameW(buf, buf2, iRet);
        }
        path = QString::fromUtf16((const ushort*)buf2, iRet);
        delete[] buf;
        delete[] buf2;
    }
#else
    if (path.isEmpty())
        path = QString::fromLocal8Bit(qgetenv("TEMP"));
    if (path.isEmpty())
        path = QString::fromLocal8Bit(qgetenv("TMP"));
#endif
    QDir d(path);
    if (d.exists())
        return QDir::toNativeSeparators(d.absolutePath());
            
    if (d.mkpath(d.absolutePath()))
        return QDir::toNativeSeparators(d.absolutePath());

    qWarning() << "could not setup temporay directory" << d.absolutePath();
    // last ressort path
    return QDir::toNativeSeparators(QDir::currentPath());
}

void Settings::setDownloadDir(const QString &dir)
{
    QDir d(dir);
    m_settingsMain->setValue("tempdir", QDir::toNativeSeparators(d.absolutePath()));
    sync();
    emit downloadDirChanged(QDir::toNativeSeparators(d.absolutePath()));
}

void Settings::setCompilerType(CompilerType ctype)
{
    if (compilerType() != ctype) {
        m_settings->setValue("compilerType", (int)ctype);
        m_settings->sync();
        emit compilerTypeChanged();
    }
}

QString Settings::mirror() const
{
    return m_settingsMain->value("mirror", "").toString();
}

void Settings::setMirror(const QString &mirror)
{
    m_settingsMain->setValue("mirror", mirror);
    m_settingsMain->sync();
}

QStringList Settings::localMirrors() const
{
    return m_settingsMain->value("localMirrors").toStringList();
}

void Settings::addLocalMirror(const QString &locMirror)
{
    m_settingsMain->setValue("localMirrors", m_settings->value("localMirrors").toStringList() << locMirror);
    m_settingsMain->sync();
}

void Settings::setLocalMirrors(const QStringList &locMirrors)
{
    m_settingsMain->setValue("localMirrors", locMirrors);
    m_settingsMain->sync();
}

bool Settings::showTitlePage() const
{
    return m_settingsMain->value("displayTitlePage").toBool();
}

void Settings::setShowTitlePage(bool bShow)
{
    m_settingsMain->setValue("displayTitlePage", bShow);
    m_settingsMain->sync();
}

bool Settings::createStartMenuEntries()
{
    return m_settings->value("createStartMenuEntries").toBool();
}

void Settings::setCreateStartMenuEntries(bool bCreate)
{
    m_settings->setValue("createStartMenuEntries", bCreate);
    m_settings->sync();
}

void Settings::sync()
{
    m_settings->sync();
    m_settingsMain->sync();
    emit settingsChanged();
}

Settings &Settings::instance()
{
    static Settings settings;
    return settings;
}

#ifdef Q_OS_WIN
bool Settings::getIEProxySettings(const QString &url, proxySettings &proxy)
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

    proxy.hostname = parts[0];
    proxy.port = parts[1].toInt();
    proxy.user = proxyUser();
    proxy.password = proxyPassword();
    return true;
}
#endif

bool Settings::getEnvironmentProxySettings(const QString &_url, proxySettings &proxy)
{
    QUrl url(qgetenv("http_proxy"));

    if(url.isValid())
    {
        proxy.hostname = url.host();
        proxy.port = url.port(8080);
        proxy.user = url.userName();
        proxy.password = url.password();
        return true;
    }
    return false;
}

bool Settings::getFireFoxProxySettings(const QString &url, proxySettings &proxy)
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
    if (prefs.contains("network.proxy.type"))
    {
        int mode = prefs["network.proxy.type"].toInt();
        // 1 manual proxy
        // 2 use autoconfig url -> network.proxy.autoconfig_url
        // 4 automatic detection, how to do ?
        if (mode == 1)
        {
            if (url.startsWith("http") ||
                       prefs.contains("network.proxy.share_proxy_settings")
                    && prefs["network.proxy.share_proxy_settings"] == "true")
            {
                proxy.hostname = prefs["network.proxy.http"];
                proxy.port = prefs["network.proxy.http_port"].toInt();
                /// @TODO: get username and passwort from firefox settings
                proxy.user = proxyUser();
                proxy.password = proxyPassword();
            }
            else
            {
                proxy.hostname = prefs["network.proxy.ftp"];
                proxy.port = prefs["network.proxy.ftp_port"].toInt();
                /// @TODO: get username and passwort from firefox settings
                proxy.user = proxyUser();
                proxy.password = proxyPassword();

            }
            return true;
        }
    }
    return false;
}

bool Settings::proxy(const QString &url, proxySettings &proxy)
{
    // FIXME: add support for different ftp proxy settings
    switch(proxyMode()) {
#ifdef Q_WS_WIN
        case InternetExplorer:
            return getIEProxySettings(url, proxy);
#endif
        case FireFox:
            return getFireFoxProxySettings(url, proxy);

        case Manual:
            proxy.hostname = proxyHost();
            proxy.port = proxyPort();
            proxy.user = proxyUser();
            proxy.password = proxyPassword();
            return true;
#ifndef Q_WS_WIN
        case Environment:
            return getEnvironmentProxySettings(url, proxy);
#endif
        case None:
        default:
            proxy.hostname = QString();
            proxy.port = 0;
            proxy.user = QString();
            proxy.password = QString();
            break;
    }
    return false;
}

/**
  check if a specific debug group 'area' is available
  The debug group is retrieved from the installer configuration
  file (normally installer.ini) using the entry debug in the
  [General] group. Possible values are 'all' which returns all
  groups are to debug or any string provided in the source.
  See hasDebug() calls in the source.
  @param area
  @return flag if debug area was found
*/
bool Settings::hasDebug(const QString area)
{
    Settings &s = Settings::instance();
    return area.isEmpty() && !s.debug().isEmpty()
        || !area.isEmpty() &&
            (s.debug().toLower() == "all"
            || s.debug().toLower().contains(area.toLower()));
}

QDebug operator<<(QDebug out, Settings &c)
{
    out << "Settings ("
        << "installDir:" << c.installDir()
        << "downloadDir:" << c.downloadDir()
        << "showTitlePage:" << c.showTitlePage()
        << "createStartMenuEntries:" << c.createStartMenuEntries()
        << "isFirstRun" << c.isFirstRun()
        << "proxyMode" << c.proxyMode()
        << "proxyHost" << c.proxyHost()
        << "proxyPort" << c.proxyPort()
        << "proxyUser" << c.proxyUser()
        << "proxyPassword" << c.proxyPassword()
        << ")";
    return out;
}
