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

#ifndef INSTALLERPROGRESS_H
#define INSTALLERPROGRESS_H

#include <QString>

#ifdef USE_GUI

#include <QWidget>

class QLabel;
class QProgressBar;
class QListWidget;

class InstallerProgress : public QWidget
{
public:
    InstallerProgress(QWidget *parent);
    ~InstallerProgress();
    void hide();
    void setTitle(const QString &title);
    void setStatus(const QString &status);
    void setMaximum(int value);
    void setValue(int value);
    void show();
private:
    QLabel *titleLabel;
    QLabel *statusLabel;
//    QProgressBar *progress;
		QListWidget *progress;
};

#else
class InstallerProgress
{
public:
    InstallerProgress(QObject *parent);
    void hide();
    void setTitle(const QString &title);
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
