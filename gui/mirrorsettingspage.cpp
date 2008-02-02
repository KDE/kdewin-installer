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

MirrorSettingsPage::MirrorSettingsPage() : InstallWizardPage(0)
{
    ui.setupUi(this);
    setTitle(windowTitle());
    setSubTitle(statusTip());

    QVBoxLayout *layout = new QVBoxLayout;
    layout->addWidget(statusLabel,1,Qt::AlignBottom);
    setLayout(layout);

}

void MirrorSettingsPage::initializePage()
{
    Settings &s = Settings::instance();
    s.setSkipBasicSettings(true);
    Mirrors &mirrors = Mirrors::instance();
    if (mirrors.mirrors().size() == 0)
    {
        /// @TODO add vivible progress bar
        if ( !mirrors.fetch(Mirrors::Cygwin, QUrl("http://download.cegit.de/kde-windows/mirrors.lst")) )
        {
            qCritical() << "could not load mirrors";
            // display warning box
        }
    }
    ui.downloadMirror->clear();

    QUrl currentMirror = QUrl(s.mirror());

    int mirrorIndex = -1;
    foreach(MirrorType m, mirrors.mirrors())
    {
        qDebug() << m.toString();
        QListWidgetItem *item = new QListWidgetItem(m.toString());
        item->setData(Qt::UserRole, m.url);
        ui.downloadMirror->addItem(item);
        if (!currentMirror.isEmpty() && m.url == currentMirror)
            mirrorIndex = ui.downloadMirror->count()-1;
    }
    if (mirrorIndex == -1)
    {
        QListWidgetItem *item = new QListWidgetItem(currentMirror.toString());
        item->setData(Qt::UserRole, currentMirror);
        ui.downloadMirror->addItem(item);
        ui.downloadMirror->setCurrentRow(ui.downloadMirror->count()-1);
    }
    else
        ui.downloadMirror->setCurrentRow(mirrorIndex);

    ui.downloadMirror->sortItems();
    connect(ui.addMirrorButton,SIGNAL(clicked()), this, SLOT(addNewMirrorClicked()));
}

int MirrorSettingsPage::nextId() const
{
    return InstallWizard::packageSelectorPage;
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
