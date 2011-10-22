/****************************************************************************
**
** Copyright (C) 2010 Ralf Habacker ralf.habacker@freenet.de
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

#ifndef INSTALLERDIALOG_H
#define INSTALLERDIALOG_H

#include "ui_installerdialog.h"

#include "installerEngineSinglePackage.h"

#include "../shared/package.h"
#include "../shared/postprocessing.h"



#include <QDialog>

class QPixmap;
class QTextEdit;

class InstallerDialog : public QDialog
{
    Q_OBJECT

public:
    InstallerDialog();
    ~InstallerDialog();

protected slots:
    void setupEngine();
    void downloadPackages();
    void downloadPackagesStage1();
    void downloadPackagesStage2();
    void installPackages();
    void postProcessing();
    void finished();

protected:
    Ui::InstallerDialog ui;
    InstallerEngineSinglePackage m_engine;
    void closeEvent(QCloseEvent *e);

protected slots:
    void addHint(const QString &hint);
    void setSubLabelHint(const QString &hint);
    void showLog(bool checked);
    void stop();

private:
    void initItems();
    void setItem(int pagenum);
    void setError(int pagenum);

    QStringList packages;
    QList<Package *> packagesToInstall;
    PostProcessing m_postProcessing;
    int page;
    QPixmap next;
    QPixmap okay;
    QPixmap error;
    int m_counter;
    QTextEdit *m_log;
};

#endif
