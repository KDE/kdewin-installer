/****************************************************************************
**
** Copyright (C) 2007  Christian Ehrlicher <ch.ehrlicher@gmx.de>.
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

#include "packager.h"

static void printHelp(const QString &addInfo)
{
    // too lazy atm 
    
    exit(1);    
}

int main(int argc, char *argv[])
{
    QCoreApplication app(argc, argv);

    QStringList args = app.arguments();
    QString root, version;
    QFileInfo fi;
    
    int idx = args.indexOf("--root");
    if(idx != -1 && idx < args.count() -1) {
        root = args[idx + 1];
        fi = QFileInfo(root);
    }

    idx = args.indexOf("--version");
    if(idx != -1 && idx < args.count() -1)
        version = args[idx + 1];

    if(root.isEmpty())
       printHelp("--root not specified");
    if(version.isEmpty())
       printHelp("--version not specified");
    
    if(!fi.isDir() || !fi.isReadable())
       printHelp(QString("Root path %1 is not accessible").arg(root));
    

    Packager packager("Test", version, "Testnotes");
    // packager.generatePackageFileList()

    QStringList files;
    
    packager.generatePackageFileList(files, fi.filePath(), Packager::BIN);
    qDebug() << files.join("\n");
    packager.makePackage(fi.filePath());

    return 0;
}

