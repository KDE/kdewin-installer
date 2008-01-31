
#ifndef INTERNETSETTINGSPAGE_H
#define INTERNETSETTINGSPAGE_H

#include "installwizard.h"
#include "ui_internetsettingspage.h"

class InternetSettingsPage : public InstallWizardPage
{
    Q_OBJECT

public:
    InternetSettingsPage();

    void initializePage();
    bool isComplete();
    int nextId() const;
    bool validatePage();

protected: 
    Ui::InternetSettingsPage ui;
};


#endif