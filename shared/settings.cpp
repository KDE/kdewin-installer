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

#include <QDir>
#include <QtDebug>

#include "settings.h"
#include "misc.h"

Settings::Settings()
 : m_settings(QSettings::IniFormat, QSettings::UserScope, "KDE", "Installer")
{
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
}

QString Settings::installDir()
{
    QString dir = m_settings.value("rootdir", QDir::currentPath () ).toString();
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
        m_settings.setValue("rootdir", dir);
        m_settings.sync();
        emit installDirChanged(dir);
    }
    }

QString Settings::downloadDir()
{
    QString dir = m_settings.value("tempdir", QDir::currentPath()).toString();
    QFileInfo fi(dir);
    if(!fi.exists()) {
        if(!QDir().mkdir(dir))
        {
            qWarning() << "could not create directory" << dir;
            return QDir::currentPath();
        }
        return dir;
    }
    if(!fi.isDir())
    {
        qWarning() << "tempdir is no directory " << dir;
        return QDir::currentPath();
    }
    return dir;
}

void Settings::setDownloadDir(const QString &dir)
{
    if (dir != downloadDir())
    {
        m_settings.setValue("tempdir", dir);
        sync();
        emit downloadDirChanged(dir);
    }
}

void Settings::setCompilerType(CompilerType ctype) 
{ 
    if (compilerType() != ctype) {
        m_settings.setValue("compilerType", (int)ctype); 
        m_settings.sync(); 
        emit compilerTypeChanged();
    }
}

bool Settings::showTitlePage()
{
    return m_settings.value("displayTitlePage").toBool();
}

void Settings::setShowTitlePage(bool bShow)
{
    m_settings.setValue("displayTitlePage", bShow);
    m_settings.sync();
}

bool Settings::createStartMenuEntries()
{
    return m_settings.value("createStartMenuEntries").toBool();
}

void Settings::setCreateStartMenuEntries(bool bCreate)
{
    m_settings.setValue("createStartMenuEntries", bCreate);
    m_settings.sync();
}

void Settings::sync() 
{ 
    m_settings.sync(); 
    emit settingsChanged(); 
}


Settings &Settings::getInstance()
{
    static Settings settings;
    return settings;
}



bool Settings::getIEProxySettings(const QString &url, QString &host, int &port)
{
    host = QString();
    port = 0;

    bool ok; 
    quint32 enable = getWin32RegistryValue(hKEY_CURRENT_USER,"Software\\Microsoft\\Windows\\CurrentVersion\\Internet Settings","ProxyEnable",&ok).toUInt();
    if (!ok)
        return false; 

    if (enable == 0)
        return false;
    
    QString proxyServer = getWin32RegistryValue(hKEY_CURRENT_USER,"Software\\Microsoft\\Windows\\CurrentVersion\\Internet Settings","ProxyServer",&ok).toString();
    if (!ok)
        return false; 
    
    QStringList parts = proxyServer.split(':');
    if(parts.count() != 2)
        return false;

    host = parts[0]; 
    port = parts[1].toInt(); 

    qDebug() << enable << host << port;
    return true;
}

bool Settings::getFireFoxProxySettings(const QString &url, QString &host, int &port)
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
                QString data = line.mid(10,line.length()-11-1);
                QStringList attr = data.split(',');
                QString key = attr[0].mid(1,attr[0].length()-2);
                QString value = attr[1].mid(1,attr[1].length()-1).remove('\"');
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
                host = prefs["network.proxy.http"];
                port = prefs["network.proxy.http_port"].toInt();
                return true;
            }
            host = prefs["network.proxy.ftp"];
            port = prefs["network.proxy.ftp_port"].toInt();
            return true;
        }
    }
    host = QString();
    port = 0;
    return false;
}


bool Settings::getProxySettings(const QString &url, QString &host, int &port)
{
    // FIXME: add support for different ftp proxy settings
    switch(proxyMode()) {
        case InternetExplorer:
            return getIEProxySettings(url, host, port);
        case FireFox:
            return getFireFoxProxySettings(url, host, port);
        case Manual:
            host = proxyHost();
            port = proxyPort();
            return true; 
        case None:
        default:
            host = QString();
            port = 0;
            break;
    };
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
	Settings &s = Settings::getInstance();
	return area.isEmpty() && !s.debug().isEmpty()
		|| !area.isEmpty() && 
			(s.debug().toLower() == "all" 
			|| s.debug().toLower().contains(area.toLower()));
}




#include "settings.moc"
