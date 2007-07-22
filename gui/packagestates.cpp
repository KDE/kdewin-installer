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

#include "packagestates.h"
// @TODO add reference counter to be able to unselected indirect dependencies

void PackageStates::setState(QString pkgName, QString pkgVersion, Package::Type type, stateType state)
{
    QString key = pkgName+'-'+pkgVersion;
    PackageFlags value;
    if (states.contains(key)) 
        value = states[key];
    
    switch(type) {
        case Package::BIN: value.bin = state; break;
        case Package::LIB: value.lib = state; break;
        case Package::DOC: value.doc = state; break;
        case Package::SRC: value.src = state; break;
        case Package::ALL: value.all = state; break;
    }
    states[key] = value;
}

stateType PackageStates::getState(QString pkgName, QString pkgVersion, Package::Type type  )
{
    QString key = pkgName+'-'+pkgVersion;
    
    if (!states.contains(key))
        return _Nothing;
    PackageFlags value = states[key];

    switch(type) {
        case Package::BIN: return value.bin;
        case Package::LIB: return value.lib;
        case Package::DOC: return value.doc;
        case Package::SRC: return value.src;
        case Package::ALL: return value.all;
        default: return _Nothing;
    }
}
