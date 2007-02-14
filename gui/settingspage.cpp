#include <QtDebug>
#include <QFileDialog>

#include "settings.h"
#include "settingspage.h"

 
SettingsPage::SettingsPage(QWidget *parent)
     : QDialog(parent)
{
    Settings &settings = Settings::getInstance();
    ui.setupUi(this);

    ui.createStartMenuEntries->setEnabled(false);
    ui.displayTitlePage->setCheckState(settings.showTitlePage() ? Qt::Checked : Qt::Unchecked);
    ui.displayTitlePage->setCheckState(settings.createStartMenuEntries() ? Qt::Checked : Qt::Unchecked);
	ui.nestedDownloadTree->setCheckState(settings.nestedDownloadTree() ? Qt::Checked : Qt::Unchecked);

    ui.rootPathEdit->setText(QDir::convertSeparators(settings.installDir()));
    ui.tempPathEdit->setText(QDir::convertSeparators(settings.downloadDir()));

    connect( ui.rootPathSelect,SIGNAL(clicked()),this,SLOT(rootPathSelectClicked()) );
    connect( ui.tempPathSelect,SIGNAL(clicked()),this,SLOT(tempPathSelectClicked()) );
}

void SettingsPage::accept()
{
    hide();
    Settings &settings = Settings::getInstance();

    settings.setCreateStartMenuEntries(ui.createStartMenuEntries->checkState() == Qt::Checked ? true : false);
    settings.setShowTitlePage(ui.displayTitlePage->checkState() == Qt::Checked ? true : false);
    settings.setNestedDownloadTree(ui.nestedDownloadTree->checkState() == Qt::Checked ? true : false);
    settings.setInstallDir(ui.rootPathEdit->text());
    settings.setDownloadDir(ui.tempPathEdit->text());
	
}

void SettingsPage::reject()
{
    hide();
}

void SettingsPage::rootPathSelectClicked()
{
    QString fileName = QFileDialog::getExistingDirectory(this,
                       tr("Select Root Installation Directory"),
                       "",
                       QFileDialog::ShowDirsOnly| QFileDialog::DontResolveSymlinks);
    ui.rootPathEdit->setText(fileName);
}
 
void SettingsPage::tempPathSelectClicked()
{
    QString fileName = QFileDialog::getExistingDirectory(this,
                       tr("Select Package Download Directory"),
                       "",
                       QFileDialog::ShowDirsOnly| QFileDialog::DontResolveSymlinks);
    ui.tempPathEdit->setText(fileName);
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
