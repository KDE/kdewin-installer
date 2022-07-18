/****************************************************************************
**
** Copyright (C) 2006-2009 Ralf Habacker <ralf.habacker@freenet.de>
**  All rights reserved.
**
** This file is part of the KDE installer for windows
**
** SPDX-License-Identifier: GPL-2.0-only
****************************************************************************/

#include "installwizardpage.h"

#include <QLabel>
#include <QtDebug>
#include <QTimer>

InstallWizardPage::InstallWizardPage(QWidget *parent) : QWizardPage(parent)
{
    statusLabel = new QLabel("", this);
}

void InstallWizardPage::initializePage()
{
}

int InstallWizardPage::nextId() const
{
    return QWizardPage::nextId();
}

bool InstallWizardPage::isComplete()
{
    return QWizardPage::isComplete();
}

void InstallWizardPage::cancel()
{
}

void InstallWizardPage::setStatus(const QString &text)
{
    statusLabel->setText(text);
    QTimer::singleShot(5000, this, SLOT(slotClearStatus()));
}

void InstallWizardPage::slotClearStatus()
{
    statusLabel->setText(QString());
}

void InstallWizardPage::setSettingsButtonVisible(bool mode)
{
#if 0
    if (mode)
        wizard()->button(QWizard::CustomButton2)->show();
    else
        wizard()->button(QWizard::CustomButton2)->hide();
#endif
}

void InstallWizardPage::performAction()
{
}
