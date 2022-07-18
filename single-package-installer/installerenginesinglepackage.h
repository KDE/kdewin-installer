/****************************************************************************
**
** Copyright (C) 2010 Ralf Habacker ralf.habacker@freenet.de
**
** This file is part of the KDE installer for windows
**
** SPDX-License-Identifier: LGPL-2.0-only
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
