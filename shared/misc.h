/****************************************************************************
**
** Copyright (C) 2006-2007 Ralf Habacker. All rights reserved.
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

#ifndef MISC_H
#define MISC_H

#include <QRegExp>
#include <QString>
#include <QStringList>

bool generateFileList(QStringList &result, const QString &root, const QString &subdir, const QString &filter, const QList<QRegExp> &excludeList);
bool generateFileList(QStringList &result, const QString &root, const QString &subdir, const QString &filter, const QString &exclude = QString());

bool createStartMenuEntries(const QString &dir, const QString &category=QString());
bool removeStartMenuEntries(const QString &dir, const QString &category=QString());

typedef enum RegKey { hKEY_CURRENT_USER, hKEY_LOCAL_MACHINE, hKEY_CLASSES_ROOT };
QVariant getWin32RegistryValue(RegKey key, const QString& subKey, const QString& item, bool *ok = false);

#endif
