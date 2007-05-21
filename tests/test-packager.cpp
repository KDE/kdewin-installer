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

#include <stdio.h>

#include <QtDebug>
#include <QCoreApplication>
#include <QFile>

#include "packager.h"

int main(int argc, char *argv[])
{
    QCoreApplication app(argc, argv);
    QStringList args = QCoreApplication::arguments();

    QString dbusDir = QFile::decodeName(qgetenv("PROGRAMFILES"));
    QString dir = args.size() > 1 ? args[1] : dbusDir + "/dbus" ;
    QString name = args.size() > 2 ? args[2] : "test" ;
    QString version = args.size() > 3 ? args[3] : "1.0.1" ;
    QString notes = args.size() > 4 ? args[4] : QString() ;
    QList<InstallFile> result;
    
    Packager packager(name,version,notes); 
        
    qDebug() << "BIN packages list";
    packager.generatePackageFileList(result,Packager::BIN,dir);
    for(int i = 0; i < result.count(); i++)
        qDebug() << result[i].inputFile << " " << result[i].outputFile;

    qDebug() << "LIB packages list";
    packager.generatePackageFileList(result,Packager::LIB,dir);
    for(int i = 0; i < result.count(); i++)
        qDebug() << result[i].inputFile << " " << result[i].outputFile;

    qDebug() << "DOC packages list";
    packager.generatePackageFileList(result,Packager::DOC,dir);
    for(int i = 0; i < result.count(); i++)
        qDebug() << result[i].inputFile << " " << result[i].outputFile;

    qDebug() << "SRC packages list";
    packager.generatePackageFileList(result,Packager::SRC,dir);
    for(int i = 0; i < result.count(); i++)
        qDebug() << result[i].inputFile << " " << result[i].outputFile;

    packager.stripFiles(dir);

    packager.makePackage(dir);


    return 0;
}


