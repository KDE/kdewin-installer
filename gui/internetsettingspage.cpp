/****************************************************************************
**
** Copyright (C) 2008 Ralf Habacker <ralf.habacker@freenet.de> 
** All rights reserved.
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

#include "internetsettingspage.h"


InternetSettingsPage::InternetSettingsPage() : InstallWizardPage(0)
{
    ui.setupUi(this);
    setTitle(windowTitle());
    setSubTitle(statusTip());

    connect( ui.proxyManual,SIGNAL(clicked(bool)),this,SLOT(switchProxyFields(bool)) );
    connect( ui.proxyFireFox,SIGNAL(clicked(bool)),this,SLOT(switchProxyFields(bool)) );
    connect( ui.proxyIE,SIGNAL(clicked(bool)),this,SLOT(switchProxyFields(bool)) );
    connect( ui.proxyOff,SIGNAL(clicked(bool)),this,SLOT(switchProxyFields(bool)) );
    
}

void InternetSettingsPage::initializePage()
{
    Settings &s = Settings::instance();
#ifndef Q_WS_WIN
    ui.proxyIE->setText(tr("Environment settings"));
#endif
    ProxySettings proxy;
    switch (s.proxyMode()) {
        case Settings::InternetExplorer: 
            ui.proxyIE->setChecked(true); 
            break;
        case Settings::Manual: 
            ui.proxyManual->setChecked(true); 
            break;
        case Settings::FireFox: 
            ui.proxyFireFox->setChecked(true); 
            break;
        case Settings::Environment: 
            ui.proxyIE->setChecked(true); 
            break;
        case Settings::None:
        default: 
            ui.proxyOff->setChecked(true); 
            break;
    }
    s.proxy(s.proxyMode(),"",proxy);

    ui.proxyHost->setText(proxy.hostname);
    ui.proxyPort->setText(proxy.port ? QString("%1").arg(proxy.port) : QString());
    ui.proxyUserName->setText(proxy.user);
    ui.proxyPassword->setText(proxy.password);

    ui.proxyHost->setEnabled(ui.proxyManual->isChecked());
    ui.proxyPort->setEnabled(ui.proxyManual->isChecked());
    ui.proxyUserName->setEnabled(ui.proxyManual->isChecked());
    ui.proxyPassword->setEnabled(ui.proxyManual->isChecked());
}

bool InternetSettingsPage::validatePage()
{
    Settings &s = Settings::instance();
    Settings::ProxyMode m = Settings::None;
    if(ui.proxyIE->isChecked())
#ifdef Q_WS_WIN
      m = Settings::InternetExplorer;
#else
      m = Settings::Environment;
#endif
    if(ui.proxyFireFox->isChecked())
        m = Settings::FireFox;
    if(ui.proxyManual->isChecked())
        m = Settings::Manual;
    s.setProxyMode(m);
    if (ui.proxyManual->isChecked())
    {
        ProxySettings proxy;
        proxy.hostname = ui.proxyHost->text();
        proxy.port = ui.proxyPort->text().toInt();
        proxy.user = ui.proxyUserName->text();
        proxy.password = ui.proxyPassword->text();
        s.setProxy(proxy);
    }
    return true;
}

bool InternetSettingsPage::isComplete()
{
    return true;
}

void InternetSettingsPage::switchProxyFields(bool mode)
{
    mode = ui.proxyManual->isChecked() ? true : false;
       
    ui.proxyHost->setEnabled(mode);
    ui.proxyPort->setEnabled(mode);
    ui.proxyUserName->setEnabled(mode);
    ui.proxyPassword->setEnabled(mode);
        
    Settings &s = Settings::instance();
    ProxySettings proxy;
    if(ui.proxyIE->isChecked())
#ifdef Q_WS_WIN
        s.proxy(Settings::InternetExplorer,"",proxy);
#else
        s.proxy(Settings::Environment,"",proxy);
#endif
    else if(ui.proxyFireFox->isChecked())
        s.proxy(Settings::FireFox,"",proxy);
    else if(ui.proxyManual->isChecked())
        s.proxy(Settings::Manual,"",proxy);
    else
        s.proxy(Settings::None,"",proxy);
        
    ui.proxyHost->setText(proxy.hostname);
    ui.proxyPort->setText(proxy.port ? QString("%1").arg(proxy.port) : QString());
    ui.proxyUserName->setText(proxy.user);
    ui.proxyPassword->setText(proxy.password);

}

