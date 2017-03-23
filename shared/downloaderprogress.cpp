/****************************************************************************
**
** Copyright (C) 2005 Ralf Habacker. All rights reserved.
**
** This file is part of the KDE installer for windows
**
** This file may be used under the terms of the GNU General Public
** License version 2.0 as published by the Free Software Foundation
** and appearing in the file LICENSE.GPL included in the packaging of
** this file.  Please review the following information to ensure GNU
** General Public Licensing requirements will be met:
** http://www.trolltech.com/products/qt/opensource.html
**
** If you are unsure which license is appropriate for your use, please
** review the following information:
** http://www.trolltech.com/products/qt/licensing.html or contact the
** sales department at sales@trolltech.com.
**
** This file is provided AS IS with NO WARRANTY OF ANY KIND, INCLUDING THE
** WARRANTY OF DESIGN, MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE.
**
****************************************************************************/

#include "downloaderprogress.h"

#include <time.h>

#include <QDebug>
#include <QUrl>


//#define DISABLE_GENERICPROCESS_PARENT_CONTROL
// show() and hide() controls also parent widget 
// this may have unwanted side effects 
// uncomment the above listed define to disable 

#ifdef USE_GUI
#include <QLabel>
#include <QProgressBar>
#include <QGridLayout>
#include <QDesktopWidget>

GenericProgress::GenericProgress(QWidget *parent)
  : QWidget(parent), m_titleLabel(NULL), m_parent(parent)
{}

GenericProgress::~GenericProgress()
{}

void GenericProgress::setTitle(const QString &title)
{
    if(m_titleLabel)
      m_titleLabel->setText(title);
}

void GenericProgress::show()
{
#ifndef DISABLE_GENERICPROCESS_PARENT_CONTROL
    if(m_parent) {
        m_parent->show();
        return;
    }
#endif
  	QWidget::show();
}

void GenericProgress::hide()
{
#ifndef DISABLE_GENERICPROCESS_PARENT_CONTROL
    m_parent ? m_parent->hide() : QWidget::hide();
#else
    QWidget::hide();
#endif
}

void GenericProgress::updateDisplay()
{
}

/*
  DownloaderProgress
*/
DownloaderProgress::DownloaderProgress(QWidget *parent)
  : GenericProgress(parent), m_lastValue(0), m_fileNumber(0), m_fileCount(0)
{    
    QGridLayout *mainLayout = new QGridLayout(this);

    m_titleLabel = new QLabel;
    mainLayout->addWidget(m_titleLabel, 0, 0, 1, 1);

    m_progress = new QProgressBar(parent);
    mainLayout->addWidget(m_progress, 1, 0, 1, 1);

    m_speedLabel = new QLabel;
    mainLayout->addWidget(m_speedLabel, 2, 0, 1, 1);

    hide();
}

DownloaderProgress::~DownloaderProgress()
{}

void DownloaderProgress::setTitle(const QUrl &url, const QString &file)
{
    if (m_fileCount && m_titleLabel) 
    {
        if (!file.isEmpty())
            m_titleLabel->setText( tr("Downloading file %3 of %4 : %1 to %2 ").arg(url.toString()).arg(file).arg(m_fileNumber+1).arg(m_fileCount) );
        else 
            m_titleLabel->setText( tr("Downloading file %2 of %3 : %1").arg(url.toString()).arg(m_fileNumber+1).arg(m_fileCount) );
    }
    else 
        if (!file.isEmpty())
            m_titleLabel->setText( tr("Downloading %1 to %2").arg(url.toString()).arg(file) );
        else 
            m_titleLabel->setText( tr("Downloading %1").arg(url.toString()) );
}    

void DownloaderProgress::show()
{
    m_initTime = time(NULL);
    GenericProgress::show();
}

void DownloaderProgress::setMaximum(int value)
{
    if (m_progress->maximum() == value)
        return;
    m_progress->setMaximum(value);

    updateDisplay();
}

void DownloaderProgress::setValue(int value)
{
    m_progress->setValue(value);
    updateDisplay();
}

void DownloaderProgress::setFileCount(int count)
{
    m_fileCount = count;
}

void DownloaderProgress::setFileNumber(int number)
{
    m_fileNumber = number;
}

void DownloaderProgress::updateDisplay()
{
    // calculate speed 
    int range = m_progress->maximum() - m_progress->minimum();
    int value = m_progress->value();
    if (value == 0 || range == 0) {
        m_speedLabel->setText(QLatin1String(""));
        return;
    }
    // only update rate when difference >= 1% 
    int diff = (value - m_lastValue) * 100 / range; 
    if (diff == 0)
        return; 
    time_t now = time(NULL);
    time_t seconds = now - m_initTime;
    if (seconds == 0)
        return;
    int speed = value/seconds;
    m_speedLabel->setText(tr("%1 KB of %2 KB at %3 KB/s").arg(value/1024).arg(range/1024).arg(speed/1024));
    m_lastValue = value;
}

#else // console implementation

DownloaderProgress::DownloaderProgress(QObject *parent)
{
    hide();
}

void DownloaderProgress::hide()
{
    visible = false;
}

void DownloaderProgress::setTitle(const QString &title)
{
    qDebug() << title;
}

void DownloaderProgress::setTitle(const QUrl &url, const QString &file)
{
    qDebug() << url << "to" << file;
}

void DownloaderProgress::setStatus(const QString &status)
{
    qDebug() << status;
}

void DownloaderProgress::setMaximum(int value)
{}

void DownloaderProgress::setValue(int value)
{
    int unit = value/10240;
    if (oldunit != unit)
    {
        if (visible)
            putchar('.');
        oldunit = unit;
    }
}

void DownloaderProgress::show()
{
    visible = true;
}

#endif
