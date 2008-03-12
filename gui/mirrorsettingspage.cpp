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
#include "mirrors.h"
#include "mirrorsettingspage.h"
#include "installerdialogs.h"

#if 0
QUrl mirrorListURL("http://www.kde.org/mirrors/kdemirrors.list");
Mirrors::Type mirrorListType = Mirrors::KDE;
QString mirrorReleasePath = "unstable/4.0.61/win32";
#else
QUrl mirrorListURL("http://downloads.sourceforge.net/kde-windows/mirrors.lst");
Mirrors::Type mirrorListType = Mirrors::Cygwin;
QString mirrorReleasePath = "";
#endif

QUrl fallBackMirrorURL("http://webdev.cegit.de/snapshots/kde-windows/mirrors.lst");
Mirrors::Type fallBackMirrorType = Mirrors::Cygwin;
QString fallBackMirrorReleasePath = "";

MirrorSettingsPage::MirrorSettingsPage() : InstallWizardPage(0)
{
    ui.setupUi(this);
    setTitle(windowTitle());
    setSubTitle(statusTip());
}

void MirrorSettingsPage::initializePage()
{
    Settings &s = Settings::instance();
    s.setSkipBasicSettings(true);
    Mirrors &mirrors = Mirrors::instance();
    InstallerDialogs::instance().downloadProgressDialog(this,true,tr("Downloading Mirror List"));

    if (mirrors.mirrors().size() == 0)
    {
        /// @TODO add vivible progress bar
        if ( !mirrors.fetch(mirrorListType, mirrorListURL, mirrorReleasePath) ) 
        {
            qCritical() << "could not load mirrors from" << mirrorListURL;
            /// @TODO add vivible progress bar
            if ( !mirrors.fetch(fallBackMirrorType, fallBackMirrorURL, fallBackMirrorReleasePath) )
            {
                qCritical() << "could not load fallback mirror list from" << fallBackMirrorURL;
                // display warning box
            }
        }
    }
    ui.downloadMirror->clear();
    InstallerDialogs::instance().downloadProgressDialog(this,false);

    QUrl currentMirror = QUrl(s.mirror());

    int mirrorIndex = -1;
    Q_FOREACH(const MirrorType &m, mirrors.mirrors())
    {
        qDebug() << m.toString();
        QListWidgetItem *item = new QListWidgetItem(m.toString());
        item->setData(Qt::UserRole, m.url);
        ui.downloadMirror->addItem(item);
        if (!currentMirror.isEmpty() && m.url == currentMirror)
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

bool MirrorSettingsPage::validatePage()
{
    Settings &s = Settings::instance();
    QUrl data; 
    if (ui.downloadMirror->currentItem())
        data = ui.downloadMirror->currentItem()->data(Qt::UserRole).toUrl();
    qDebug() << data;
    if (!data.isEmpty() && QUrl(s.mirror()) != data)
        s.setMirror(data.toString());

    s.setFirstRun(false);
    setSettingsButtonVisible(true);
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
        QUrl url(ui.addServerURL->text());
        
        if (!url.isValid() || !(url.scheme() == "http" || url.scheme() == "ftp"  || url.scheme() == "file"))
        {
            setStatus(tr("Warning: This URL is not valid"));
            return;
        }
        
        QList<QListWidgetItem *> list = ui.downloadMirror->findItems (ui.addServerURL->text(), Qt::MatchContains);
        if (list.size() > 0) 
        {
            setStatus(tr("Warning: This URL is already in the list"));
            return;
        }
        QListWidgetItem *item = new QListWidgetItem(ui.addServerURL->text());
        item->setData(Qt::UserRole, url);
        ui.downloadMirror->addItem(item);
        ui.downloadMirror->setCurrentRow(ui.downloadMirror->count()-1);
        ui.addServerURL->setText(QString());
    }
}

#include "mirrorsettingspage.moc"
