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

#ifndef DOWNLOADERPROGRESS_H
#define DOWNLOADERPROGRESS_H

#include <QString>

#ifdef USE_GUI

#include <QWidget>
class QLabel;
class QProgressBar;

class DownloaderProgress : public QWidget
{
public:
    DownloaderProgress(QWidget *parent=0);
    ~DownloaderProgress();
    void hide();
    void setTitle(const QString &title);
    void setStatus(const QString &status);
    void setMaximum(int value);
    void setValue(int value);
    void show();
    DownloaderProgress &getInstance();
private:
    QLabel *titleLabel;
    QLabel *statusLabel;
    QProgressBar *progress;
};

#else
class DownloaderProgress
{
public:
    DownloaderProgress(QObject *parent=0);
    void hide();
    void setTitle(const QString &title);
    void setStatus(const QString &status);
    void setMaximum(int value);
    void setValue(int value);
    void show();
    DownloaderProgress &getInstance();

private:
    int oldunit;
    bool visible;
};
#endif


#endif
