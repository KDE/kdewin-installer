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
#ifndef DIALOGS_H
#define DIALOGS_H

#include <QMessageBox>
#include <QString>

#include "downloaderprogress.h"
#include "downloader.h"

class InstallerDialogs : public QObject
{
    public:
        void setParent(QWidget *parent) { m_parent = parent; }
        void setTitle(const QString &title) { m_title = title; }

        bool installerOutdated();
        QMessageBox::StandardButton downloadFailed(const QString &url, const QString &error=QString());
        void downloadProgressDialog(QWidget *parent,bool show, const QString &title=QString());

        static InstallerDialogs &instance();

    protected:
        QWidget *m_parent;
        QString m_title;
        DownloaderProgress *m_progress;
        DownloaderProgress *m_oldProgress;
        QDialog *m_d;
};

#endif
