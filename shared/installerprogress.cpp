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

#include "installerprogress.h"
#include "settings.h"

#ifdef USE_GUI
InstallerProgress::InstallerProgress(QWidget *parent)
{
    QVBoxLayout *mainLayout = new QVBoxLayout;

    titleLabel = new QLabel;
    mainLayout->addWidget(titleLabel);

    QHBoxLayout *statusLayout = new QHBoxLayout;
    mainLayout->addLayout(statusLayout);

    statusLabel = new QLabel;
    statusLayout->addWidget(statusLabel);
    progress = new QListWidget;
    statusLayout->addWidget(progress);

    setLayout(mainLayout);
    hide();
}

InstallerProgress::~InstallerProgress()
{}

void InstallerProgress::hide()
{
    titleLabel->hide();
    statusLabel->hide();
    progress->hide();
    QWidget::hide();
}

void InstallerProgress::setTitle(const QString &label)
{
    if (!Settings::getInstance().installDetails())
    {
        titleLabel->setText(label);
    }
    else 
    {
        static int i = 0;
        progress->addItem(label);
        if (i++ % 10 == 0) {
            progress->scrollToBottom();
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
    QWidget::show();
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



