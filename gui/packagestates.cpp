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
    QString key = getKey(pkgName,pkgVersion);
    PackageFlags value;
    if (m_states.contains(key))
        value = m_states[key];

    switch(type) {
        case Package::BIN: value.bin = state; break;
        case Package::LIB: value.lib = state; break;
        case Package::DOC: value.doc = state; break;
        case Package::SRC: value.src = state; break;
        case Package::ALL: value.all = state; break;
        case Package::ANY: value.bin = value.lib = value.doc = value.src = value.all = state;
        default: break;
    }
    m_states[key] = value;
}

void PackageStates::setState(Package *pkg, Package::Type type, stateType state)
{
    setState(pkg->name(),pkg->version().toString(),type,state);
}

#if 0
bool PackageStates::setStates(const QList <Package *>&list, stateType state)
{
    QList<Package*>::ConstIterator it = list.constBegin();
    for (; it != list.constEnd(); ++it)
    {
        Package *p = *it;
        if (p->isInstalled(Package::BIN))
            setState(p->name(),p->version(),Package::BIN,state);
        if (p->isInstalled(Package::LIB))
            setState(p->name(),p->version(),Package::LIB,state);
        if (p->isInstalled(Package::DOC))
            setState(p->name(),p->version(),Package::DOC,state);
        if (p->isInstalled(Package::SRC))
            setState(p->name(),p->version(),Package::SRC,state);
    }
    return true;
}
#endif

#if 0
bool PackageStates::setPresentState(const QList <Package *>&list)
{
    QList<Package*>::ConstIterator it = list.constBegin();
    for (; it != list.constEnd(); ++it)
    {
        Package *p = *it;
        if (p->isInstalled(Package::BIN))
            addState(p->name(),p->version(),Package::BIN,state);
        if (p->isInstalled(Package::LIB))
            addState(p->name(),p->version(),Package::LIB,state);
        if (p->isInstalled(Package::DOC))
            addState(p->name(),p->version(),Package::DOC,state);
        if (p->isInstalled(Package::SRC))
            addState(p->name(),p->version(),Package::SRC,state);
    }
    return true;
}

#endif

stateType PackageStates::getState(QString pkgName, QString pkgVersion, Package::Type type)
{
    QString key = getKey(pkgName,pkgVersion);

    if (!m_states.contains(key))
        return _Nothing;
    PackageFlags value = m_states[key];

    switch(type) {
        case Package::BIN: return value.bin;
        case Package::LIB: return value.lib;
        case Package::DOC: return value.doc;
        case Package::SRC: return value.src;
        case Package::ALL: return value.all;
        default: return _Nothing;
    }
}
stateType PackageStates::getState(Package* pkg, Package::Type type)
{
    return getState(pkg->name(),pkg->version().toString(),type);
}

void PackageStates::clear()
{
    m_states.clear();
}

QList <Package *>PackageStates::packages(PackageList *list)
{
    QList <Package *> packages;
    PackageStatesType::iterator i = m_states.begin();
    for (;i != m_states.end(); ++i)
    {
        Package *p = list->getPackage(i.key());
        if (p)
            packages.append(p);
    }
    return packages;
}

QDebug &operator<<(QDebug &out, const PackageStates &c)
{
    PackageStatesType::const_iterator i = c.m_states.begin();
    for (;i != c.m_states.end(); ++i)
    {
        if (i.key().isEmpty())
            continue;
        out << i.key() + ":"
            << "all" << c.m_states[i.key()].all
            << "bin" << c.m_states[i.key()].bin
            << "lib" << c.m_states[i.key()].lib
            << "doc" << c.m_states[i.key()].doc
            << "src" << c.m_states[i.key()].src;
    }
    return out;
}
