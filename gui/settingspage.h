#ifndef SETTINGSPAGE_H
#define SETTINGSPAGE_H

#include <QDialog>

#include "settings.h"
#include "ui_settingspage.h"

class SettingsPage : public QDialog
{
     Q_OBJECT

 public:
     SettingsPage(QWidget *parent = 0);

 private slots: 
     void accept();
     void reject();
     void rootPathSelectClicked();
     void tempPathSelectClicked();
     
 private:
     Ui::SettingsDialog ui;
     Settings settings;
};
 

#endif
