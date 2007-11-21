#include <QtDebug>
#include <QFileDialog>
#include <QInputDialog>

#include "settings.h"
#include "settingspage.h"

QWidget *SettingsDownloadPage::widget()
{
    ui.downloadGroupBox->setFlat(true);
    ui.downloadGroupBox->setTitle("");
    ui.downloadMirror->setVisible(false);
    ui.downloadMirrorLabel->setVisible(false);
    return ui.downloadGroupBox;
}

void SettingsDownloadPage::reset()
{
#if 0
    ui.downloadMirror->setEditText(QString("%1").arg(s.mirror()));

    if (m_globalConfig && ui.downloadMirror->count() == 0) 
    {
        int currentIndex = 0;
        if (m_globalConfig->mirrors()->size() > 0) 
        {
            QList<GlobalConfig::Mirror*>::iterator p = m_globalConfig->mirrors()->begin();
            for (int i = 0; p != m_globalConfig->mirrors()->end(); p++, i++)
            {
                mirrorList << ((*p)->url);
                if (s.mirror() == (*p)->url)
                    currentIndex = i;
            }
        }
        else if (!s.mirror().isEmpty())
            mirrorList << s.mirror();

        mirrorList << s.localMirrors();

        rebuildMirrorList(0);
    }
#endif
}

void SettingsDownloadPage::accept()
{
    s.setDownloadDir(ui.tempPathEdit->text());
    if (!ui.downloadMirror->currentText().isEmpty() 
            && s.mirror() != ui.downloadMirror->currentText())
        s.setMirror(ui.downloadMirror->currentText());
}

void SettingsDownloadPage::reject()
{
}

bool SettingsDownloadPage::isComplete()
{
    return !ui.rootPathEdit->text().isEmpty();
}

QWidget *SettingsInstallPage::widget()
{
    ui.installGroupBox->setFlat(true);
    ui.installGroupBox->setTitle("");
    ui.installWarningLabel->setVisible(false);
    return ui.installGroupBox;
}
void SettingsInstallPage::reset()
{
    ui.createStartMenuEntries->setEnabled(false);
    ui.installModeEndUser->setChecked(!s.isDeveloperMode() ? Qt::Checked : Qt::Unchecked);
    ui.installModeDeveloper->setChecked(s.isDeveloperMode() ? Qt::Checked : Qt::Unchecked);
    ui.rootPathEdit->setText(QDir::convertSeparators(s.installDir()));
    ui.tempPathEdit->setText(QDir::convertSeparators(s.downloadDir()));

    switch (s.compilerType()) {
		case Settings::unspecified: ui.compilerUnspecified->setChecked(true); break;
		case Settings::MinGW: ui.compilerMinGW->setChecked(true); break;
		case Settings::MSVC: ui.compilerMSVC->setChecked(true); break;
        default: ui.compilerUnspecified->setChecked(true); break;
	}
    if (s.isDeveloperMode())
        ui.installModeDeveloper->setChecked(true); 
    else 
        ui.installModeEndUser->setChecked(true);
}

void SettingsInstallPage::accept()
{
    s.setCreateStartMenuEntries(ui.createStartMenuEntries->checkState() == Qt::Checked ? true : false);
    s.setInstallDir(ui.rootPathEdit->text());

    s.setDeveloperMode(ui.installModeDeveloper->isChecked());

    if (ui.compilerUnspecified->isChecked())
		s.setCompilerType(Settings::unspecified);
	if (ui.compilerMinGW->isChecked())
		s.setCompilerType(Settings::MinGW);
	if (ui.compilerMSVC->isChecked())
		s.setCompilerType(Settings::MSVC);
}

void SettingsInstallPage::reject()
{
}

bool SettingsInstallPage::isComplete()
{
    return !ui.rootPathEdit->text().isEmpty();
}

QWidget *SettingsProxyPage::widget()
{   
    ui.proxyGroupBox->setFlat(true);
    ui.proxyGroupBox->setTitle("");
    return ui.proxyGroupBox;
}

void SettingsProxyPage::reset()
{
	switch (s.proxyMode()) {
        case Settings::InternetExplorer: ui.proxyIE->setChecked(true); break;
        case Settings::Manual: ui.proxyManual->setChecked(true); break;
        case Settings::FireFox: ui.proxyFireFox->setChecked(true); break;
        case Settings::None: 
        default: ui.proxyOff->setChecked(true); break;
    }

    QNetworkProxy proxy; 
    if (s.proxy("",proxy))
    {
        ui.proxyHost->setText(proxy.hostName());
        ui.proxyPort->setText(QString("%1").arg(proxy.port()));
        ui.proxyUserName->setText(proxy.user());
        ui.proxyPassword->setText(proxy.password());
        ui.proxyHost->setEnabled(ui.proxyManual->isChecked());
        ui.proxyPort->setEnabled(ui.proxyManual->isChecked());
        ui.proxyUserName->setEnabled(ui.proxyManual->isChecked());
        ui.proxyPassword->setEnabled(ui.proxyManual->isChecked());
    }
}

