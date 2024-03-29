/****************************************************************************
**
** Copyright (C) 2008 Ralf Habacker <ralf.habacker@freenet.de> 
** All rights reserved.
**
** This file is part of the KDE installer for windows
**
** SPDX-License-Identifier: GPL-2.0-only
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
#ifndef Q_OS_WIN32
    ui.proxyIE->setText(tr("Environment settings"));
#endif
    ProxySettings proxy;
    switch (s.proxyMode()) {
        case ProxySettings::InternetExplorer:
            ui.proxyIE->setChecked(true); 
            break;
        case ProxySettings::Manual:
            ui.proxyManual->setChecked(true); 
            break;
        case ProxySettings::FireFox:
            ui.proxyFireFox->setChecked(true); 
            break;
        case ProxySettings::Environment:
            ui.proxyIE->setChecked(true); 
            break;
        case ProxySettings::None:
        default: 
            ui.proxyOff->setChecked(true); 
            break;
    }
    proxy.from(s.proxyMode());

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
    ProxySettings::ProxyMode m = ProxySettings::None;
    if(ui.proxyIE->isChecked())
#ifdef Q_OS_WIN32
      m = ProxySettings::InternetExplorer;
#else
      m = ProxySettings::Environment;
#endif
    if(ui.proxyFireFox->isChecked())
        m = ProxySettings::FireFox;
    if(ui.proxyManual->isChecked())
        m = ProxySettings::Manual;
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
#ifdef Q_OS_WIN32
        proxy.from(ProxySettings::InternetExplorer);
#else
        proxy.from(ProxySettings::Environment);
#endif
    else if(ui.proxyFireFox->isChecked())
        proxy.from(ProxySettings::FireFox);
    else if(ui.proxyManual->isChecked())
        proxy.from(ProxySettings::Manual);
    else
        proxy.from(ProxySettings::None);
        
    ui.proxyHost->setText(proxy.hostname);
    ui.proxyPort->setText(proxy.port ? QString("%1").arg(proxy.port) : QString());
    ui.proxyUserName->setText(proxy.user);
    ui.proxyPassword->setText(proxy.password);

}

