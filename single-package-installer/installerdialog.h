/****************************************************************************
**
** Copyright (C) 2010 Ralf Habacker ralf.habacker@freenet.de
**
** This file is part of the KDE installer for windows
**
** SPDX-License-Identifier: LGPL-2.0-only
**
****************************************************************************/

#ifndef INSTALLERDIALOG_H
#define INSTALLERDIALOG_H

#include "ui_installerdialog.h"

#include "installerenginesinglepackage.h"

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

    QStringList m_packages;
    QList<Package *> m_packagesToInstall;
    PostProcessing m_postProcessing;
    int page;
    QPixmap next;
    QPixmap okay;
    QPixmap error;
    int m_counter;
    QTextEdit *m_log;
};

#endif
