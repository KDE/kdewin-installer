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

#include <QHBoxLayout>
#include <QPushButton>
#include <QCloseEvent>

#include "settings.h"
#include "complexwizard.h"
#include "downloaderprogress.h"
#include "installerprogress.h"

ComplexWizard::ComplexWizard(QWidget *parent)
        : QDialog(parent)
{
    setWindowFlags(Qt::WindowMinimizeButtonHint | Qt::WindowMaximizeButtonHint);
    settingsButton = new QPushButton(tr("Settings"));
    cancelButton = new QPushButton(tr("Cancel"));
    backButton = new QPushButton(tr("< &Back"));
    nextButton = new QPushButton(tr("Next >"));
    finishButton = new QPushButton(tr("&Finish"));
    progressBar = new DownloaderProgress(this);
    instProgressBar = new InstallerProgress(this);

    connect(cancelButton, SIGNAL(clicked()), this, SLOT(cancelButtonClicked()));
    connect(backButton, SIGNAL(clicked()), this, SLOT(backButtonClicked()));
    connect(nextButton, SIGNAL(clicked()), this, SLOT(nextButtonClicked()));
    connect(finishButton, SIGNAL(clicked()), this, SLOT(accept()));
    connect(settingsButton, SIGNAL(clicked()), this, SLOT(settingsButtonClicked()));


    buttonLayout = new QHBoxLayout;
    buttonLayout->addStretch(1);
    buttonLayout->addWidget(settingsButton);
    buttonLayout->addWidget(cancelButton);
    buttonLayout->addWidget(backButton);
    buttonLayout->addWidget(nextButton);
    buttonLayout->addWidget(finishButton);

    mainLayout = new QVBoxLayout;
    mainLayout->addWidget(progressBar);
    mainLayout->addLayout(buttonLayout);
    setLayout(mainLayout);

    setSizeGripEnabled(true);

    readSettings();
}

void ComplexWizard::closeEvent(QCloseEvent *event)
{
    cancelButtonClicked();
}

void ComplexWizard::writeSettings()
{
    Settings &settings = Settings::getInstance();

    settings.beginGroup("Geometry");
    settings.setValue("size", size());
    settings.setValue("pos", pos());
    settings.endGroup();
}

void ComplexWizard::readSettings()
{
    Settings &settings = Settings::getInstance();

    settings.beginGroup("Geometry");
    resize(settings.value("size", QSize(400, 400)).toSize());
    move(settings.value("pos", QPoint(200, 200)).toPoint());
    settings.endGroup();
}

void ComplexWizard::setFirstPage(WizardPage *page)
{
    page->resetPage();
    history.append(page);
    switchPage(0);
}

void ComplexWizard::backButtonClicked()
{
    WizardPage *oldPage = history.takeLast();
    oldPage->resetPage();
    switchPage(oldPage);
}

void ComplexWizard::nextButtonClicked()
{
    backButton->setEnabled(false);
    nextButton->setEnabled(false);
    finishButton->setDefault(false);

    WizardPage *oldPage = history.last();
    WizardPage *newPage = oldPage->nextPage();
    newPage->resetPage();
    history.append(newPage);

    switchPage(oldPage);
}

void ComplexWizard::completeStateChanged()
{
    WizardPage *currentPage = history.last();
    if (currentPage->isLastPage())
        finishButton->setEnabled(currentPage->isComplete());
    else
        nextButton->setEnabled(currentPage->isComplete());
}

void ComplexWizard::switchPage(WizardPage *oldPage)
{
    if (oldPage)
    {
        oldPage->hide();
        mainLayout->removeWidget(oldPage);
        disconnect(oldPage, SIGNAL(completeStateChanged()),
                   this, SLOT(completeStateChanged()));
    }

    WizardPage *newPage = history.last();
    mainLayout->insertWidget(0, newPage);
    newPage->show();
    newPage->setFocus();
    connect(newPage, SIGNAL(completeStateChanged()),
            this, SLOT(completeStateChanged()));

    backButton->setEnabled(history.size() != 1);
    if (newPage->isLastPage())
    {
        nextButton->setEnabled(false);
        finishButton->setEnabled(true);
        finishButton->setDefault(true);
    }
    else
    {
        nextButton->setDefault(true);
        nextButton->setEnabled(true);
        finishButton->setEnabled(false);
    }
    completeStateChanged();
}

void ComplexWizard::cancelButtonClicked()
{
    history.last()->reject();
    writeSettings();
    reject();
}

void ComplexWizard::settingsButtonClicked()
{
    history.last()->reject();
    reject();
}

#include "complexwizard.moc"
