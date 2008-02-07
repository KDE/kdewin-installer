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

#include <QVBoxLayout>
#include "installerdialogs.h"

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

void InstallerDialogs::downloadProgressDialog(QWidget *parent,bool show)
{
    if (show) 
    {
        progress = new DownloaderProgress(0);
        d = new QDialog(parent);
        QVBoxLayout *layout = new QVBoxLayout(d); 
        layout->addWidget(progress);
        d->setLayout(layout);
        d->show();
        Downloader::instance()->setProgress(progress);
    }
    else 
    {
        Downloader::instance()->setProgress(0);
        d->hide();
        delete d;
    }
}

InstallerDialogs &InstallerDialogs::instance()
{
    static InstallerDialogs instance;
    return instance;
}
