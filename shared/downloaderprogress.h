/****************************************************************************
**
** Copyright (C) 2005-2007 Ralf Habacker. All rights reserved.
**
** This file is part of the KDE installer for windows
**
** SPDX-License-Identifier: LGPL-2.0-only
**
****************************************************************************/

#ifndef DOWNLOADERPROGRESS_H
#define DOWNLOADERPROGRESS_H

#include <QString>

class QUrl;
class QObject;

#ifdef USE_GUI

#include <QWidget>

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
    virtual void setValue(int value) {}
protected:
    virtual void updateDisplay();
    QLabel *m_titleLabel;
    QWidget *m_parent;
};

/**
  DownloaderProgress provides a user feedback about the
  download progress. In gui mode it contains a progress bar
  and text with remote url, local file name, the number of
  files to download and the index of the current downloaded
  file.
*/
class DownloaderProgress : public GenericProgress
{
public:
    DownloaderProgress(QWidget *parent);
    virtual ~DownloaderProgress();
    void setTitle(const QUrl &url, const QString &localFile=QString());
    /// set progress bar maximum
    void setMaximum(int value);
    /// set progress bar value
    void setValue(int value);
    /// set number of files
    void setFileCount(int count);
    /// set current number of file
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
