/****************************************************************************
**
** Copyright (C) 2005-2009 Ralf Habacker. All rights reserved.
**
** This file is part of the KDE installer for windows
**
** SPDX-License-Identifier: GPL-2.0-only
****************************************************************************/

#ifndef INSTALLWIZARDPAGE_H
#define INSTALLWIZARDPAGE_H

#include <QObject>
#include <QWizard>

class QLabel;

#include <QAbstractButton>
#include <QWizardPage>

class InstallWizardPage : public QWizardPage
{
    Q_OBJECT 
    
public:
    InstallWizardPage(QWidget *parent = 0);

    /// setup page before it is displayed
    virtual void initializePage();
    /// return next page id, called when pressed next (not used) 
    virtual int nextId() const;
    /// check if page is complete
    virtual bool isComplete();
    /// cancel wizard 
    virtual void cancel();
    /// enable/disable settings button (deprecated )
    void setSettingsButtonVisible(bool mode);

public Q_SLOTS:
    /// set status label
    void setStatus(const QString &text);
    /// slot for performing page action after page is displayed
    virtual void performAction();

protected:
    QLabel *topLabel;
    QLabel *statusLabel;

protected slots:
    void slotClearStatus();
};

#endif
