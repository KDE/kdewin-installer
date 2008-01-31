
#include <QDir>
#include <QString>
#include <QFileDialog>

#include "database.h"
#include "pathsettingspage.h"

PathSettingsPage::PathSettingsPage(SettingsSubPage *s) : InstallWizardPage(s)
{
//    setTitle(tr("Basic Setup"));
//    setSubTitle(tr("Select the directory where you want to install the KDE packages, "
//       "for which compiler this installation should be and which installation mode you prefer."
//    ));
    
    ui.setupUi(this);
    setTitle(windowTitle());
    setSubTitle(statusTip());

    QVBoxLayout *layout = new QVBoxLayout;
    // @TODO: remove top level line from widget
//    layout->addWidget(page->widget(),10);
    layout->addWidget(statusLabel,1,Qt::AlignBottom);
    setLayout(layout);
    connect( ui.rootPathSelect,SIGNAL(clicked()),this,SLOT(rootPathSelectClicked()) );
}

void PathSettingsPage::initializePage()
{
    Settings &s = Settings::getInstance();
    ui.createStartMenuEntries->setEnabled(false);
    ui.installModeEndUser->setChecked(!s.isDeveloperMode() ? Qt::Checked : Qt::Unchecked);
    ui.installModeDeveloper->setChecked(s.isDeveloperMode() ? Qt::Checked : Qt::Unchecked);
    ui.rootPathEdit->setText(QDir::convertSeparators(s.installDir()));
//    ui.tempPathEdit->setText(QDir::convertSeparators(s.downloadDir()));

    // logical grouping isn't available in the designer yet :-P
    QButtonGroup *groupA = new QButtonGroup(this);
    groupA->addButton(ui.compilerUnspecified);
    groupA->addButton(ui.compilerMinGW);
    groupA->addButton(ui.compilerMSVC);

    if (Database::isAnyPackageInstalled(s.installDir()))
    {
        ui.compilerUnspecified->setEnabled(false);
        ui.compilerMinGW->setEnabled(false);
        ui.compilerMSVC->setEnabled(false);
    }
    else
    {
        ui.compilerUnspecified->setEnabled(true);
        ui.compilerMinGW->setEnabled(true);
        ui.compilerMSVC->setEnabled(true);
    }

    switch (s.compilerType()) {
        case Settings::unspecified: ui.compilerUnspecified->setChecked(true); break;
        case Settings::MinGW: ui.compilerMinGW->setChecked(true); break;
        case Settings::MSVC: ui.compilerMSVC->setChecked(true); break;
        default: ui.compilerMinGW->setChecked(true); break;
    }

    QButtonGroup *groupB = new QButtonGroup(this);
    groupB->addButton(ui.installModeDeveloper);
    groupB->addButton(ui.installModeEndUser);

    if (Database::isAnyPackageInstalled(s.installDir()))
    {
        ui.installModeDeveloper->setEnabled(false);
        ui.installModeEndUser->setEnabled(false);
    }
    else
    {
        ui.installModeDeveloper->setEnabled(true);
        ui.installModeEndUser->setEnabled(true);
    }

    if (s.isDeveloperMode())
        ui.installModeDeveloper->setChecked(true);
    else
        ui.installModeEndUser->setChecked(true);

    setPixmap(QWizard::WatermarkPixmap, QPixmap());
}

bool PathSettingsPage::isComplete()
{
    return !ui.rootPathEdit->text().isEmpty();
}

int PathSettingsPage::nextId() const
{
    return InstallWizard::proxySettingsPage;
}

bool PathSettingsPage::validatePage()
{
    Settings &s = Settings::getInstance();
    s.setCreateStartMenuEntries(ui.createStartMenuEntries->checkState() == Qt::Checked ? true : false);
    s.setInstallDir(ui.rootPathEdit->text());

    s.setDeveloperMode(ui.installModeDeveloper->isChecked());

    if (ui.compilerUnspecified->isChecked())
        s.setCompilerType(Settings::unspecified);
    if (ui.compilerMinGW->isChecked())
        s.setCompilerType(Settings::MinGW);
    if (ui.compilerMSVC->isChecked())
        s.setCompilerType(Settings::MSVC);
    return true;
}

void PathSettingsPage::rootPathSelectClicked()
{
    QString fileName = QFileDialog::getExistingDirectory(this,
                       tr("Select Root Installation Directory"),
                       "",
                       QFileDialog::ShowDirsOnly| QFileDialog::DontResolveSymlinks);
    if(!fileName.isEmpty())
        ui.rootPathEdit->setText(QDir::toNativeSeparators(fileName));
}


#include "pathsettingspage.moc"
