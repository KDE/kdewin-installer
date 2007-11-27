#include <QtDebug>
#include <QFileDialog>
#include <QInputDialog>

#include "settings.h"
#include "settingspage.h"
#include "database.h"
#include "mirrors.h"

QWidget *SettingsDownloadPage::widget()
{
    ui.downloadGroupBox->setFlat(true);
    ui.downloadGroupBox->setTitle("");
    return ui.downloadGroupBox;
}

void SettingsDownloadPage::reset()
{
    /// @TODO set initial values 
}

void SettingsDownloadPage::accept()
{
    s.setDownloadDir(ui.tempPathEdit->text());
}

void SettingsDownloadPage::reject()
{
}

bool SettingsDownloadPage::isComplete()
{
    return !ui.tempPathEdit->text().isEmpty();
}

QWidget *SettingsMirrorPage::widget()
{
    ui.mirrorGroupBox->setFlat(true);
    ui.mirrorGroupBox->setTitle("");
    return ui.mirrorGroupBox;
}

void SettingsMirrorPage::reset()
{
    Mirrors &mirrors = Mirrors::getInstance();
    if (mirrors.mirrors().size() == 0)
    {
        /// @TODO add vivible progress bar
        if ( !mirrors.fetch(Mirrors::Cygwin,QUrl("http://download.cegit.de/kde-windows/mirrors.lst")) ) 
        {
            qCritical() << "could not load mirrors"; 
            // display warning box
        }
    }

    QUrl currentMirror = QUrl(s.mirror());
    
	foreach(MirrorType m, mirrors.mirrors())
	{
        qDebug() << m;
    	ui.downloadMirror->addItem(m.toString(),m.url);
    }
    qDebug() << currentMirror;
    int index = ui.downloadMirror->findData(currentMirror);
    if (index != -1)
        ui.downloadMirror->setCurrentIndex(index);
    else
    {
        ui.downloadMirror->addItem(currentMirror.toString(),currentMirror);
        ui.downloadMirror->setCurrentIndex(ui.downloadMirror->count()-1);
    }
}

void SettingsMirrorPage::accept()
{
    QUrl data = ui.downloadMirror->itemData(ui.downloadMirror->currentIndex()).toUrl();
    qDebug() << data;
    if (!data.isEmpty() && QUrl(s.mirror()) != data)
        s.setMirror(data.toString());
}

void SettingsMirrorPage::reject()
{
}

bool SettingsMirrorPage::isComplete()
{
    return !ui.downloadMirror->currentText().isEmpty();
}

QWidget *SettingsInstallPage::widget()
{
    ui.installGroupBox->setFlat(true);
    ui.installGroupBox->setTitle("");
    return ui.installGroupBox;
}

void SettingsInstallPage::reset()
{
    ui.createStartMenuEntries->setEnabled(false);
    ui.installModeEndUser->setChecked(!s.isDeveloperMode() ? Qt::Checked : Qt::Unchecked);
    ui.installModeDeveloper->setChecked(s.isDeveloperMode() ? Qt::Checked : Qt::Unchecked);
    ui.rootPathEdit->setText(QDir::convertSeparators(s.installDir()));
    ui.tempPathEdit->setText(QDir::convertSeparators(s.downloadDir()));

    // logical grouping isn't available in the designer yet :-P
    QButtonGroup *groupA = new QButtonGroup(this);
    groupA->addButton(ui.compilerUnspecified);
    groupA->addButton(ui.compilerMinGW);
    groupA->addButton(ui.compilerMSVC);

    if (Database::isAnyPackageInstalled(s.installDir()))
    {
        ui.compilerUnspecified->setEnabled(false);
        ui.compilerMinGW->setEnabled(false);
        ui.compilerMSVC->setEnabled(false);
    }
    switch (s.compilerType()) {
		case Settings::unspecified: ui.compilerUnspecified->setChecked(true); break;
		case Settings::MinGW: ui.compilerMinGW->setChecked(true); break;
		case Settings::MSVC: ui.compilerMSVC->setChecked(true); break;
        default: ui.compilerMinGW->setChecked(true); break;
	}

    QButtonGroup *groupB = new QButtonGroup(this);
    groupB->addButton(ui.installModeDeveloper);
    groupB->addButton(ui.installModeEndUser);

    if (Database::isAnyPackageInstalled(s.installDir()))
    {
        ui.installModeDeveloper->setEnabled(false); 
        ui.installModeEndUser->setEnabled(false);
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
: QDialog(parent), s(Settings::getInstance()), 
  m_downloadPage(ui), 
  m_installPage(ui), 
  m_proxyPage(ui),
  m_mirrorPage(ui)
{
    ui.setupUi(this);
    connect( ui.rootPathSelect,SIGNAL(clicked()),this,SLOT(rootPathSelectClicked()) );
    connect( ui.tempPathSelect,SIGNAL(clicked()),this,SLOT(tempPathSelectClicked()) );
	connect( ui.addMirrorButton,SIGNAL(clicked()), this, SLOT(addNewMirrorClicked()));

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
    m_mirrorPage.reset();

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
    m_mirrorPage.accept();

    s.setShowTitlePage(ui.displayTitlePage->checkState() == Qt::Checked ? true : false);
    s.setNestedDownloadTree(ui.nestedDownloadTree->checkState() == Qt::Checked ? true : false);
    s.setInstallDetails(ui.installDetails->checkState() == Qt::Checked ? true : false);
    s.setAutoNextStep(ui.autoNextStep->checkState() == Qt::Checked ? true : false);
}

void SettingsPage::reject()
{
    hide();
    m_downloadPage.reject();
    m_installPage.reject();
    m_proxyPage.reject();
    m_mirrorPage.reject();
    init(); // reinit page to restore old settings, is this really required ? 
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


void SettingsPage::addNewMirrorClicked()
{
	bool ok;
    QString text = QInputDialog::getText(this, tr("Add a new Mirror"),
                                         tr("Mirror address:"), QLineEdit::Normal,
                                         QString("http://"), &ok);
    if (ok && !text.isEmpty()) 
        ui.downloadMirror->addItem(text,QUrl(text));
}



void SettingsPage::switchProxyFields(bool mode)
{
    ui.proxyHost->setEnabled(mode);
    ui.proxyPort->setEnabled(mode);
    ui.proxyUserName->setEnabled(mode);
    ui.proxyPassword->setEnabled(mode);
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
