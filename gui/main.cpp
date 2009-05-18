/****************************************************************************
**
** Copyright (C) 2007-2008 Ralf Habacker
** Copyright (C) 2007-2008 Christian Ehrlicher <ch.ehrlicher@gmx.de>
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

#include "config.h"
#include "installwizard.h"
#include "installerengine.h"
#include "installerupdate.h"
#include "selfinstaller.h"
#include "debug.h"
#include "misc.h"

#include <QApplication>
#include <QLocale>
#include <QPlastiqueStyle>
#include <QTranslator>

int main(int argc, char *argv[])
{
    QApplication app(argc, argv);

#ifdef BUILD_TRANSLATIONS
    QTranslator translator;
    QString lang = QLocale::system().name().left(2);
    QString qmFile = exePath() + "/installer-gui-" + lang  + ".qm"; 
    bool result = translator.load(qmFile);
    qDebug() << "loading translations from" << qmFile << (result ? "okay" : "failed");    
    app.installTranslator(&translator);
#endif

    // check if download url is given on command line
    if(argc > 1) 
    {
        QString param = QApplication::arguments().at(1);
        if (param.startsWith("--finish-update"))
        {
            InstallerUpdate::instance().finish(QApplication::arguments(),2);
        }
        else if (param.startsWith("--handle-control-panel"))
        {
            InstallWizard::setTitlePage(InstallWizard::endUserInstallModePage);
        }
        else if (param.startsWith("file:") || param.startsWith("http:") || param.startsWith("ftp:"))
            InstallerEngine::defaultConfigURL = param;

        // setup local install mode 
        if (param.startsWith("file:"))
        {
            InstallerEngine::setInstallMode(InstallerEngine::localInstall);
            Settings::instance().setDownloadDir(param.replace("file:///",""));
        }
    }

    //
    // when installer is located in install root dir run it from a temporary location 
    // to avoid installer executable acess problem for update and removal, 
    //
    if (SelfInstaller::instance().isInstalled() && SelfInstaller::instance().isRunningFromInstallRoot())
    {
        QStringList args = QApplication::arguments(); 
        args.removeFirst();
        SelfInstaller::instance().runFromTemporayLocation(args);
    }

    setMessageHandler();

    InstallWizard *wizard = new InstallWizard();
    int ret = wizard->exec();
    delete wizard;
    return ret;
}
