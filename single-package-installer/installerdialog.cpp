/****************************************************************************
**
** Copyright (C) 2010-2011 Ralf Habacker ralf.habacker@freenet.de
**
** This file is part of the KDE installer for windows
**
** SPDX-License-Identifier: LGPL-2.0-only
**
****************************************************************************/

#include "installerdialog.h"

#include "../shared/misc.h"
#include "../shared/debug.h"
#include "../shared/installercallconfig.h"
#include "../shared/installerprogress.h"
#include "../shared/downloader.h"
#include "../shared/unpacker.h"
#include "../shared/postprocessing.h"

#include <QCloseEvent>
#include <QPixmap>
#include <QTextEdit>
#include <QTimer>

InstallerDialog::InstallerDialog()
    : m_postProcessing(&m_engine, this),
      next(":/images/go-next.png"),
      okay(":/images/dialog-ok-apply.png"),
      error(":/images/dialog-cancel.png"),
      m_log(0)
{
    ui.setupUi(this);

    connect(ui.closeButton, SIGNAL(clicked()), this, SLOT(stop()));
    initItems();

    setMessageHandler("single-package-installer");

    QString setupName;

    if (QCoreApplication::arguments().size() > 1)
        setupName = QCoreApplication::arguments().at(1);

    InstallerCallConfig config(setupName);

    if (config.isValid())
    {
        InstallerEngine::defaultConfigURL = QString("http://download.kde.org/%1/%2").arg(toString(config.releaseType)).arg(config.version);
        InstallerEngine::fallBackURL = QString("http://www.winkde.org/pub/kde/ports/win32/releases/%1/%2").arg(toString(config.releaseType)).arg(config.version);
#ifdef Q_OS_WIN
        QString installRoot = QString("%1/%2-%3-%4-%5%6").arg(QLatin1String(qgetenv("ProgramFiles"))).arg(config.packageName).arg(allCompilers.toString(config.compilerType)).arg(toString(config.releaseType)).arg(config.version).arg(config.hasSDK ? "-sdk" : "");
#else
        QString installRoot = QString("%1/%2-%3-%4-%5%6").arg(QLatin1String(qgetenv("HOME"))).arg(config.packageName).arg(allCompilers.toString(config.compilerType)).arg(toString(config.releaseType)).arg(config.version).arg(config.hasSDK ? "-sdk" : "");
#endif
        addHint("I'm installing into " + installRoot);
        Settings::instance().setInstallDir(installRoot, false);
        m_engine.setRoot(installRoot);
        m_engine.setWithDevelopmentPackages(config.hasSDK);
        m_engine.setCurrentCompiler(config.compilerType);
        ProxySettings ps;

        m_postProcessing.setSingleApplicationMode(true);
        m_postProcessing.setPackageName(config.packageName);

        QString title = tr("KDE %1 %2 Installer").arg(config.packageName).arg(config.hasSDK ? "SDK" : "Application");
        setWindowTitle(title);
        ui.topLabel->setText(title);

        if (ps.from(ProxySettings::AutoDetect))
        {
            ps.save();
            addHint("I'm " + ps.toString());
        }
        m_packages << config.packageName.toLower()+'-'+allCompilers.toString(config.compilerType);

        // @TODO: this is a hack, need to be solved by build requirements in the config file
        if (config.hasSDK && config.packageName.toLower() == "umbrello")
            m_packages << "automoc" << "boost";

        setItem(0);
        QTimer::singleShot(250,this,SLOT(setupEngine()));
    }
    else
    {
        setWindowTitle(tr("KDE Single Application Installer"));
        ui.topLabel->setText(tr("KDE Single Application Installer"));
        addHint("I could not find a valid configuration");
    }
}

InstallerDialog::~InstallerDialog()
{
	delete m_log;
}

void InstallerDialog::initItems()
{
    ui.label1->setText("");
    ui.label1->setVisible(false);
    ui.textLabel1->setVisible(false);
    ui.label2->setText("");
    ui.label2->setVisible(false);
    ui.textLabel2->setVisible(false);
    ui.label3->setText("");
    ui.label3->setVisible(false);
    ui.textLabel3->setVisible(false);
    ui.label4->setText("");
    ui.label4->setVisible(false);
    ui.textLabel4->setVisible(false);
    ui.label5->setText("");
    ui.label5->setVisible(false);
    ui.textLabel5->setVisible(false);
	ui.hintLabel->setTextInteractionFlags(Qt::TextBrowserInteraction);
    ui.logButton->setVisible(false);
    setSubLabelHint("");
	connect(ui.logButton, SIGNAL(clicked(bool)), this, SLOT(showLog(bool)));
}

void InstallerDialog::addHint(const QString &hint)
{
    ui.hintLabel->setText(ui.hintLabel->text() + "<br>" + hint);
    QCoreApplication::processEvents();
}

void InstallerDialog::setSubLabelHint(const QString &hint)
{
    if (hint.isEmpty())
    {
        ui.subLabel->setVisible(false);
    }
    else
    {
        ui.subLabel->setVisible(true);
        ui.subLabel->setText(hint);
    }
    QCoreApplication::processEvents();
}

void InstallerDialog::showLog(bool checked)
{
    m_log = new QTextEdit(*log());
    m_log->showMaximized();
}

