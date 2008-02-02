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

#include <QDir>
#include <QString>
#include <QFileDialog>

#include "database.h"
#include "installdirectorypage.h"

InstallDirectoryPage::InstallDirectoryPage() : InstallWizardPage(0)
{
    ui.setupUi(this);
    setTitle(windowTitle());
    setSubTitle(statusTip());

    QVBoxLayout *layout = new QVBoxLayout;
    layout->addWidget(statusLabel,1,Qt::AlignBottom);
    setLayout(layout);
    connect( ui.rootPathSelect,SIGNAL(clicked()),this,SLOT(rootPathSelectClicked()) );
}

void InstallDirectoryPage::initializePage()
{
    Settings &s = Settings::instance();
    ui.rootPathEdit->setText(QDir::convertSeparators(s.installDir()));
}

bool InstallDirectoryPage::isComplete()
{
    return !ui.rootPathEdit->text().isEmpty();
}

int InstallDirectoryPage::nextId() const
{
    return InstallWizard::installTypePage;
}

bool InstallDirectoryPage::validatePage()
{
    Settings &s = Settings::instance();
    s.setInstallDir(ui.rootPathEdit->text());

    return true;
}

void InstallDirectoryPage::rootPathSelectClicked()
{
    QString fileName = QFileDialog::getExistingDirectory(this,
                       tr("Select Root Installation Directory"),
                       "",
                       QFileDialog::ShowDirsOnly| QFileDialog::DontResolveSymlinks);
    if(!fileName.isEmpty())
        ui.rootPathEdit->setText(QDir::toNativeSeparators(fileName));
}


#include "installdirectorypage.moc"
