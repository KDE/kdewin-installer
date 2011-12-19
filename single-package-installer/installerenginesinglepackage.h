/****************************************************************************
**
** Copyright (C) 2010 Ralf Habacker ralf.habacker@freenet.de
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

#ifndef INSTALLERENGINESINGLEPACKAGE_H
#define INSTALLERENGINESINGLEPACKAGE_H

#include  "installerengine.h"

class InstallerEngineSinglePackage : public InstallerEngine
{
public:
    InstallerEngineSinglePackage();

    void setRoot(const QString &root);
    bool withDevelopmentPackages()
    {
        return m_withDevelopmentPackages;
    }
    void setWithDevelopmentPackages(bool state)
    {
        m_withDevelopmentPackages = state;
    }
    bool downloadPackages(QList<Package*> &packagesToInstall);
    bool installPackages(QList<Package*> &packagesToInstall);

    bool init();

private:
    bool done;
    bool m_withDevelopmentPackages;
};

#endif
