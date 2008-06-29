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

InstallerDialogs &InstallerDialogs::instance()
{
    static InstallerDialogs instance;
    return instance;
}
