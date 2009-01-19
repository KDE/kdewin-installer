/****************************************************************************
**
** Copyright (C) 2005-2007 Ralf Habacker. All rights reserved.
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

#include "installerdialogs.h"
#include "settings.h"

#include <QtGui/QDesktopWidget>
#include <QtGui/QVBoxLayout>

InstallerDialogs::InstallerDialogs()
: m_parent(0), m_progress(0), m_oldProgress(0), m_d(0)
{}

InstallerDialogs::~InstallerDialogs()
{
    delete m_d;
}

bool InstallerDialogs::installerOutdated()
{
    QString text = tr("Your installer version is outdated. Please download and install the newest version ");
    QMessageBox::StandardButton result = QMessageBox::warning ( 
            m_parent, 
            m_title, 
            text
        );
    return true; 
}

bool InstallerDialogs::newInstallerAvailable()
{
    QString text = tr("There is a new installer available. Should I download it to the same location as the recent installer and run it ?");
    QMessageBox::StandardButton result = QMessageBox::warning ( 
            m_parent, 
            m_title, 
            text,
            QMessageBox::Cancel | QMessageBox::Ok,
            QMessageBox::Ok
        );
    return result == QMessageBox::Ok;
}

bool InstallerDialogs::newInstallerDownloadError()
{
    QString text = tr("Could not download new installer. You should search manually for the new installer");
    QMessageBox::StandardButton result = QMessageBox::warning ( 
            m_parent, 
            m_title, 
            text
        );
    return true; 
}

QMessageBox::StandardButton InstallerDialogs::downloadFailed(const QString &url, const QString &error)
{
    QString text;
    if (!error.isEmpty())
        text = tr("The download of %1 failed with error %2").arg(url).arg(error);
    else
        text = tr("The download of %1 failed").arg(url);
    return QMessageBox::critical ( 
                m_parent, 
                m_title, 
                text, 
                QMessageBox::Cancel | QMessageBox::Ignore | QMessageBox::Retry,
                QMessageBox::Retry
            );
}

bool InstallerDialogs::downloadMirrorListFailed(const QUrl &url1, const QUrl &url2)
{
    QString text = tr("<p>The mirror list could not be download from any of the following locations:"
        "<ul><li><a href=\"%1\">%1</a></li><li><a href=\"%2\">%2</a></li></ul></p>"
        "<p>You max check your proxy settings"
        " and/or inspect the installer log file located at<br><a href=\"%3\">%3</a></p>").arg(url1.toString()).arg(url2.toString()).arg(Settings::instance().logFile());
    return QMessageBox::critical ( 
                m_parent, 
                m_title, 
                text, 
                QMessageBox::Cancel,
                QMessageBox::Cancel
            );
}

void InstallerDialogs::downloadProgressDialog(QWidget *parent,bool show, const QString &title)
{
    if (show) 
    {
        m_d = new QDialog(parent);
        m_progress = new DownloaderProgress(m_d);
        if (!title.isEmpty())
            m_d->setWindowTitle(title);
        QVBoxLayout *layout = new QVBoxLayout(m_d); 
        layout->addWidget(m_progress);
        m_d->setLayout(layout);
        m_oldProgress = Downloader::instance()->progress();
        Downloader::instance()->setProgress(m_progress);
    }
    else 
    {
        Downloader::instance()->setProgress(m_oldProgress);
        if(m_d)
            m_d->hide();
        delete m_d;
        m_d = NULL;
        m_progress = NULL;
    }
}

bool InstallerDialogs::confirmRemovalDialog()
{
    QString text = tr("Are you sure to remove the KDE installation from %1 ? ").arg(Settings::instance().installDir());
    QMessageBox::StandardButton result = QMessageBox::warning ( 
            m_parent, 
            m_title, 
            text,
            QMessageBox::Cancel | QMessageBox::Ok,
            QMessageBox::Cancel
        );
    return result == QMessageBox::Ok;
}

bool InstallerDialogs::confirmRepairDialog()
{
    QString text = tr("Are you sure to reinstall all packages ? ");
    QMessageBox::StandardButton result = QMessageBox::warning ( 
            m_parent, 
            m_title, 
            text,
            QMessageBox::Cancel | QMessageBox::Ok,
            QMessageBox::Cancel
        );
    return result == QMessageBox::Ok;
}

bool InstallerDialogs::confirmKillKDEAppsDialog()
{
    QString text = tr("There are KDE applications running, which need to be closed "
        "before any package update, removal or installation could be performed.\n"
        "Please save all unsaved work, then press Okay to continue the install process."
        "If you press the Cancel button, the currently started installation will be aborted.");
    QMessageBox::StandardButton result = QMessageBox::warning ( 
            m_parent, 
            m_title, 
            text,
            QMessageBox::Cancel | QMessageBox::Ok,
            QMessageBox::Cancel
        );
    return result == QMessageBox::Ok;
}


InstallerDialogs &InstallerDialogs::instance()
{
    static InstallerDialogs instance;
    return instance;
}
