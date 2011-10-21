/****************************************************************************
**
** Copyright (C) 2010-2011 Ralf Habacker ralf.habacker@freenet.de
**
** This file is part of the KDE installer for windows
**
** This library is free software; you can redistribute it and/or
** modify it under the terms of the GNU Library General Public
** License version 2 as published by the Free Software Foundation.
**
** This library is distributed in the hope that it will be useful,
** but WITHOUT ANY WARRANTY; without even the implied warranty of
** MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU
** Library General Public License for more details.
**
** You should have received a copy of the GNU Library General Public License
** along with this library; see the file COPYING.LIB.  If not, write to
** the Free Software Foundation, Inc., 51 Franklin Street, Fifth Floor,
** Boston, MA 02110-1301, USA.
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

#include <QtGui>

InstallerDialog::InstallerDialog()
    :  okay(":/images/dialog-ok-apply.png"), next(":/images/go-next.png"), error(":/images/dialog-cancel.png"), m_postProcessing(&m_engine, this)
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
        InstallerEngine::defaultConfigURL = QString("http://www.winkde.org/pub/kde/ports/win32/releases/%1/%2").arg(toString(config.releaseType)).arg(config.version);
        QString installRoot = QString("%1/%2-%3-%4-%5").arg(QLatin1String(qgetenv("ProgramFiles"))).arg(config.packageName).arg(CompilerTypes::toString(config.compilerType)).arg(toString(config.releaseType)).arg(config.version);
        addHint("I'm installing into " + installRoot);
        Settings::instance().setInstallDir(installRoot, false);
        m_engine.setRoot(installRoot);
        ProxySettings ps;

        m_postProcessing.setSingleApplicationMode(true);
        m_postProcessing.setPackageName(config.packageName);

        setWindowTitle(tr("KDE %1 Application Installer").arg(config.packageName));
        ui.topLabel->setText(tr("KDE %1 Installer").arg(config.packageName));

        if (ps.from(ProxySettings::AutoDetect))
        {
            ps.save();
            addHint("I'm " + ps.toString());
        }
        packages << config.packageName.toLower()+'-'+CompilerTypes::toString(config.compilerType);
        setItem(0);
        QTimer::singleShot(250,this,SLOT(setupEngine()));
    }
    else
    {
        setWindowTitle(tr("KDE Single Application Installer"));
        ui.topLabel->setText(tr("KDE Single Application Installer"));
        addHint("I could not found a valid configuration");
    }
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
    setSubLabelHint("");
}

void InstallerDialog::addHint(const QString &hint)
{
    ui.hintLabel->setText(ui.hintLabel->text() + "\n" + hint);
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
    QString logFileName = logFileName();
    if (!logFileName.isEmpty())
        addHint(QString("!! Error details could be fetched from %1").arg(QUrl(logFileName));
    QCoreApplication::processEvents();
}

void InstallerDialog::setupEngine()
{  
    setItem(1);
    addHint("I'm fetching packages from " + InstallerEngine::defaultConfigURL);
    if (m_engine.init())
    {
        Package *p = m_engine.getPackageByName(packages[0]);
        if (!p) 
        {
            addHint(QString(".... could not find package %s").arg(packages[0]));
            setError(4);
        }
        packagesToInstall.append(p);
        m_engine.setDependencyState(p,packagesToInstall);
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
    addHint(QString("I will download up to %1 package(s) depending on what has been downloaded earlier").arg(packagesToInstall.size()));
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
    if (m_engine.downloadPackages(packagesToInstall))
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
    addHint(QString("I'm installing %1 package(s)").arg(packagesToInstall.size()));
    if (m_engine.installPackages(packagesToInstall))
        QTimer::singleShot(1,this,SLOT(postProcessing()));
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
    m_postProcessing.setVersion(packagesToInstall[0]->version().toString());
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
    m_postProcessing.stop();
    QDialog::close();
}

