/****************************************************************************
**
** Copyright (C) 2005-2007 Ralf Habacker. All rights reserved.
**
** This file is part of the KDE installer for windows
**
** SPDX-License-Identifier: LGPL-2.0-only
**
****************************************************************************/
#ifndef DIALOGS_H
#define DIALOGS_H

#include <QString>
#include <QMessageBox>

#include "downloaderprogress.h"
#include "downloader.h"

class InstallerDialogs : public QObject
{
    public:
        ~InstallerDialogs();
        void setParent(QWidget *parent) { m_parent = parent; }
        void setTitle(const QString &title) { m_title = title; }

        bool installerOutdated();
        bool newInstallerAvailable();
        bool newInstallerDownloadError();
        QMessageBox::StandardButton downloadFailed(const QString &url, const QString &error=QString());
        bool downloadMirrorListFailed(const QUrl &url1, const QUrl &url2);
        void enableDownloadProgressDialog(QWidget *parent, const QString &title=QString());
        void disableDownloadProgressDialog();
        bool confirmRemovalDialog();
        bool confirmRepairDialog();
        bool confirmKillKDEAppsDialog();

        static InstallerDialogs &instance();

    protected:
        QWidget *m_parent;
        DownloaderProgress *m_progress;
        DownloaderProgress *m_oldProgress;
        QDialog *m_d;
        QString m_title;
    private:
        InstallerDialogs();
        QString logFileLink();
};

#endif
