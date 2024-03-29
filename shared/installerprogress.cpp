/****************************************************************************
**
** Copyright (C) 2006-2007 Ralf Habacker. All rights reserved.
**
** This file is part of the KDE installer for windows
**
** SPDX-License-Identifier: LGPL-2.0-only
**
****************************************************************************/

#include "installerprogress.h"
#include "settings.h"

#include <QDebug>

#ifdef USE_GUI
#include <QLabel>
#include <QProgressBar>
#include <QHBoxLayout>
#include <QVBoxLayout>
#include <QListWidget>

InstallerProgress::InstallerProgress(QWidget *parent,bool showProgress)
: GenericProgress(parent), m_progress(0)
{
    QVBoxLayout *mainLayout = new QVBoxLayout(this);

    m_titleLabel = new QLabel;
    m_titleLabel->setTextFormat( Qt::PlainText );
    mainLayout->addWidget(m_titleLabel);

    if (showProgress) 
    {
        m_progress = new QProgressBar;
        mainLayout->addWidget(m_progress);
    }

    m_fileNameLabel = new QLabel;
    m_fileNameLabel->setTextFormat( Qt::PlainText );
    mainLayout->addWidget(m_fileNameLabel);

    hide();
}

InstallerProgress::~InstallerProgress()
{}

void InstallerProgress::setTitle(const QString &label)
{
    m_titleLabel->setText(label);
}

void InstallerProgress::setPackageName(const QString &packageName)
{
    m_packageName = packageName;
    updateDisplay();
}

void InstallerProgress::setFileName(const QString &fileName)
{
    m_fileName = fileName;
    updateDisplay();
}

void InstallerProgress::setPackageCount(int value)
{
    if (!m_progress)
        return;
    m_progress->setMaximum(value);    
    setPackageNumber(0);    
}

void InstallerProgress::setPackageNumber(int value)
{
    if (!m_progress)
        return;
    m_progress->setValue(value);    
    updateDisplay();
}

void InstallerProgress::updateDisplay()
{
    if (!m_progress)
        return;
    m_titleLabel->setText(tr("Installing package %1 of %2 : %3").arg(m_progress->value()+1).arg(m_progress->maximum()).arg(m_packageName));
    m_fileNameLabel->setText(tr("File %1").arg(m_fileName));
}

void InstallerProgress::show()
{
    if (m_progress) 
    {
        if (m_progress->maximum() > 1) 
        {
            m_progress->show();
        }
        else
        {
            m_progress->hide();
        }
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

void InstallerProgress::setPackageName(const QString &packageName)
{
    QString a = QObject::tr("Unpacking %1").arg(packageName);
    printf(a.toLatin1().data());
}

void InstallerProgress::setFileName(const QString &fileName)
{
    QString a = QObject::tr("File %1").arg(fileName);
    printf(a.toLatin1().data());
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



