#include <QtDebug>
#include <QFileDialog>

#include "settings.h"
#include "settingspage.h"

 
SettingsPage::SettingsPage(QWidget *parent)
     : QDialog(parent)
{
    ui.setupUi(this);
    connect( ui.rootPathSelect,SIGNAL(clicked()),this,SLOT(rootPathSelectClicked()) );
    connect( ui.tempPathSelect,SIGNAL(clicked()),this,SLOT(tempPathSelectClicked()) );

    connect( ui.proxyManual,SIGNAL(clicked(bool)),this,SLOT(switchProxyFields(bool)) );
    connect( ui.proxyFireFox,SIGNAL(clicked(bool)),this,SLOT(switchProxyFields(bool)) );
    connect( ui.proxyIE,SIGNAL(clicked(bool)),this,SLOT(switchProxyFields(bool)) );
    connect( ui.proxyOff,SIGNAL(clicked(bool)),this,SLOT(switchProxyFields(bool)) );
    init();
}

void SettingsPage::init()
{
    Settings &settings = Settings::getInstance();
    ui.createStartMenuEntries->setEnabled(false);
    ui.displayTitlePage->setCheckState(settings.showTitlePage() ? Qt::Checked : Qt::Unchecked);
    ui.displayTitlePage->setCheckState(settings.createStartMenuEntries() ? Qt::Checked : Qt::Unchecked);
    ui.nestedDownloadTree->setCheckState(settings.nestedDownloadTree() ? Qt::Checked : Qt::Unchecked);
    ui.installDetails->setCheckState(settings.installDetails() ? Qt::Checked : Qt::Unchecked);
    ui.autoNextStep->setCheckState(settings.autoNextStep() ? Qt::Checked : Qt::Unchecked);

    ui.rootPathEdit->setText(QDir::convertSeparators(settings.installDir()));
    ui.tempPathEdit->setText(QDir::convertSeparators(settings.downloadDir()));
    switch (settings.proxyMode()) {
        case Settings::InternetExplorer: ui.proxyIE->setChecked(true); break;
        case Settings::Manual: ui.proxyManual->setChecked(true); break;
        case Settings::FireFox: ui.proxyFireFox->setChecked(true); break;
        case Settings::None: 
        default: ui.proxyOff->setChecked(true); break;
    }

    ui.proxyHost->setText(settings.proxyHost());
    ui.proxyPort->setText(QString("%1").arg(settings.proxyPort()));
    switchProxyFields(true);
}

void SettingsPage::accept()
{
    hide();
    Settings &settings = Settings::getInstance();

    settings.setCreateStartMenuEntries(ui.createStartMenuEntries->checkState() == Qt::Checked ? true : false);
    settings.setShowTitlePage(ui.displayTitlePage->checkState() == Qt::Checked ? true : false);
    settings.setNestedDownloadTree(ui.nestedDownloadTree->checkState() == Qt::Checked ? true : false);
    settings.setInstallDetails(ui.installDetails->checkState() == Qt::Checked ? true : false);
    settings.setAutoNextStep(ui.autoNextStep->checkState() == Qt::Checked ? true : false);

    settings.setInstallDir(ui.rootPathEdit->text());
    settings.setDownloadDir(ui.tempPathEdit->text());
    Settings::ProxyMode m = Settings::None;
    if(ui.proxyIE->isChecked())
        m = Settings::InternetExplorer;
    if(ui.proxyFireFox->isChecked())
        m = Settings::FireFox;
    if(ui.proxyManual->isChecked())
        m = Settings::Manual;
    settings.setProxyMode(m);
    if (ui.proxyManual->isChecked())
        settings.setProxy(ui.proxyHost->text(),ui.proxyPort->text());
}

void SettingsPage::reject()
{
    hide();
    init(); // reinit page to restore old settings
}

void SettingsPage::switchProxyFields(bool checked)
{
    ui.proxyHost->setEnabled(ui.proxyManual->isChecked());
    ui.proxyPort->setEnabled(ui.proxyManual->isChecked());
}

void SettingsPage::rootPathSelectClicked()
{
    QString fileName = QFileDialog::getExistingDirectory(this,
                       tr("Select Root Installation Directory"),
                       "",
                       QFileDialog::ShowDirsOnly| QFileDialog::DontResolveSymlinks);
    if(!fileName.isEmpty())
        ui.rootPathEdit->setText(QDir::toNativeSeparators(fileName));
}
 
void SettingsPage::tempPathSelectClicked()
{
    QString fileName = QFileDialog::getExistingDirectory(this,
                       tr("Select Package Download Directory"),
                       "",
                       QFileDialog::ShowDirsOnly| QFileDialog::DontResolveSymlinks);
    if(!fileName.isEmpty())
        ui.tempPathEdit->setText(QDir::toNativeSeparators(fileName));
}



#if test
int main(int argc, char **argv)
{
    QApplication app(arc, argv);

    SettingsPage settingsPage; 
    
    settingsPage.show();
    app->exec();
}
    
#endif

#include "settingspage.moc"
