/****************************************************************************
**
** Copyright (C) 2009 Ralf Habacker. All rights reserved.
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

#ifndef XMLTEMPLATEPACKAGER_H
#define XMLTEMPLATEPACKAGER_H

#include "misc.h"
#include "packager.h"

class XmlData;
class XmlModule;
class XmlPackage;

#include <QList>

class XmlTemplatePackager: public Packager
{
public:
    XmlTemplatePackager(const QString &packageName, const QString &packageVersion,const QString &notes=QString());
    ~XmlTemplatePackager();
    virtual bool parseConfig(const QString &fileName);
    virtual bool generatePackageFileList(QList<InstallFile> &fileList, Packager::Type type, const QString &root);
    virtual bool makePackage(const QString &dir, const QString &destdir=QString(), bool bComplete=false);
    QString getDescription(Packager::Type type);

protected:
    XmlData *m_data;
    XmlPackage *m_currentPackage;
    XmlModule *m_currentModel;
    QList<InstallFile> m_fileList;
    bool m_debug;
};

#endif
