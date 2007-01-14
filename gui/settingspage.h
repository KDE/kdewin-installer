#include <Dialog>

#include "ui_settingspage.h"

class SettingsDialog : public QDialog
{
     Q_OBJECT

 public:
     SettingsDialog(QWidget *parent = 0);

 private slots: 
     void accept();
     void reject();
     
 private:
     Ui::SettingsDialog ui;
     Settings settings;
};
 
