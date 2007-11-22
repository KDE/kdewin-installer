/****************************************************************************
**
** Copyright (C) 2004-2005 Trolltech AS. All rights reserved.
**
** This file is part of the example classes of the Qt Toolkit.
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

#ifndef COMPLEXWIZARD_H
#define COMPLEXWIZARD_H

#include <QDialog>
#include <QList>
#include "downloader.h"

class QHBoxLayout;
class QPushButton;
class QVBoxLayout;
class WizardPage;
class QTreeWidgetItem;
class DownloaderProgress;
class InstallerProgress;

class ComplexWizard : public QDialog
{
    Q_OBJECT

public:
    ComplexWizard(QWidget *parent = 0);
    QPushButton *aboutButton;
    QPushButton *settingsButton;

    const QList<WizardPage *> &historyPages() const
    {
        return history;
    }
    InstallerProgress &installProgressBar() { return *instProgressBar; }

protected:
    void setFirstPage(WizardPage *page);
    DownloaderProgress *progressBar;
    InstallerProgress *instProgressBar;
    QPushButton *nextButton;
    QPushButton *backButton;
    QPushButton *cancelButton;
    QPushButton *AboutButton;
    QPushButton *finishButton;

protected slots:
    void backButtonClicked();
    void nextButtonClicked();
    void completeStateChanged();
    void cancelButtonClicked();
    virtual void settingsButtonClicked();
    virtual void aboutButtonClicked();
private:
    void switchPage(WizardPage *oldPage);

    QList<WizardPage *> history;
    QHBoxLayout *buttonLayout;
    QVBoxLayout *mainLayout;

    void writeSettings();
    void readSettings();
    void closeEvent(QCloseEvent *event);
    WizardPage *currentPage;

};

class WizardPage : public QWidget
{
    Q_OBJECT

public:
    WizardPage(QWidget *parent = 0)
        : QWidget(parent)
    {
        hide();
    }

    virtual void resetPage() = 0;
    virtual WizardPage *nextPage() = 0;
    virtual bool isLastPage() { return false; }
    virtual bool isComplete() { return true; }
    virtual void reject() {};

signals:
    void completeStateChanged();
};

#endif
