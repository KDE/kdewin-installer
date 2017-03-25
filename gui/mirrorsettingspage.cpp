/****************************************************************************
**
** Copyright (C) 2008 Ralf Habacker <ralf.habacker@freenet.de> 
** All rights reserved.
**
** This file is part of the KDE installer for windows
**
** This file may be used under the terms of the GNU General Public
** License version 2.0 as published by the Free Software Foundation
** and appearing in the file LICENSE.GPL included in the packaging of
** this file.  Please review the following information to ensure GNU
** General Public Licensing requirements will be met:
** http://www.trolltech.com/products/qt/opensource.html
**
** If you are unsure which license is appropriate for your use, please
** review the following information:
** http://www.trolltech.com/products/qt/licensing.html or contact the
** sales department at sales@trolltech.com.
**
** This file is provided AS IS with NO WARRANTY OF ANY KIND, INCLUDING THE
** WARRANTY OF DESIGN, MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE.
**
****************************************************************************/

#include "config.h"
#include "debug.h"
#include "mirrors.h"
#include "mirrorsettingspage.h"
#include "installerdialogs.h"

Mirrors::Config mirrorConfig(
    "http://download.kde.org/stable/kdewin/installer/mirrors.list",
    Mirrors::KDE,
    "",
    ""
);

Mirrors::Config fallBackConfig(
    "http://ftp.gwdg.de/pub/linux/kde/stable/kdewin/installer/mirrors.list",
    Mirrors::KDE,
    "",
    ""
);

QString fallBackHost = "ftp.gwdg.de";

MirrorSettingsPage::MirrorSettingsPage() : InstallWizardPage(0)
{
    ui.setupUi(this);
    setTitle(windowTitle());
    setSubTitle(statusTip());
}

void MirrorSettingsPage::initializePage()
{
    Settings &s = Settings::instance();
    Mirrors &mirrors = Mirrors::instance();
    mirrors.setConfig(mirrorConfig);
    InstallerDialogs::instance().enableDownloadProgressDialog(this,tr("Downloading Mirror List"));
    m_failed = false;
    qDebug() << "start";

    if (mirrors.mirrors().size() == 0)
    {
        qDebug() << "downloading mirror list from" << mirrorConfig.url;
        if ( !mirrors.fetch() ) 
        {
            qCritical() << "could not load mirrors from" << mirrorConfig.url;
            mirrors.setConfig(fallBackConfig);
            qDebug() << "downloading mirror list from" << fallBackConfig.url;
            if ( !mirrors.fetch() )
            {
                qCritical() << "could not load fallback mirror list from" << fallBackConfig.url;
                InstallerDialogs::instance().downloadMirrorListFailed(mirrorConfig.url,fallBackConfig.url);
                m_failed = true;
            }
        }
    }
    ui.downloadMirror->clear();
    InstallerDialogs::instance().disableDownloadProgressDialog();

    QUrl currentMirror(s.mirror());

    int mirrorIndex = -1;
    Q_FOREACH(const MirrorType &m, mirrors.mirrors())
    {
        qDebug() << m.toString();
        QListWidgetItem *item = new QListWidgetItem(m.toString());
        item->setData(Qt::UserRole, m.url);
        if (m.url.host() == fallBackHost) 
        {
            QFont font = item->font();
            font.setBold(true);
            item->setFont(font);
            qDebug() << "identified fall back host" << m.url.host();
        }
        ui.downloadMirror->addItem(item);
        if (!currentMirror.isEmpty() && m.url.host() == currentMirror.host())
            mirrorIndex = ui.downloadMirror->count()-1;
    }
    if (currentMirror.isEmpty())
        ui.downloadMirror->setCurrentRow(0);
    // current mirror not empty and not found in list 
    else if (mirrorIndex == -1)
    {
        QListWidgetItem *item = new QListWidgetItem(currentMirror.toString());
        item->setData(Qt::UserRole, currentMirror);
        ui.downloadMirror->addItem(item);
        ui.downloadMirror->setCurrentRow(ui.downloadMirror->count()-1);
    }
    else
        ui.downloadMirror->setCurrentRow(mirrorIndex);

    QListWidgetItem *item = ui.downloadMirror->currentItem();
    ui.downloadMirror->sortItems();
    ui.downloadMirror->setCurrentItem(item);

    connect(ui.addMirrorButton,SIGNAL(clicked()), this, SLOT(addNewMirrorClicked()));
}

void MirrorSettingsPage::performAction()
{
    if (m_failed)
        wizard()->back();                    
}

bool MirrorSettingsPage::validatePage()
{
    Settings &s = Settings::instance();
    QUrl data; 
    if (ui.downloadMirror->currentItem())
        data = ui.downloadMirror->currentItem()->data(Qt::UserRole).toUrl();
    qDebug() << data;
    if (data.scheme() == "file")
    {
        InstallerEngine::setInstallMode(InstallerEngine::localInstall);
        InstallerEngine::defaultConfigURL = data.toString();
        s.setMirrorWithReleasePath(data);
        qDebug() << "local install mirror set to" << data;
    }
    else if (!data.isEmpty() && QUrl(s.mirror()) != data)
        s.setMirror(data.toString());
    return true;
}

void MirrorSettingsPage::cleanupPage()
{
    disconnect(ui.addMirrorButton,SIGNAL(clicked()), this, SLOT(addNewMirrorClicked()));
}

bool MirrorSettingsPage::isComplete()
{
    return ui.downloadMirror->currentItem();
}

void MirrorSettingsPage::addNewMirrorClicked()
{
    if (!ui.addServerURL->text().isEmpty()) {
        QString text = ui.addServerURL->text().trimmed();
        ui.addServerURL->setText(text);
        QUrl url(text);
        
        if (!url.isValid() || !(url.scheme() == "http" || url.scheme() == "https" || url.scheme() == "ftp"  || url.scheme() == "file"))
        {
            setStatus(tr("Warning: This URL is not valid"));
            return;
        }
        
        QList<QListWidgetItem *> list = ui.downloadMirror->findItems (text, Qt::MatchExactly);
        if (list.size() > 0) 
        {
            setStatus(tr("Warning: This URL is already in the list"));
            return;
        }
        QListWidgetItem *item = new QListWidgetItem(text);
        item->setData(Qt::UserRole, url);
        ui.downloadMirror->addItem(item);
        ui.downloadMirror->setCurrentRow(ui.downloadMirror->count()-1);
        ui.addServerURL->setText(QString());
    }
}
