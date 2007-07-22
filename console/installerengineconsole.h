/****************************************************************************
**
** Copyright (C) 2005-2007 Ralf Habacker. All rights reserved.
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

#ifndef INSTALLERENGINECONSOLE_H
#define INSTALLERENGINECONSOLE_H

#include  "installerengine.h"

class InstallerEngineConsole : public InstallerEngine
{
public:
    InstallerEngineConsole(DownloaderProgress *progressBar,InstallerProgress *instProgressBar);
    void queryPackages(const QString &pkgName=QString(), bool listFiles=false);
    void listPackages(const QString &title);
    bool downloadPackages(const QStringList &packages, const QString &category=QString());
    bool installPackages(const QStringList &packages, const QString &category=QString());
private:
};

#endif
