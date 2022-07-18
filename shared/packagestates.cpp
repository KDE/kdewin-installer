/****************************************************************************
**
** Copyright (C) 2005-2007 Ralf Habacker. All rights reserved.
**
** This file is part of the KDE installer for windows
**
** SPDX-License-Identifier: LGPL-2.0-only
**
****************************************************************************/

#include "packagestates.h"
#include "database.h"
#include "typehelper.h"

// @TODO add reference counter to be able to unselected indirect dependencies

void PackageStates::setState(const QString &pkgName, const QString & pkgVersion, FileTypes::Type type, stateType state)
{
    QString key = getKey(pkgName,pkgVersion);
    PackageFlags value;
    if (m_states.contains(key))
        value = m_states[key];

    switch(type) {
        case FileTypes::BIN: value.bin = state; break;
        case FileTypes::LIB: value.lib = state; break;
        case FileTypes::DOC: value.doc = state; break;
        case FileTypes::SRC: value.src = state; break;
        case FileTypes::DBG: value.dbg = state; break;
        case FileTypes::ALL: value.all = state; break;
        case FileTypes::ANY: value.bin = value.lib = value.doc = value.src = value.dbg = value.all = state;
        default: break;
    }
    m_states[key] = value;
}

void PackageStates::setState(const Package *pkg, FileTypes::Type type, stateType state)
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

stateType PackageStates::getState(const QString &pkgName, const QString &pkgVersion, FileTypes::Type type)
{
    QString key = getKey(pkgName,pkgVersion);

    if (!m_states.contains(key))
        return _Nothing;
    PackageFlags value = m_states[key];

    switch(type) {
        case FileTypes::BIN: return value.bin;
        case FileTypes::LIB: return value.lib;
        case FileTypes::DOC: return value.doc;
        case FileTypes::SRC: return value.src;
        case FileTypes::DBG: return value.dbg;
        case FileTypes::ALL: return value.all;
        default: return _Nothing;
    }
}
stateType PackageStates::getState(const Package* pkg, FileTypes::Type type)
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

QList <Package *>PackageStates::packages(Database *database)
{
    QList <Package *> packages;
    PackageStatesType::iterator i = m_states.begin();
    for (;i != m_states.end(); ++i)
    {
        Package *p = database->getPackage(i.key());
        if (p)
            packages.append(p);
    }
    return packages;
}

QDebug operator<<(QDebug out, const PackageStates &c)
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
