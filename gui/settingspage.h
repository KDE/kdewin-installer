#ifndef SETTINGSPAGE_H
#define SETTINGSPAGE_H

#include <QDialog>

#include "ui_settingspage.h"
#include "globalconfig.h"

class SettingsPage : public QDialog
{
     Q_OBJECT

 public:
     SettingsPage(QWidget *parent = 0);
     void init();
     void setGlobalConfig(GlobalConfig *globalConfig) { m_globalConfig = globalConfig; }

 private slots: 
     void accept();
     void reject();
     void rootPathSelectClicked();
     void tempPathSelectClicked();
     void switchProxyFields(bool checked);
     
 private:
     Ui::SettingsDialog ui;
     GlobalConfig *m_globalConfig;

};
 

#endif
