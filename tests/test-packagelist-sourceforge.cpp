/****************************************************************************
**
** Copyright (C) 2005-2006  Ralf Habacker. All rights reserved.
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

#include <QtDebug>
#include <QCoreApplication>

#include "packagelist.h"
#include "downloader.h"
#include "installer.h"


int main(int argc, char ** argv)
{
    QCoreApplication app(argc,argv);

    Downloader *downloader = Downloader::instance();
    PackageList packageList;

    qDebug() << "trying to download sourceforge related package list";
    downloader->fetch(QUrl("http://sourceforge.net/project/showfiles.php?group_id=23617"),"packages.html");

    if (!packageList.readFromFile("packages.html",PackageList::SourceForge))
    {
        qDebug() << "... failed ";
        return false;
    }
    packageList.listPackages();
    return 0;
}

