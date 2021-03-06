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

#include <QtGlobal>
#include <QCoreApplication>
#include <QtDebug>

#include  "downloader.h"
#include  "downloaderprogress.h"

int main(int argc, char *argv[])
{
    QCoreApplication app(argc, argv);

    Downloader *d = Downloader::instance();

    qDebug() << "download return code" << d->fetch(QUrl("https://download.kde.org/Attic/4.10.2/win32/config.txt"),"config.txt");
    qDebug() << "download return code" << d->fetch(QUrl("http://go.microsoft.com/fwlink/?LinkId=51410&clcid=0x409"),"vcexpress.exe");
    qDebug() << "download return code" << d->fetch(QUrl("ftp://ftp.gwdg.de/pub/linux/kde/Attic/4.10.2/win32/config.txt"),"config-ftp.txt");
    qDebug() << "download return code" << d->fetch(QUrl("http://ftp.gwdg.de/pub/linux/kde/Attic/4.10.2/win32/"),"config-dir.txt");
    qDebug() << "download return code" << d->fetch(QUrl("ftp://ftp.gwdg.de/pub/linux/kde/Attic/4.10.2/win32/"),"config-ftp-dir.txt");
    return 0;
}
