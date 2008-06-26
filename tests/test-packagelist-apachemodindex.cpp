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
#include "downloaderprogress.h"

int main(int argc, char *argv[])
{

    QCoreApplication app(argc, argv);

    Downloader *download = Downloader::instance();
    Site site;

    QStringList args = app.arguments();
    args.removeFirst();

    if (args.size() > 0) {
        foreach(QString arg, args) {
            QUrl url (arg);
            site.setURL(url);
            site.setName(arg);
            PackageList packageList;
            packageList.setCurrentSite(&site);
            packageList.setBaseURL(site.url());
            QString fileName = url.path().replace("/","_") + ".txt";
            download->fetch(site.url(),fileName);            
            if (!packageList.readFromFile(fileName,PackageList::ApacheModIndex)) {
                qDebug() << "parsing" << arg << "... failed ";
            }
            qDebug() << arg << packageList;
        }
    }
    else {
        site.setURL(QUrl("http://82.149.170.66/kde-windows/win32libs/"));
        site.setName("test");
        PackageList packageList;
        packageList.setCurrentSite(&site);
        packageList.setBaseURL(site.url());
        
        qDebug() << "trying to download win32 related package list";
        download->fetch(site.url(),"packages1.html");
        if (!packageList.readFromFile("packages1.html",PackageList::ApacheModIndex))
        {
            qDebug() << "... failed ";
            return 1;
        }
        qDebug() << "win32libs Package List" << packageList;

        PackageList packageList2;

        download->fetch(QUrl("http://software.opensuse.org/download/KDE:/KDE3/SUSE_Linux_10.1/noarch/"),"packages2.html");
        if (!packageList2.readFromFile("packages2.html",PackageList::ApacheModIndex))
        {
            qDebug() << "... failed ";
            return 1;
        }
        qDebug() << "KDE i18n Package List" << packageList2;

    }
    return 0;
}

