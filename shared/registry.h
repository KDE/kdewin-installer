/****************************************************************************
**
** Copyright (C) 2006-2008 Ralf Habacker <ralf.habacker@freenet.de>
**
** All rights reserved.
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

#ifndef REGISTRY_H
#define REGISTRY_H

#include <QString>
#include <QVariant>

typedef enum RegKey { hKEY_CURRENT_USER, hKEY_LOCAL_MACHINE, hKEY_CLASSES_ROOT };
typedef enum RegType { qt_unknown = -1, qt_String, qt_ExpandedString, qt_DWORD, qt_BINARY, qt_MultiString };
QVariant getWin32RegistryValue(RegKey akey, const QString& subKey, const QString& item, bool *ok = false);
bool setWin32RegistryValue(const RegKey& akey, const QString& subKey, const QString& item, const QVariant& value, RegType pqvType = qt_unknown);
bool delWin32RegistryValue(const RegKey& akey, const QString& subKey);

#endif
