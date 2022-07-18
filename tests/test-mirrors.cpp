/****************************************************************************
**
** Copyright (C) 2005-2006  Ralf Habacker. All rights reserved.
**
** This file is part of the KDE installer for windows
**
** SPDX-License-Identifier: GPL-2.0-only
****************************************************************************/

#include <QtGlobal>
#include <QtDebug>
#include <QCoreApplication>

#include "mirrors.h"


int main(int argc, char *argv[])
{
    QCoreApplication app(argc, argv);

    Mirrors::Config www_kde_org_config(
        "http://www.kde.org/mirrors/kdemirrors.list",
        Mirrors::KDE,
        "",
        ""
    );

    Mirrors::Config winkde_org_mirrorConfig(
        "http://www.winkde.org/pub/kde/ports/win32/mirrors.lst",
        Mirrors::KDE,
        "",
        ""
    );

    Mirrors::Config cygwin_config(
        "http://www.cygwin.com/mirrors.lst",
        Mirrors::Cygwin,
        "",
        ""
    );
    
    Mirrors m0(winkde_org_mirrorConfig);
    if (m0.fetch())
        qDebug() << m0.mirrors();

    Mirrors m(cygwin_config);
    if (m.fetch())
        qDebug() << m.mirrors();

    Mirrors m1(winkde_org_mirrorConfig);
    if (m1.fetch())
        qDebug() << m1.mirrors();
    return 0;
}
