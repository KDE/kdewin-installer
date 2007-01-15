#include <QtDebug>

#include "settings.h"
#include "settingspage.h"

 
SettingsPage::SettingsPage(QWidget *parent)
     : QDialog(parent)
{
     ui.setupUi(this);

     ui.rootPathEdit->setText(settings.value("rootdir").toString());
     ui.tempPathEdit->setText(settings.value("tempdir").toString());
}

void SettingsPage::accept()
{
    if (ui.createStartMenuEntries->checkState() == Qt::Checked)
        ;
    settings.setValue("rootdir",ui.rootPathEdit->text());
    settings.setValue("tempdir",ui.tempPathEdit->text());
}

void SettingsPage::reject()
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

#include "settingspage.moc"
