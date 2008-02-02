#include <QtDebug>
#include <QFileDialog>
#include <QInputDialog>

#include "settings.h"
#include "settingspage.h"
#include "database.h"
#include "mirrors.h"


SettingsPage::SettingsPage(QWidget *parent)
: QDialog(parent),
  s(Settings::instance())
{
//    init();
}

void SettingsPage::init()
{
    // required to reset changes from single page access
    ui.setupUi(this);
    ui.displayTitlePage->setCheckState(s.isPackageManagerMode() ? Qt::Checked : Qt::Unchecked);
    ui.installDetails->setCheckState(s.installDetails() ? Qt::Checked : Qt::Unchecked);
    ui.autoNextStep->setCheckState(s.autoNextStep() ? Qt::Checked : Qt::Unchecked);

}

void SettingsPage::accept()
{
    hide();

    s.setPackageManagerMode(ui.displayTitlePage->checkState() == Qt::Checked ? true : false);
    s.setInstallDetails(ui.installDetails->checkState() == Qt::Checked ? true : false);
    s.setAutoNextStep(ui.autoNextStep->checkState() == Qt::Checked ? true : false);
}

void SettingsPage::reject()
{
    hide();
    init(); // reinit page to restore old settings, is this really required ?
}

#if test
int main(int argc, char **argv)
{
    QApplication app(argc, argv);

    SettingsPage settingsPage;

    settingsPage.show();
    app.exec();
}
#endif

#include "settingspage.moc"
