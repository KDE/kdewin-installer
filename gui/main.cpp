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

#include "installwizard.h"
#include "installerengine.h"
#include "installerupdate.h"
#include "debug.h"

#include <QApplication>
#include <QPlastiqueStyle>

int main(int argc, char *argv[])
{
    QApplication app(argc, argv);

    // check if download url is given on command line
    if(argc > 1) 
    {
        QString param = QApplication::arguments().at(1);
        if (param.startsWith("--finish-update"))
        {
            InstallerUpdate::instance().finish(QApplication::arguments().at(2));
        }
        else if (param.startsWith("file:") || param.startsWith("http:") || param.startsWith("ftp:"))
            InstallerEngine::defaultConfigURL = param;

        // setup local install mode 
        if (param.startsWith("file:"))
        {
            InstallerEngine::setLocalInstall(true);
            Settings::instance().setDownloadDir(param.replace("file:///",""));
        }
    }
    setMessageHandler();

    InstallWizard *wizard = new InstallWizard();
    int ret = wizard->exec();
    delete wizard;
    return ret;
}
