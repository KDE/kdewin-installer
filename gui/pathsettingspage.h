
#ifndef PATHSETTINGSPAGE_H
#define PATHSETTINGSPAGE_H

#include "installwizard.h"

#include "ui_pathsettingspage.h"

class PathSettingsPage : public InstallWizardPage
{
    Q_OBJECT

public:
    PathSettingsPage();

    void initializePage();
    bool isComplete();
    int nextId() const;
    bool validatePage();
protected:
    Ui::PathSettingsPage ui;
    
protected slots:
    void rootPathSelectClicked();
};

#endif
