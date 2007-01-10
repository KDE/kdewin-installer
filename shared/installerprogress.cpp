/****************************************************************************
**
** Copyright (C) 2006 Ralf Habacker. All rights reserved.
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
#include <QListWidget>
#include <QApplication>

#include "installerprogress.h"

#ifdef USE_GUI
InstallerProgress::InstallerProgress(QWidget *parent)
{
    statusLabel = new QLabel();
//    progress = new QProgressBar(parent);
    progress = new QListWidget(parent);

    QHBoxLayout *statusLayout = new QHBoxLayout;
    statusLayout->addWidget(statusLabel);
    statusLayout->addWidget(progress);

    titleLabel = new QLabel();
    QVBoxLayout *mainLayout = new QVBoxLayout;
    mainLayout->addWidget(titleLabel);
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
    //titleLabel->setText(label);
    progress->addItem(label);
    if (i++ % 10 == 0) {
        progress->scrollToBottom();
        QApplication::instance()->processEvents();
    }
}

void InstallerProgress::setStatus(const QString &label)
{
    statusLabel->setText(label);
}

void InstallerProgress::show()
{
    titleLabel->show();
    statusLabel->show();
    progress->show();
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
    qDebug() << title;
}

void InstallerProgress::setStatus(const QString &status)
{
    qDebug() << status;
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



