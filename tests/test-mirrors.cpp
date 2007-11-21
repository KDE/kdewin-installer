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
#include <QtDebug>
#include <QCoreApplication>

#include "mirrors.h"


int main(int argc, char *argv[])
{
    QCoreApplication app(argc, argv);

    Mirrors m0(Mirrors::KDE);
    if (m0.fetch( QUrl("http://download.kde.org/mirrorstatus.html")) )
        qDebug() << m0.mirrors();

    Mirrors m(Mirrors::Cygwin);
    if (m.fetch( QUrl("http://www.cygwin.com/mirrors.lst")) )
        qDebug() << m.mirrors();

    Mirrors m1(Mirrors::Cygwin);
    if (m1.fetch(QUrl("http://download.cegit.de/kde-windows/mirrors.lst")) )
        qDebug() << m1.mirrors();
    return 0;

}
