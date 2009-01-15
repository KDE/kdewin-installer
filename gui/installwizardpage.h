/****************************************************************************
**
** Copyright (C) 2005-2009 Ralf Habacker. All rights reserved.
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

#ifndef INSTALLWIZARDPAGE_H
#define INSTALLWIZARDPAGE_H

#include <QObject>
#include <QWizard>

class QLabel;

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
