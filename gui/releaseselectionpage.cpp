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
#include "releases.h"
#include "releaseselectionpage.h"
#include "installerdialogs.h"

#include <QTimer>

ReleaseSelectionPage::ReleaseSelectionPage() : InstallWizardPage(0)
{
    ui.setupUi(this);
    setTitle(windowTitle());
    setSubTitle(statusTip());
}

void ReleaseSelectionPage::initializePage()
{
    QUrl mirror = Settings::instance().mirror();

    InstallerDialogs::instance().enableDownloadProgressDialog(this, tr("Downloading Releases"));
    m_failed = false;
    
    Releases releases;
    if ( !releases.fetch(mirror) ) 
    {
        qCritical() << "could not load mirrors from" << mirror;
        m_failed = true;
    }
    
    ui.releaseList->clear();
    InstallerDialogs::instance().disableDownloadProgressDialog();
    if (m_failed)
        return;
    QUrl currentMirror = Settings::instance().mirrorWithReleasePath();
    int mirrorIndex = -1;
    if (currentMirror.isEmpty())
        currentMirror = releases.realBaseURL().toString() + "stable/latest/";
    qDebug() << "current mirror with release" << currentMirror;
    
    Q_FOREACH(const MirrorReleaseType &m, releases.releases())
    {
        qDebug() << "adding release" << m.name << "with url" << m.url;
        QString name = m.type == Undefined ? m.name : m.toString();
        QListWidgetItem *item = new QListWidgetItem(name);
        item->setData(Qt::UserRole, m.url);
        ui.releaseList->addItem(item);
        if (!currentMirror.isEmpty() && m.url == currentMirror)
            mirrorIndex = ui.releaseList->count()-1;
    }

    if (currentMirror.isEmpty())
        ;// do nothing
    // current release not empty and not found in list 
    else if (mirrorIndex == -1)
    {
        qWarning() << "release" << currentMirror << "not found on this server"; 
    }
    else
        ui.releaseList->setCurrentRow(mirrorIndex);

    ui.releaseList->sortItems();

    if (releases.releases().size() == 1)
    {
        ui.releaseList->setCurrentRow(0);
        QTimer::singleShot(10, wizard(), SLOT(next()));
    }
}

void ReleaseSelectionPage::performAction()
{
    if (m_failed)
        wizard()->back();                    
}

bool ReleaseSelectionPage::validatePage()
{
    Settings &s = Settings::instance();
    QUrl data; 
    if (ui.releaseList->currentItem())
        data = ui.releaseList->currentItem()->data(Qt::UserRole).toUrl();
    qDebug() << data;
    if (!data.isEmpty()) 
    {
        s.setMirrorWithReleasePath(data);
        engine->setConfigURL(data);
        return true;
    }
    return false;
}

void ReleaseSelectionPage::cleanupPage()
{
}

bool ReleaseSelectionPage::isComplete()
{
    return ui.releaseList->currentItem();
}
