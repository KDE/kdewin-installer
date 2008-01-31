#include <QtDebug>
#include <QFileDialog>
#include <QInputDialog>

#include "settings.h"
#include "settingspage.h"
#include "database.h"
#include "mirrors.h"

QWidget *SettingsDownloadPage::widget()
{
    ui.downloadGroupBox->setFlat(true);
    ui.downloadGroupBox->setTitle("");
    return ui.downloadGroupBox;
}

void SettingsDownloadPage::reset()
{
    /// @TODO set initial values
}

void SettingsDownloadPage::accept()
{
    s.setDownloadDir(ui.tempPathEdit->text());
}

void SettingsDownloadPage::reject()
{
}

bool SettingsDownloadPage::isComplete()
{
    return !ui.tempPathEdit->text().isEmpty();
}

QWidget *SettingsMirrorPage::widget()
{
    ui.mirrorGroupBox->setFlat(true);
    ui.mirrorGroupBox->setTitle("");
    return ui.mirrorGroupBox;
}

void SettingsMirrorPage::reset()
{
    Mirrors &mirrors = Mirrors::getInstance();
    if (mirrors.mirrors().size() == 0)
    {
        /// @TODO add vivible progress bar
        if ( !mirrors.fetch(Mirrors::Cygwin, QUrl("http://download.cegit.de/kde-windows/mirrors.lst")) )
        {
            qCritical() << "could not load mirrors";
            // display warning box
        }
    }

    QUrl currentMirror = QUrl(s.mirror());

    foreach(MirrorType m, mirrors.mirrors())
    {
        qDebug() << m.toString();
        ui.downloadMirror->addItem(m.toString(), m.url);
    }
    if (!currentMirror.isEmpty())
    {
        qDebug() << currentMirror;
        int index = ui.downloadMirror->findData(currentMirror);
        if (index != -1)
              ui.downloadMirror->setCurrentIndex(index);
        else
        {
              ui.downloadMirror->addItem(currentMirror.toString(), currentMirror);
              ui.downloadMirror->setCurrentIndex(ui.downloadMirror->count()-1);
        }
    }
    else
        ui.downloadMirror->setCurrentIndex(0);
    connect(ui.downloadMirror, SIGNAL(activated( int )), this, SLOT(mirrorChanged( int )));
    connect(ui.addMirrorButton,SIGNAL(clicked()), this, SLOT(addNewMirrorClicked()));
}

void SettingsMirrorPage::mirrorChanged( int numberMirror )
{
    qDebug() << "mirrorChanged signal caught";
    emit completeStateChanged();
}

void SettingsMirrorPage::accept()
{
    QUrl data = ui.downloadMirror->itemData(ui.downloadMirror->currentIndex()).toUrl();
    qDebug() << data;
    if (!data.isEmpty() && QUrl(s.mirror()) != data)
        s.setMirror(data.toString());
}

void SettingsMirrorPage::reject()
{
}

bool SettingsMirrorPage::isComplete()
{
    return !ui.downloadMirror->currentText().isEmpty();
}


void SettingsMirrorPage::addNewMirrorClicked()
{
    bool ok;
    QString text = QInputDialog::getText(parent, tr("Add a new Mirror"),
                                         tr("Mirror address:"), QLineEdit::Normal,
                                         QString("http://"), &ok);
    if (ok && !text.isEmpty())
    {
        ui.downloadMirror->addItem(text,QUrl(text));
        ui.downloadMirror->setCurrentIndex(ui.downloadMirror->count()-1);
    }
}

SettingsPage::SettingsPage(QWidget *parent)
: QDialog(parent),
  m_downloadPage(ui,parent),
  m_mirrorPage(ui,parent),
  s(Settings::getInstance())
{
    ui.setupUi(this);
    connect( ui.rootPathSelect,SIGNAL(clicked()),this,SLOT(rootPathSelectClicked()) );
    connect( ui.tempPathSelect,SIGNAL(clicked()),this,SLOT(tempPathSelectClicked()) );

    connect( ui.proxyManual,SIGNAL(clicked(bool)),this,SLOT(switchProxyFields(bool)) );
    connect( ui.proxyFireFox,SIGNAL(clicked(bool)),this,SLOT(switchProxyFields(bool)) );
    connect( ui.proxyIE,SIGNAL(clicked(bool)),this,SLOT(switchProxyFields(bool)) );
    connect( ui.proxyOff,SIGNAL(clicked(bool)),this,SLOT(switchProxyFields(bool)) );
//    init();
}

void SettingsPage::init()
{
    // required to reset changes from single page access
    ui.setupUi(this);

    m_downloadPage.reset();
    m_mirrorPage.reset();

    ui.displayTitlePage->setCheckState(s.showTitlePage() ? Qt::Checked : Qt::Unchecked);
    ui.createStartMenuEntries->setCheckState(s.createStartMenuEntries() ? Qt::Checked : Qt::Unchecked);
    ui.nestedDownloadTree->setCheckState(s.nestedDownloadTree() ? Qt::Checked : Qt::Unchecked);
    ui.installDetails->setCheckState(s.installDetails() ? Qt::Checked : Qt::Unchecked);
    ui.autoNextStep->setCheckState(s.autoNextStep() ? Qt::Checked : Qt::Unchecked);

}

void SettingsPage::accept()
{
    hide();
    m_downloadPage.accept();
    m_mirrorPage.accept();

    s.setShowTitlePage(ui.displayTitlePage->checkState() == Qt::Checked ? true : false);
    s.setNestedDownloadTree(ui.nestedDownloadTree->checkState() == Qt::Checked ? true : false);
    s.setInstallDetails(ui.installDetails->checkState() == Qt::Checked ? true : false);
    s.setAutoNextStep(ui.autoNextStep->checkState() == Qt::Checked ? true : false);
}

void SettingsPage::reject()
{
    hide();
    m_downloadPage.reject();
    m_mirrorPage.reject();
    init(); // reinit page to restore old settings, is this really required ?
}

void SettingsPage::rootPathSelectClicked()
{
    QString fileName = QFileDialog::getExistingDirectory(this,
                       tr("Select Root Installation Directory"),
                       "",
                       QFileDialog::ShowDirsOnly| QFileDialog::DontResolveSymlinks);
    if(!fileName.isEmpty())
        ui.rootPathEdit->setText(QDir::toNativeSeparators(fileName));
}

void SettingsPage::rootPathEditChanged(const QString &text)
{
    // update select boxes
    s.setInstallDir(text);
    bool f = Database::isAnyPackageInstalled(s.installDir());
    ui.compilerUnspecified->setEnabled(!f);
    ui.compilerMinGW->setEnabled(!f);
    ui.compilerMSVC->setEnabled(!f);
}

void SettingsPage::tempPathSelectClicked()
{
    QString fileName = QFileDialog::getExistingDirectory(this,
                       tr("Select Package Download Directory"),
                       "",
                       QFileDialog::ShowDirsOnly| QFileDialog::DontResolveSymlinks);
    if(!fileName.isEmpty())
        ui.tempPathEdit->setText(QDir::toNativeSeparators(fileName));
}

void SettingsPage::switchProxyFields(bool mode)
{
    ui.proxyHost->setEnabled(mode);
    ui.proxyPort->setEnabled(mode);
    ui.proxyUserName->setEnabled(mode);
    ui.proxyPassword->setEnabled(mode);
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
