#include <QtDebug>
#include <QFileDialog>

#include "settingspage.h"

 
SettingsPage::SettingsPage(QWidget *parent)
     : QDialog(parent)
{
    ui.setupUi(this);

    ui.createStartMenuEntries->setEnabled(false);
    ui.displayTitlePage->setCheckState(settings.value("displayTitlePage",false).toBool() ? Qt::Checked : Qt::Unchecked);

    ui.rootPathEdit->setText(settings.value("rootdir").toString());
    ui.tempPathEdit->setText(settings.value("tempdir").toString());
    connect( ui.rootPathSelect,SIGNAL(clicked()),this,SLOT(rootPathSelectClicked()) );
    connect( ui.tempPathSelect,SIGNAL(clicked()),this,SLOT(tempPathSelectClicked()) );
}

void SettingsPage::accept()
{
    hide();
    // FIXME: move to Settings
    settings.setValue("createStartMenuEntries",ui.createStartMenuEntries->checkState() == Qt::Checked ? true : false);
    settings.setValue("displayTitlePage",ui.displayTitlePage->checkState() == Qt::Checked ? true : false);
    // FIXME: update installerengine-> Settings class should emit a signal 
    settings.setValue("rootdir",ui.rootPathEdit->text());
    settings.setValue("tempdir",ui.tempPathEdit->text());
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
