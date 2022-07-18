/****************************************************************************
**
** Copyright (C) 2005-2006  Ralf Habacker. All rights reserved.
**
** This file is part of the KDE installer for windows
**
** SPDX-License-Identifier: GPL-2.0-only
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


