/****************************************************************************
**
** Copyright (C) 2005-2008  Ralf Habacker. All rights reserved.
**
** This file is part of the KDE installer for windows
**
** SPDX-License-Identifier: GPL-2.0-only
****************************************************************************/

#include <QtDebug>
#include <QCoreApplication>

#include "unpacker.h"

int main(int argc, char ** argv)
{
    QCoreApplication app(argc,argv);

    Unpacker *u = Unpacker::instance(); 
    
    if( argc == 3 ) {
        u->unpackFile( argv[1], argv[2] );
    } else {
        u->unpackFile("e:\\downloads\\kde\\OggDS0995.exe","e:\\downloads\\kde");
    }
    fprintf(stdout, "Unpack result: %s", qPrintable( u->lastError() ));

    return 0;
}
