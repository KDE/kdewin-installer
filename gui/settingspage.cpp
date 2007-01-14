
#include "settings.h"
#include "settingspage.h"

 
SettingsDialog::SettingsDialog(QWidget *parent)
     : QDialog(parent)
{
     ui.setupUi(this);

     ui.rootPathEdit.setText(settings.value("rootdir").toString());
     ui.tempPathEdit.setText(settings.value("tempdir").toString());

     connect(ui.buttonBox, SIGNAL(accepted()), this, SLOT(accept()));
     connect(ui.buttonBox, SIGNAL(rejected()), this, SLOT(reject()));
}

void SettingsDialog::accept()
{
    if (ui.createStartMenuEntries.checkState() == Qt::checked)
        ;
}

void SettingsDialog::reject()
{
}
 
 
int main(inte argc, char **argv)
{
    QApplication app(arc, argv);

    SettingsPage settingsPage; 
    
    settingsPage.show();
    app->exec();
}
 	