/****************************************************************************
**
** Copyright (C) 2009 Ralf Habacker. All rights reserved.
**
** This file is part of the KDE installer for windows
**
** SPDX-License-Identifier: LGPL-2.0-only
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
    QList<InstallFile> m_srcFileList;
    QList<InstallFile> m_dbgFileList;
    bool m_debug;
};

#endif
