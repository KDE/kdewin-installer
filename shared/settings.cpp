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

#include "settings.h"

Settings::Settings()
 : m_settings(QSettings::IniFormat, QSettings::UserScope, "KDE", "Installer")
{}

QString Settings::installDir()
{
    QString dir = m_settings.value("rootdir", QDir::currentPath () ).toString();
    QFileInfo fi(dir);
    if(!fi.exists()) {
        if(!QDir().mkdir(dir))
            return QDir::currentPath();
    }
    if(!fi.isDir())
        return QDir::currentPath();
    return dir;
}

void Settings::setInstallDir(const QString &dir)
{
    m_settings.setValue("rootdir", dir);
    emit installDirChanged(dir);
}

QString Settings::downloadDir()
{
    QString dir = m_settings.value("tempdir", QDir::currentPath()).toString();
    QFileInfo fi(dir);
    if(!fi.exists()) {
        if(!QDir().mkdir(dir))
            return QDir::currentPath();
    }
    if(!fi.isDir())
        return QDir::currentPath();
    return dir;
}

void Settings::setDownloadDir(const QString &dir)
{
    m_settings.setValue("tempdir", dir);
    emit downloadDirChanged(dir);
}

bool Settings::showTitlePage()
{
    return m_settings.value("displayTitlePage").toBool();
}

void Settings::setShowTitlePage(bool bShow)
{
    m_settings.setValue("displayTitlePage", bShow);
}

bool Settings::createStartMenuEntries()
{
    return m_settings.value("createStartMenuEntries").toBool();
}

void Settings::setCreateStartMenuEntries(bool bCreate)
{
    m_settings.setValue("createStartMenuEntries", bCreate);
}

bool Settings::isFirstRun()
{
    bool bFirst = m_settings.value("FirstRun", true).toBool();
    if(!bFirst)
        m_settings.setValue("FirstRun", false);
    return bFirst;
}

#include "settings.moc"
