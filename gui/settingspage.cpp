#include <QtDebug>

#include "settings.h"
#include "settingspage.h"

 
SettingsDialog::SettingsDialog(QWidget *parent)
     : QDialog(parent)
{
     ui.setupUi(this);

     ui.rootPathEdit->setText(settings.value("rootdir").toString());
     ui.tempPathEdit->setText(settings.value("tempdir").toString());

     connect(ui.buttonBox, SIGNAL(accepted()), this, SLOT(accept()));
     connect(ui.buttonBox, SIGNAL(rejected()), this, SLOT(reject()));
}

void SettingsDialog::accept()
{
    if (ui.createStartMenuEntries->checkState() == Qt::Checked)
        ;
    settings.setValue("rootdir",ui.rootPathEdit->text());
    settings.setValue("tempdir",ui.tempPathEdit->text());
}

void SettingsDialog::reject()
{
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