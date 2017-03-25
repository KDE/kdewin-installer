/****************************************************************************
**
** Copyright (C) 2006-2009 Ralf Habacker <ralf.habacker@freenet.de>
**  All rights reserved.
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
