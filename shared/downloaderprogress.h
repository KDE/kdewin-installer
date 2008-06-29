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

#include <QtCore/QString>

class QUrl;
class QObject;

#ifdef USE_GUI

#include <QtGui/QWidget>

class QLabel;
class QProgressBar;
class GenericProgress : public QWidget
{
public:
    GenericProgress(QWidget *parent);
    virtual ~GenericProgress();
    virtual void setTitle(const QString &title);
    virtual void show();
    virtual void hide();
protected:
    virtual void updateDisplay();
    QLabel *m_titleLabel;
    QWidget *m_parent;
};

class DownloaderProgress : public GenericProgress
{
public:
    DownloaderProgress(QWidget *parent);
    virtual ~DownloaderProgress();
    void setTitle(const QUrl &url, const QString &localFile=QString());
    void setMaximum(int value);
    void setValue(int value);
    void setFileCount(int count);
    void setFileNumber(int number);
    void show();
protected:
    void updateDisplay();

private:
    time_t m_initTime; 
    QLabel *m_speedLabel;
    int m_lastValue;
    int m_fileNumber;
    int m_fileCount;
    QProgressBar *m_progress;
};

#else
class DownloaderProgress
{
public:
    DownloaderProgress(QObject *parent=0);
    void hide();
    void setTitle(const QUrl &url, const QString &fileName=QString());
    void setTitle(const QString &title);
    void setStatus(const QString &status);
    void setMaximum(int value);
    void setValue(int value);
    void show();
    DownloaderProgress &instance();

private:
    int oldunit;
    bool visible;
};
#endif


#endif
