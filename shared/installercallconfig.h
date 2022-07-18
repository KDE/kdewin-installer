/****************************************************************************
**
** Copyright (C) 2006-2009 Ralf Habacker. All rights reserved.
**
** This file is part of the KDE installer for windows
**
** SPDX-License-Identifier: LGPL-2.0-only
**
****************************************************************************/

#ifndef INSTALLERCALLCONFIG_H
#define INSTALLERCALLCONFIG_H

#include "misc.h"
#include "typehelper.h"

#include <QString>

/**
 InstallerCallConfig provides package related informations from the 
 installer filename which could be used to skip related wizard pages 
 or to implement specific gui applications. 
 
 - which package to install (e.g. umbrello)
 - which compiler (e.g vc90, mingw4)
 - which release type (e.g. stable,nightly,unstable)
 - which release version (e.g. 4.4.4)
 - which download server
 
 The filename has to build in the following manner: 

    setup-<packageName>-<compiler>-<releaseType>-<version>[-<mirror>].exe

 '-' in packagenames has to be replaced by '_'

  parts in [] are optional
*/

class InstallerCallConfig {
public:
    InstallerCallConfig(const QString &fileName=QString());
    bool isValid() { return key == "setup"; }

    static InstallerCallConfig &instance(); 

    QString packageName;
    ReleaseType releaseType; 
    QString version; 
    QString mirror; 
    CompilerTypes::Type compilerType;
    bool hasReleaseType;
    bool hasVersion;
    bool hasMirror;
    bool hasSDK;

protected:
    bool isLoaded; 
    QString key;
    QString installerBaseName;

};



#endif
