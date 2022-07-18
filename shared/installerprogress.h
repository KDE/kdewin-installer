/****************************************************************************
**
** Copyright (C) 2005-2007 Ralf Habacker. All rights reserved.
**
** This file is part of the KDE installer for windows
**
** SPDX-License-Identifier: LGPL-2.0-only
**
****************************************************************************/

#ifndef INSTALLERPROGRESS_H
#define INSTALLERPROGRESS_H

#include "downloaderprogress.h"

#ifdef USE_GUI

class InstallerProgress : public GenericProgress
{
public:
    InstallerProgress(QWidget *parent, bool showProgress=true);
    virtual ~InstallerProgress();
    void setTitle(const QString &title);
    void setPackageName(const QString &packageName);
    void setFileName(const QString &fileName);
    void setPackageCount(int value);
    void setPackageNumber(int value);
    void show();
private:
    void updateDisplay();
    QProgressBar *m_progress;
    QLabel  *m_fileNameLabel;
    QString m_packageName;
    QString m_fileName;
};

#else
class InstallerProgress
{
public:
    InstallerProgress(QObject *parent);
    void hide();
    void setTitle(const QString &title);
    void setPackageName(const QString &packageName);
    void setFileName(const QString &fileName);
    void setStatus(const QString &status);
    void setMaximum(int value);
    void setValue(int value);
    void show();

private:
    int oldunit;
    bool visible;
};
#endif


#endif
