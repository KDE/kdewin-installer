/****************************************************************************
**
** Copyright (C) 2010 Ralf Habacker ralf.habacker@freenet.de
**
** This file is part of the KDE installer for windows
**
** SPDX-License-Identifier: LGPL-2.0-only
**
****************************************************************************/

#include <QApplication>

#include "installerdialog.h"

int main(int argc, char *argv[])
{
    Q_INIT_RESOURCE(installerdialog);

    QApplication app(argc, argv);
    InstallerDialog dialog;
    dialog.show();
    return app.exec();
}
