/****************************************************************************
**
** Copyright (C) 2006 Ralf Habacker. All rights reserved.
**
** This file is part of the KDE installer for windows
**
** This file may be used under the terms of the GNU General Public
** License version 2.0 as published by the Free Software Foundation
** and appearing in the file LICENSE.GPL included in the packaging of
** this file.  Please review the following information to ensure GNU
** General Public Licensing requirements will be met:
** http://www.trolltech.com/products/qt/opensource.html
**
** If you are unsure which license is appropriate for your use, please
** review the following information:
** http://www.trolltech.com/products/qt/licensing.html or contact the
** sales department at sales@trolltech.com.
**
** This file is provided AS IS with NO WARRANTY OF ANY KIND, INCLUDING THE
** WARRANTY OF DESIGN, MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE.
**
****************************************************************************/

#ifndef MISC_H
#define MISC_H

class QDir;
class QStringList;
class QRegExp;

bool generateFileList(QStringList &result, const QString &root, const QString &subdir, const QString &filter, const QList<QRegExp> &excludeList);
bool generateFileList(QStringList &result, const QString &root, const QString &subdir, const QString &filter, const QString &exclude = QString());

bool createStartMenuEntries(const QString &dir, const QString &category=QString());
bool removeStartMenuEntries(const QString &dir, const QString &category=QString());

#endif
