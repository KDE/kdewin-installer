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
        path += "/KDE";
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

void Settings::setCompilerType(CompilerTypes::CompilerType ctype)
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
    const QString m = mirror.endsWith('/') ? mirror : mirror + '/';
    m_settingsMain->setValue("mirror", m);
    m_settingsMain->sync();
}

QStringList Settings::localMirrors() const
{
    return m_settingsMain->value("localMirrors").toStringList();
}

void Settings::addLocalMirror(const QString &locMirror)
{
    const QString m = locMirror.endsWith('/') ? locMirror : locMirror + '/';
    m_settingsMain->setValue("localMirrors", m_settings->value("localMirrors").toStringList() << m);
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
