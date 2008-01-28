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
: GenericProgress(parent)
{
    QVBoxLayout *mainLayout = new QVBoxLayout;

    m_titleLabel = new QLabel;
    m_titleLabel->setTextFormat( Qt::PlainText );
    mainLayout->addWidget(m_titleLabel);

    QHBoxLayout *statusLayout = new QHBoxLayout;
    mainLayout->addLayout(statusLayout);

    m_statusLabel = new QLabel;
    statusLayout->addWidget(m_statusLabel);

    m_progress = new QListWidget;
    statusLayout->addWidget(m_progress);

    setLayout(mainLayout);
    hide();
}

InstallerProgress::~InstallerProgress()
{}

void InstallerProgress::setTitle(const QString &label)
{
    if (!Settings::getInstance().installDetails())
    {
        m_titleLabel->setText(label);
    }
    else 
    {
        static int i = 0;
        m_progress->addItem(label);
        if (i++ % 10 == 0) {
            m_progress->scrollToBottom();
        }
    }
}

void InstallerProgress::show()
{
    if (Settings::getInstance().installDetails())
    {
        m_statusLabel->show();
        m_progress->show();
    } else {
        m_statusLabel->hide();
        m_progress->hide();
    }
    GenericProgress::show();
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



