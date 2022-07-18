/****************************************************************************
**
** Copyright (C) 2006-2008 Ralf Habacker <ralf.habacker@freenet.de>
**
** All rights reserved.
**
** This file is part of the KDE installer for windows
**
** SPDX-License-Identifier: LGPL-2.0-only
**
****************************************************************************/

#ifndef REGISTRY_H
#define REGISTRY_H

#include <QString>
#include <QVariant>

typedef enum { hKEY_CURRENT_USER, hKEY_LOCAL_MACHINE, hKEY_CLASSES_ROOT }  RegKey;
typedef enum { qt_unknown = -1, qt_String, qt_ExpandedString, qt_DWORD, qt_BINARY, qt_MultiString } RegType;

QVariant getWin32RegistryValue(RegKey akey, const QString& subKey, const QString& item, bool *ok = 0);
bool setWin32RegistryValue(const RegKey& akey, const QString& subKey, const QString& item, const QVariant& value, RegType pqvType = qt_unknown);
bool delWin32RegistryValue(const RegKey& akey, const QString& subKey);

#endif
