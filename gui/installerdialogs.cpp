/****************************************************************************
**
** Copyright (C) 2005-2007 Ralf Habacker. All rights reserved.
**
** This file is part of the KDE installer for windows
**
** SPDX-License-Identifier: LGPL-2.0-only
**
****************************************************************************/

#include "installerdialogs.h"
#include "debug.h"
#include "settings.h"

#include <QDesktopWidget>
#include <QVBoxLayout>

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
        text = tr("<p>The download of %1 failed with error %2.</p>").arg(url).arg(error);
    else
        text = tr("<p>The download of %1 failed.</p>").arg(url);
    text += logFileLink();

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
    QString text = tr("<p>The mirror list could not be downloaded from any of the following locations:"
        "<ul><li><a href=\"%1\">%1</a></li><li><a href=\"%2\">%2</a></li></ul></p>"
        "<p>You may check your proxy settings</p>").arg(url1.toString()).arg(url2.toString());
    text += logFileLink();
	
    return QMessageBox::critical ( 
                m_parent, 
                m_title, 
                text, 
                QMessageBox::Cancel,
                QMessageBox::Cancel
            );
}

void InstallerDialogs::enableDownloadProgressDialog(QWidget *parent, const QString &title)
{
    m_d = new QDialog(parent);
    m_progress = new DownloaderProgress(m_d);
    if (!title.isEmpty())
        m_d->setWindowTitle(title);
    QVBoxLayout *layout = new QVBoxLayout(parent);
    layout->addWidget(m_progress);
    m_d->setLayout(layout);
    m_oldProgress = Downloader::instance()->progress();
    Downloader::instance()->setProgress(m_progress);
}

void InstallerDialogs::disableDownloadProgressDialog()
{
    Downloader::instance()->setProgress(m_oldProgress);
    if(m_d)
        m_d->hide();
    delete m_d;
    m_d = NULL;
    m_progress = NULL;
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

QString InstallerDialogs::logFileLink()
{
    return tr("<p>For details see the installer log file located at<br><a href=\"%3\">%3</a></p>").arg(logFileNameAsURL());
}

InstallerDialogs &InstallerDialogs::instance()
{
    static InstallerDialogs instance;
    return instance;
}
