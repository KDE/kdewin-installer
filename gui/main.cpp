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
#include "misc.h"

#include <QApplication>
#include <QPlastiqueStyle>

int main(int argc, char *argv[])
{
    setMessageHandler();
    QApplication app(argc, argv);

    // check if download url is given on command line
    QString param = QApplication::arguments().at(1);
    if (param.startsWith("file:") || param.startsWith("http:") || param.startsWith("ftp:"))
        InstallerEngine::defaultConfigURL = param;

    InstallWizard *wizard = new InstallWizard();
    int ret = wizard->exec();
    delete wizard;
    return ret;
}
