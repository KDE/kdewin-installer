/****************************************************************************
**
** Copyright (C) 2008 Ralf Habacker ralf.habacker@freenet.de
**
** This file is part of the KDE installer for windows
**
** SPDX-License-Identifier: LGPL-2.0-only
**
****************************************************************************/

#include "hash.h"

#include <QCoreApplication>
#include <QFile>
#include <QStringList>
#include <QTextStream>
#include <QtDebug>

#include <stdio.h>

int main(int argc, char *argv[])
{
    QCoreApplication app(argc, argv);

    // check if download url is given on command line
    if(argc < 2) 
    {
        fprintf(stdout,"no file given"); 
        return -1;
    }
    QString input, output;
        
    if (QCoreApplication::arguments().at(1) == "-o" || QCoreApplication::arguments().at(1) == "--output")  
    {
        if (QCoreApplication::arguments().size() < 4)
        {
            fprintf(stderr,"not enough parameter given");
            return -2;
        }
        output = QCoreApplication::arguments().at(2);
        input = QCoreApplication::arguments().at(3);
    }
    else if (QCoreApplication::arguments().at(1) == "-c" || QCoreApplication::arguments().at(1) == "--check")  
    {
        if (QCoreApplication::arguments().size() < 3)
        {
            fprintf(stderr,"no filename given");
            return -2;
        }
        input = QCoreApplication::arguments().at(2);
        QFile file(input);
        if (!file.open(QIODevice::ReadOnly))
        {
            fprintf(stderr,"could not open sha1sum file");
            return -2;
        }
        QTextStream in(&file);
        QString line = in.readLine();
        QStringList cols = line.split(" ",QString::SkipEmptyParts);
        if (cols.size() != 2)
        {
            fprintf(stderr,"hash file has illegal format");
            return -3;
        }
        QByteArray givenHash = cols[0].toLatin1();
        QFile inFile(cols[1]);
        if (!inFile.open(QIODevice::ReadOnly))
        {
            fprintf(stderr,"could not open file %s", cols[1].toLatin1().data());
            return -4;
        }
        QByteArray computedHash = Hash::sha1(inFile).toHex();
        if (computedHash != givenHash)
        {
            fprintf(stderr,"wrong hash computed %s reference %s",computedHash.data(), givenHash.data());
            return -5;
        }   
        return 0;
    }

    else 
        input = QCoreApplication::arguments().at(1);

    QByteArray hashValue = Hash::sha1(input);
    if (!output.isEmpty())
    {
        FILE *f = fopen(output.toLatin1().data(),"w");
        fprintf(f,"%s  %s",hashValue.toHex().data(),input.toLatin1().data());
        fclose(f);
    }
    else 
    {
        fprintf(stdout,"%s  %s\n",hashValue.toHex().data(),input.toLatin1().data());
    }
    return 0;
}
