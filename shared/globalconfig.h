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

#ifndef GLOBALCONFIG_H
#define GLOBALCONFIG_H

#include "site.h"
#include "package.h"
class Downloader;

class GlobalConfig {
    public:
        GlobalConfig(const QString &url, Downloader &downloader);
        ~GlobalConfig();
        QList<Site*> *sites()  { return &m_sites;   } 
        QList<Package*> *packages() { return &m_packages; }
        void dump(const QString &title=QString());
    protected:
        bool parseFromFile(const QString &fileName);
        bool parseFromByteArray(const QByteArray &ba);
        bool parse(QIODevice *ioDev);

    private:
        QList <Site*> m_sites; 
        QList <Package *> m_packages;
};

#endif
