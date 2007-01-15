#include <QtDebug>
#include <QFileDialog>

#include "settingspage.h"

 
SettingsPage::SettingsPage(QWidget *parent)
     : QDialog(parent)
{
     ui.setupUi(this);

     ui.rootPathEdit->setText(settings.value("rootdir").toString());
     ui.tempPathEdit->setText(settings.value("tempdir").toString());
     connect( ui.rootPathSelect,SIGNAL(clicked()),this,SLOT(rootPathSelectClicked()) );
     connect( ui.tempPathSelect,SIGNAL(clicked()),this,SLOT(tempPathSelectClicked()) );
}

void SettingsPage::accept()
{
    hide();
    if (ui.createStartMenuEntries->checkState() == Qt::Checked)
        ;
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
