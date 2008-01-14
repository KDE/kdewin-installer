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

#include "config.h"

#include <QHBoxLayout>
#include <QPushButton>
#include <QCloseEvent>
#include <QMessageBox>
#include <QApplication>

#include "settings.h"
#include "complexwizard.h"
#include "downloaderprogress.h"
#include "installerprogress.h"

ComplexWizard::ComplexWizard(QWidget *parent)
        : QDialog(parent)
{
    setWindowFlags(Qt::WindowMinimizeButtonHint | Qt::WindowMaximizeButtonHint);
    aboutButton = new QPushButton(tr("About"));
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
    connect(aboutButton, SIGNAL(clicked()), this, SLOT(aboutButtonClicked()));


    buttonLayout = new QHBoxLayout;
    buttonLayout->addWidget(aboutButton);
    buttonLayout->addStretch(1);
    buttonLayout->addWidget(settingsButton);
    buttonLayout->addStretch(1);
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
    event->accept();
}

void ComplexWizard::writeSettings()
{
    Settings &settings = Settings::getInstance();

    settings.beginGroup("Geometry");
    settings.setValue("normalGeometry", normalGeometry());
    settings.setValue("maximized", isMaximized());
    settings.endGroup();
}

void ComplexWizard::readSettings()
{
    Settings &settings = Settings::getInstance();

    settings.beginGroup("Geometry");
    setGeometry(settings.value("normalGeometry", QRect(200,200, 400, 400)).toRect());
    if (settings.value("maximized", false).toBool()) {
      setWindowState(Qt::WindowMaximized);
    }

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
        nextButton->setVisible(false);
        finishButton->setEnabled(true);
        finishButton->setDefault(true);
    }
    else
    {
        nextButton->setDefault(true);
        nextButton->setVisible(true);
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
    qApp->closeAllWindows();
    qApp->exit(0);
}

void ComplexWizard::settingsButtonClicked()
{
    // @TODO really required  ?
    history.last()->reject();
    reject();
}


void ComplexWizard::aboutButtonClicked()
{
    QMessageBox::information(this,
        tr("KDE-Installer"),
        tr("The KDEWIN Installer is an open source application, "
           "which makes it able to install KDE applications on windows.\n"
           "\nAuthors: \n\n\tRalf Habacker\n\tChristian Ehrlicher\n"
           "\n\n\nbuild with Qt version " QTVERSION
          ),
          QMessageBox::Ok
    );
}

#include "complexwizard.moc"
