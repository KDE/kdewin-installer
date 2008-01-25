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

#include <QDebug>
#include <QLabel>
#include <QProgressBar>
#include <QHBoxLayout>
#include <QVBoxLayout>

#include "downloaderprogress.h"

#ifdef USE_GUI

DownloaderProgress::DownloaderProgress(QWidget *parent)
{    
    mainLayout = new QVBoxLayout;

    QHBoxLayout *statusLayout = new QHBoxLayout;
    mainLayout->addLayout(statusLayout);

    statusLabel = new QLabel;
    statusLayout->addWidget(statusLabel);

    progress = new QProgressBar(parent);
    statusLayout->addWidget(progress);

    titleLabel = new QLabel;
    mainLayout->addWidget(titleLabel);

    speedLabel = new QLabel;
    mainLayout->addWidget(speedLabel);

    setLayout(mainLayout);
    hide();
}

DownloaderProgress::~DownloaderProgress()
{
    delete mainLayout;
    mainLayout = 0;
}

void DownloaderProgress::hide()
{
    titleLabel->hide();
    statusLabel->hide();
    progress->hide();
}

void DownloaderProgress::setTitle(const QString &label)
{
    titleLabel->setText(label);
}

void DownloaderProgress::setStatus(const QString &label)
{
    statusLabel->setText(label);
}

void DownloaderProgress::show()
{
    titleLabel->show();
    statusLabel->show();
    progress->show();
    initTime = QDateTime::currentDateTime();
}

void DownloaderProgress::setMaximum(int value)
{
    if (progress->maximum() == value)
        return;
    progress->setMaximum(value);
    titleLabel->setText(titleLabel->text() + tr(" (%1 KBytes)").arg(value/1024));
}

void DownloaderProgress::setValue(int value)
{
    progress->setValue(value);
    int range = progress->maximum() - progress->minimum();
    if (value == 0 || range == 0)
        return;
    // only update rate when difference >= 1% 
    int diff = (value - lastValue) * 100 / range; 
    if (diff == 0)
        return; 
    QDateTime now = QDateTime::currentDateTime();
    int seconds = now.toTime_t() - initTime.toTime_t();
    if (seconds == 0)
        return;
    int speed = value/seconds;
    speedLabel->setText(tr("Download rate: %1 KBytes/s").arg(speed/1024));
    lastValue = value;
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

DownloaderProgress &DownloaderProgress::getInstance()
{
    static DownloaderProgress instance;
    return instance;
}
