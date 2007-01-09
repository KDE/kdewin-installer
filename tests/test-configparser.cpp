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

#include "configparser.h"
#include "site.h"

int main(int argc, char *argv[])
{
    QCoreApplication app(argc, argv);

#if 0

    DownloaderProgress progress(0);
    Downloader download(true,&progress);

    qDebug() << "trying to download global configuration file";
    download.start("http://well-known-location-server/kde-installer/config.txt","config.txt");
#endif

    ConfigParser configParser;
    configParser.parseFromFile("config.txt");
    /*
     foreach (Site aSite, *configParser.Sites) {
      qDebug() << aSite.Name();
      qDebug() << aSite.URL();
      qDebug() << aSite.type();
     }
    */

    return 0;
}

