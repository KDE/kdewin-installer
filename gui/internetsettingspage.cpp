
#include "internetsettingspage.h"


InternetSettingsPage::InternetSettingsPage() : InstallWizardPage(0)
{
    ui.setupUi(this);
    setTitle(windowTitle());
    setSubTitle(statusTip());

    QVBoxLayout *layout = new QVBoxLayout;
    layout->addWidget(statusLabel,1,Qt::AlignBottom);
    setLayout(layout);
}

void InternetSettingsPage::initializePage()
{
    Settings &s = Settings::getInstance();
#ifndef Q_WS_WIN
    ui.proxyIE->setText(tr("Environment settings"));
#endif
    switch (s.proxyMode()) {
        case Settings::InternetExplorer: ui.proxyIE->setChecked(true); break;
        case Settings::Manual: ui.proxyManual->setChecked(true); break;
        case Settings::FireFox: ui.proxyFireFox->setChecked(true); break;
        case Settings::Environment: ui.proxyIE->setChecked(true); break;
        case Settings::None:
        default: ui.proxyOff->setChecked(true); break;
    }

    Settings::proxySettings proxy;
    if (s.proxy("",proxy))
    {
        ui.proxyHost->setText(proxy.hostname);
        ui.proxyPort->setText(QString("%1").arg(proxy.port));
        ui.proxyUserName->setText(proxy.user);
        ui.proxyPassword->setText(proxy.password);
        ui.proxyHost->setEnabled(ui.proxyManual->isChecked());
        ui.proxyPort->setEnabled(ui.proxyManual->isChecked());
        ui.proxyUserName->setEnabled(ui.proxyManual->isChecked());
        ui.proxyPassword->setEnabled(ui.proxyManual->isChecked());
    }
}

int InternetSettingsPage::nextId() const
{
    return InstallWizard::downloadSettingsPage;
}

bool InternetSettingsPage::validatePage()
{
    Settings &s = Settings::getInstance();
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
        Settings::proxySettings proxy;
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

#include "internetsettingspage.moc"
