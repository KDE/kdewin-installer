#ifndef SETTINGSPAGE_H
#define SETTINGSPAGE_H

#include <QDialog>

namespace Ui {
    class SettingsDialog;
}
#include "globalconfig.h"
#include "settings.h"

class SettingsPage : public QDialog
{
    Q_OBJECT

public:
    SettingsPage(QWidget *parent = 0);
    void init();

protected:

private slots:
    void accept();
    void reject();

private:
    Ui::SettingsDialog *ui;
    Settings &s;
};


#endif
