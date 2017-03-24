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

#include "database.h"
#include "installdirectorypage.h"
#include "misc.h"

#include <QFileDialog>
#include <QMessageBox>
#include <QString>

InstallDirectoryPage::InstallDirectoryPage() : InstallWizardPage(0)
{
    ui.setupUi(this);
    setTitle(windowTitle());
    setSubTitle(statusTip());

    connect( ui.rootPathSelect,SIGNAL(clicked()),this,SLOT(rootPathSelectClicked()) );
}

void InstallDirectoryPage::initializePage()
{
    const Settings &s = Settings::instance();
#if QT_VERSION >= 0x050000
    ui.rootPathEdit->setText(QDir::toNativeSeparators(s.installDir()));
#else
    ui.rootPathEdit->setText(QDir::convertSeparators(s.installDir()));
#endif
}

bool InstallDirectoryPage::isComplete()
{
    return !ui.rootPathEdit->text().isEmpty();
}

bool InstallDirectoryPage::validatePage()
{
    Settings &s = Settings::instance();
    if (!isDirWritable(ui.rootPathEdit->text()))
    {
        QMessageBox::critical(this,
                              tr("Error"),
                              tr("You do not have write permissions on the selected directory. Please select another one."),
                              QMessageBox::Ok);
        return false;
    }
    s.setInstallDir(ui.rootPathEdit->text());
    engine->setRoot(s.installDir());
    return true;
}

void InstallDirectoryPage::rootPathSelectClicked()
{
    QString fileName = ui.rootPathEdit->text();
    if( fileName.isEmpty() )
      fileName = QString::fromLocal8Bit( qgetenv( "ProgramFiles" ) );
    fileName = QFileDialog::getExistingDirectory(this,
                       tr("Select Root Installation Directory"),
                       fileName,
                       QFileDialog::ShowDirsOnly| QFileDialog::DontResolveSymlinks);
    if(!fileName.isEmpty())
        ui.rootPathEdit->setText(QDir::toNativeSeparators(fileName));
}
