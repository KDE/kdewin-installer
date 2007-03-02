/****************************************************************************
**
** Copyright (C) 2006 Ralf Habacker. All rights reserved.
**
** This file is part of the KDE installer for windows
**
** This file may be used under the terms of the GNU General Public
** License version 2.0 as published by the Free Software Foundation
** and appearing in the file LICENSE.GPL included in the packaging of
** this file.  Please review the following information to ensure GNU
** General Public Licensing requirements will be met:
** http://www.trolltech.com/products/qt/opensource.html
**
** If you are unsure which license is appropriate for your use, please
** review the following information:
** http://www.trolltech.com/products/qt/licensing.html or contact the
** sales department at sales@trolltech.com.
**
** This file is provided AS IS with NO WARRANTY OF ANY KIND, INCLUDING THE
** WARRANTY OF DESIGN, MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE.
**
****************************************************************************/

#include <QDir>
#include <QtDebug>

#include "settings.h"
#include "misc.h"

Settings::Settings()
 : m_settings(QSettings::IniFormat, QSettings::UserScope, "KDE", "Installer")
{
    qDebug() << "installDir:" << installDir();
    qDebug() << "downloadDir:" << downloadDir();
    qDebug() << "showTitlePage:" << showTitlePage();
    qDebug() << "createStartMenuEntries:" << createStartMenuEntries();
    qDebug() << "isFirstRun" << isFirstRun();
	qDebug() << "proxyMode" << proxyMode();
	qDebug() << "proxyHost" << proxyHost();
	qDebug() << "proxyPort" << proxyPort();
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
    m_settings.setValue("rootdir", dir);
	m_settings.sync();
    emit installDirChanged(dir);
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
    m_settings.setValue("tempdir", dir);
	m_settings.sync();
    emit downloadDirChanged(dir);
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

Settings &Settings::getInstance()
{
    static Settings settings;
    return settings;
}



bool Settings::getIEProxySettings(const QString &url, QString &host, int &port)
{
	host = "";
	port = 0;

	bool ok; 
    QString enable = getWin32RegistryValue(hKEY_CURRENT_USER,"Software\\Microsoft\\Windows\\CurrentVersion\\Internet Settings","ProxyEnable",&ok);
    if (!ok)
        return false; 
	// FIXME: getWin32RegistryValue does not return propper dword values 
	// the only way is using the size to detect 0 value from other values.
    qDebug() << enable; 
	if (enable.size() == 0)
		return false;
    
    QString proxyServer = getWin32RegistryValue(hKEY_CURRENT_USER,"Software\\Microsoft\\Windows\\CurrentVersion\\Internet Settings","ProxyServer",&ok);
    if (!ok)
        return false; 
    
    QStringList parts = proxyServer.split(":");
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
		QString mozillaProfileDir = getenv("APPDATA");///lb4kyocy.default";
		QDir d(mozillaProfileDir + "/Mozilla/Firefox/Profiles");

		d.setFilter(QDir::NoDotAndDotDot | QDir::AllDirs);
		d.setNameFilters(QStringList("*.default"));
		d.setSorting(QDir::Name);

		QFileInfoList list = d.entryInfoList();
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
				QString value = attr[1].mid(1,attr[1].length()-1).replace("\"","");
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
	host = "";
	port = 0;
	return false;
}


bool Settings::getProxySettings(const QString &url, QString &host, int &port)
{
	// FIXME: add support for different ftp proxy settings
	int mode = proxyMode();
	if (mode == 1)
		return getIEProxySettings(url,host,port);
	else if (mode == 2) 
	{
		host = proxyHost();
		port = proxyPort();
		return true; 
	}
	else if (mode == 3)
		return getFireFoxProxySettings(url,host,port);
	else 
	{
		host="";
		port = 0;
		return false;
	}
}


#include "settings.moc"