void InstallerDialog::setItem(int pagenum)
{
    switch (pagenum)
    {
        case 0:
            ui.label1->setPixmap(next);
            ui.label1->setVisible(true);
            ui.textLabel1->setVisible(true);
            break;

        case 1:
            ui.label1->setPixmap(okay);
            ui.label2->setPixmap(next);
            ui.label2->setVisible(true);
            ui.textLabel2->setVisible(true);
            break;

        case 2:
            ui.label2->setPixmap(okay);
            ui.label3->setPixmap(next);
            ui.label3->setVisible(true);
            ui.textLabel3->setVisible(true);
            break;

        case 3:
            ui.label3->setPixmap(okay);
            ui.label4->setPixmap(next);
            ui.label4->setVisible(true);
            ui.textLabel4->setVisible(true);
            break;

        case 4:
            ui.label4->setPixmap(okay);
            ui.label5->setPixmap(next);
            ui.label5->setVisible(true);
            ui.textLabel5->setVisible(true);
            break;

        case 5:
            ui.label5->setPixmap(okay);
            break;
    }
    QCoreApplication::processEvents();
}

void InstallerDialog::setError(int pagenum)
{
    switch (pagenum)
    {
        case 0:
            ui.label1->setPixmap(error);
            break;

        case 1:
            ui.label2->setPixmap(error);
            break;

        case 2:
            ui.label3->setPixmap(error);
            break;

        case 3:
            ui.label4->setPixmap(error);
            break;

        case 4:
            ui.label5->setPixmap(error);
            break;
    }
    ui.logButton->setVisible(true);
    QCoreApplication::processEvents();
}

void InstallerDialog::setupEngine()
{
    setItem(1);

    if (m_engine.init())
    {
        QStringList urls;
        foreach(Site *site, *m_engine.globalConfig()->sites())
        {
            const QUrl listURL = site->listURL().isEmpty() ? site->url() : site->listURL();
            urls << site->name() + "&nbsp;:&nbsp;" + listURL.toString();
        }

        addHint("I'm fetching packages from the following repositories:<br>&nbsp;&nbsp;&nbsp;" + urls.join("<br>&nbsp;&nbsp;&nbsp;"));

        foreach(const QString &package, m_packages)
        {
            Package *p = m_engine.getPackageByName(package);
            if (p)
            {
                m_packagesToInstall.append(p);
                m_engine.setDependencyState(p,m_packagesToInstall);
            }
            else
            {
                addHint(QString(".... could not find package %1").arg(package));
                setError(4);
            }
        }
        // the api should provide a method like
        // m_engine.resolveDependencies(m_packagesToInstall);
        // @TODO: in sdk mode we should download and install all dependencies but not the requested package
        QTimer::singleShot(1,this,SLOT(downloadPackages()));
    }
    else
    {
        addHint(QString(".... initialisation failed"));
        setError(1);
    }
}

void InstallerDialog::downloadPackages()
{
    setItem(2);
    addHint(QString("I will download up to %1 package(s) depending on what has been downloaded earlier").arg(m_packagesToInstall.size()));
    m_counter = 10;
    QTimer::singleShot(1,this,SLOT(downloadPackagesStage1()));
}

void InstallerDialog::downloadPackagesStage1()
{
    if (m_counter-- > 0)
    {
        setSubLabelHint(QString("%1 seconds left until download starts").arg(m_counter+1));
        QTimer::singleShot(1000,this,SLOT(downloadPackagesStage1()));
    }
    else
    {
        setSubLabelHint("");
        QTimer::singleShot(1,this,SLOT(downloadPackagesStage2()));
    }
}

void InstallerDialog::downloadPackagesStage2()
{
    setItem(2);
    addHint(QString("I'm downloading package(s)"));
    if (m_engine.downloadPackages(m_packagesToInstall))
        QTimer::singleShot(1,this,SLOT(installPackages()));
    else
    {
        addHint(QString(".... download failed"));
        setError(2);
    }
}

void InstallerDialog::installPackages()
{
    setItem(3);
    addHint(QString("I'm installing %1 package(s)").arg(m_packagesToInstall.size()));
    if (m_engine.installPackages(m_packagesToInstall))
    {
        if (m_engine.withDevelopmentPackages())
            QTimer::singleShot(1,this,SLOT(finished()));
        else
            QTimer::singleShot(1,this,SLOT(postProcessing()));
    }
    else
    {
        addHint(QString(".... installation failed"));
        setError(3);
    }
}

void InstallerDialog::postProcessing()
{
    setItem(4);
    addHint("I'm running tools:");
    connect(&m_postProcessing,SIGNAL(finished()),this,SLOT(finished()));
    connect(&m_postProcessing,SIGNAL(commandStarted(const QString &)),this,SLOT(addHint(const QString &)));
    ui.closeButton->setEnabled(false);
    // fetch version from package - it is not sure yet if running a gui app --version returns the version on stdout
    if(m_packagesToInstall.size() <= 0) {
        finished();
        return;
    }

    m_postProcessing.setVersion(m_packagesToInstall[0]->version().toString());
    m_postProcessing.start();
}

void InstallerDialog::finished()
{
    setItem(5);
    ui.closeButton->setEnabled(true);
    ui.closeButton->setText(tr("Finish"));
}

void InstallerDialog::stop()
{
    close();
}

void InstallerDialog::closeEvent(QCloseEvent *e)
{
    m_postProcessing.stop();
    if (m_log)
        m_log->hide();
    e->accept();
}


