/****************************************************************************
**
** Copyright (C) 2007  Patrick Spendrin
** All rights reserved.
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
#include <QFileInfo>
#include <QUrl>
#include <QDir>
#include <cstdlib>

#include "misc.h"
#include <windows.h>
#include <windowsx.h>
#include <objbase.h>
#include <shlobj.h>
#include <shlwapi.h>
#include <initguid.h>

static void printHelp( const QString &addInfo )
{
    QTextStream ts( stderr );
    ts << QDir::convertSeparators( QCoreApplication::applicationFilePath() );
    if( !addInfo.isEmpty() )
        ts << ": " << addInfo;
    ts << "\n";
    ts << "Options: \t" << "[desktop file] path to the .desktop file that will be translated according to the content"
       << "\n\t\t"      << "both a simple file or a folder containing the file are valid"
       << "\n\t\t"      << "--help giving you this help"
       << "\n";

    ts.flush();
    exit(1);
}

int main( int argc, char *argv[] )
{
    QCoreApplication app( argc, argv );

    QStringList args = app.arguments();
    QString category, installpath, root;
    QFileInfo rootInfo;
    QDir rootDir;
    bool remove = false;

    args.removeAt( 0 );   // name of executable

    int idx = args.indexOf( "--help" );
    if( idx != -1 && idx < args.count() - 1 ) {
        printHelp( QString() );
    }

    idx = args.indexOf( "--category" );
    if( idx != -1 && idx < args.count() - 1 ) {
        category = args[ idx + 1 ];
        args.removeAt( idx + 1 );
        args.removeAt( idx );
    } else {
        category = "Miscelleanous";
        qDebug() << QString( "category missing; assuming category %1" ).arg( category );
    }

    idx = args.indexOf( "--installpath" );
    if( idx != -1 && idx < args.count() - 1 ) {
        installpath = args[ idx + 1 ];
        args.removeAt( idx + 1 );
        args.removeAt( idx );
    } else {
        installpath = QString( getenv( "KDEROOT" ) ) + QDir::separator() + QString( "bin" ) + QDir::separator();
        qDebug() << QString( "installpath missing; assuming installpath %1" ).arg( installpath );
    }

    idx = args.indexOf( "--build" );
    if( idx != -1 && idx < args.count() - 1 ) {
        args.removeAt( idx );
    }
    
    idx = args.indexOf( "--remove" );
    if( idx != -1 && idx < args.count() - 1 ) {
        args.removeAt( idx );
        remove = true;
    }

    if( args.count() > 0 ) {
        root = args[ 0 ];
        rootInfo = QFileInfo( root );
        if( remove ) {
            return removeStartMenuEntries( root, category );
        } else {
            return createStartMenuEntries( root, installpath, category );
        }
    } else {
        printHelp(QString());
    }
    return 0;
}