void SettingsProxyPage::accept()
{
    Settings::ProxyMode m = Settings::None;
    if(ui.proxyIE->isChecked())
        m = Settings::InternetExplorer;
    if(ui.proxyFireFox->isChecked())
        m = Settings::FireFox;
    if(ui.proxyManual->isChecked())
        m = Settings::Manual;
    s.setProxyMode(m);
    if (ui.proxyManual->isChecked())
    {
        QNetworkProxy proxy(QNetworkProxy::DefaultProxy,ui.proxyHost->text(),ui.proxyPort->text().toInt(),ui.proxyUserName->text(),ui.proxyPassword->text());
        s.setProxy(proxy);
    }
}

void SettingsProxyPage::reject()
{
}

bool SettingsProxyPage::isComplete()
{
    return true;
}

SettingsPage::SettingsPage(QWidget *parent)
: QDialog(parent), m_globalConfig(0), s(Settings::getInstance()), 
  m_downloadPage(ui), 
  m_installPage(ui), 
  m_proxyPage(ui)
{
    ui.setupUi(this);
    connect( ui.rootPathSelect,SIGNAL(clicked()),this,SLOT(rootPathSelectClicked()) );
    connect( ui.tempPathSelect,SIGNAL(clicked()),this,SLOT(tempPathSelectClicked()) );

    connect( ui.proxyManual,SIGNAL(clicked(bool)),this,SLOT(switchProxyFields(bool)) );
    connect( ui.proxyFireFox,SIGNAL(clicked(bool)),this,SLOT(switchProxyFields(bool)) );
    connect( ui.proxyIE,SIGNAL(clicked(bool)),this,SLOT(switchProxyFields(bool)) );
    connect( ui.proxyOff,SIGNAL(clicked(bool)),this,SLOT(switchProxyFields(bool)) );
//    init();
}

void SettingsPage::init()
{
    // required to reset changes from single page access
    ui.setupUi(this);

    m_downloadPage.reset();
    m_installPage.reset();
    m_proxyPage.reset();

    ui.displayTitlePage->setCheckState(s.showTitlePage() ? Qt::Checked : Qt::Unchecked);
    ui.createStartMenuEntries->setCheckState(s.createStartMenuEntries() ? Qt::Checked : Qt::Unchecked);
    ui.nestedDownloadTree->setCheckState(s.nestedDownloadTree() ? Qt::Checked : Qt::Unchecked);
    ui.installDetails->setCheckState(s.installDetails() ? Qt::Checked : Qt::Unchecked);
    ui.autoNextStep->setCheckState(s.autoNextStep() ? Qt::Checked : Qt::Unchecked);
}

void SettingsPage::accept()
{
    hide();
    m_downloadPage.accept();
    m_installPage.accept();
    m_proxyPage.accept();

    s.setShowTitlePage(ui.displayTitlePage->checkState() == Qt::Checked ? true : false);
    s.setNestedDownloadTree(ui.nestedDownloadTree->checkState() == Qt::Checked ? true : false);
    s.setInstallDetails(ui.installDetails->checkState() == Qt::Checked ? true : false);
    s.setAutoNextStep(ui.autoNextStep->checkState() == Qt::Checked ? true : false);
}

void SettingsPage::reject()
{
    hide();
    init(); // reinit page to restore old settings
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

void SettingsPage::rebuildMirrorList(int index)
{
    disconnect(ui.downloadMirror, SIGNAL(currentIndexChanged(int)), 0, 0);
	ui.downloadMirror->clear();
	ui.downloadMirror->addItems(mirrorList);
	ui.downloadMirror->addItem("...Add Mirror...", 0);
	ui.downloadMirror->setCurrentIndex(index);
	connect(ui.downloadMirror, SIGNAL(currentIndexChanged(int)), this, SLOT(addNewMirror(int)));
}

void SettingsPage::switchProxyFields(bool mode)
{
    ui.proxyHost->setEnabled(mode);
    ui.proxyPort->setEnabled(mode);
    ui.proxyUserName->setEnabled(mode);
    ui.proxyPassword->setEnabled(mode);
}

void SettingsPage::addNewMirror(int index)
{
	if(index == ui.downloadMirror->count() - 1)
	{
		bool ok;
	    QString text = QInputDialog::getText(this, tr("Add a new Mirror"),
	                                         tr("Mirror address:"), QLineEdit::Normal,
	                                         QString("http://"), &ok);
        if (ok && !text.isEmpty()) {
            mirrorList << text;
            s.addLocalMirror(text);
        }
		rebuildMirrorList(ui.downloadMirror->count() - 1);
	}
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
