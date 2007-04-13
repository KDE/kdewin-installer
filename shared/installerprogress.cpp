/****************************************************************************
**
** Copyright (C) 2006-2007 Ralf Habacker. All rights reserved.
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

#include <QDebug>
#include <QLabel>
#include <QProgressBar>
#include <QHBoxLayout>
#include <QVBoxLayout>
#include <QListWidget>
#include <QApplication>

#include "installerprogress.h"
#include "settings.h"

#ifdef USE_GUI
InstallerProgress::InstallerProgress(QWidget *parent)
{
    titleLabel = new QLabel();
    QVBoxLayout *mainLayout = new QVBoxLayout;
    mainLayout->addWidget(titleLabel);
    progress = new QListWidget(parent);
    QHBoxLayout *statusLayout = new QHBoxLayout;
    statusLabel = new QLabel();
    statusLayout->addWidget(statusLabel);
    statusLayout->addWidget(progress);
    mainLayout->addLayout(statusLayout);
    setLayout(mainLayout);
    hide();
}

InstallerProgress::~InstallerProgress()
{
    delete titleLabel;
    delete statusLabel;
    delete progress;
}

void InstallerProgress::hide()
{
    titleLabel->hide();
    statusLabel->hide();
    progress->hide();
}

void InstallerProgress::setTitle(const QString &label)
{
    static int i = 0;
    if (!Settings::getInstance().installDetails())
    {
        titleLabel->setText(label);
        QApplication::instance()->processEvents();
    }
    else 
    {
        progress->addItem(label);
        if (i++ % 10 == 0) {
            progress->scrollToBottom();
            QApplication::instance()->processEvents();
        }
    }
}

void InstallerProgress::setStatus(const QString &label)
{
    if (statusLabel)
        statusLabel->setText(label);
}

void InstallerProgress::show()
{
    titleLabel->show();
    if (Settings::getInstance().installDetails())
    {
        statusLabel->show();
        progress->show();
    }
}

void InstallerProgress::setMaximum(int value)
{
    //progress->setMaximum(value);
}

void InstallerProgress::setValue(int value)
{
    //progress->setValue(value);
}

#else // console implementation

InstallerProgress::InstallerProgress(QObject *parent)
{
    hide();
}

void InstallerProgress::hide()
{
    visible = false;
}

void InstallerProgress::setTitle(const QString &title)
{
#ifdef DEBUG
    qDebug() << title;
#endif
}

void InstallerProgress::setStatus(const QString &status)
{
#ifdef DEBUG
    qDebug() << status;
#endif
}

void InstallerProgress::setMaximum(int value)
{}

void InstallerProgress::setValue(int value)
{
    int unit = value/10240;
    if (oldunit != value/10240)
    {
        if (visible)
            putchar('.');
        oldunit = unit;
    }
}

void InstallerProgress::show()
{
    visible = true;
}

#endif



