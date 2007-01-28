/****************************************************************************
**
** Copyright (C) 2007  Christian Ehrlicher <ch.ehrlicher@gmx.de>.
** Copyright (C) 2007  Ralf Habacker 
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
#include <QDir>

#include "packager.h"

static void printHelp(const QString &addInfo)
{
    QTextStream ts(stderr);
    ts << QDir::convertSeparators(QCoreApplication::applicationFilePath());
    if(!addInfo.isEmpty())
        ts << ": " << addInfo;
    ts << "\n";
    ts << "Options: \t" << "--name <packageName>"
       << "\n\t\t"      << "--root <path to package files>"
       << "\n\t\t"      << "--version <package version>"
       << "\n\t\t"      << "--strip <strip debug infos> (MinGW only)"
       << "\n\t\t"      << "--notes <additional notes for manifest files>"
	   << "\n\t\t"      << "--type type of package (mingw, msvc)"
       << "\n";

    ts.flush();
    exit(1);
}

int main(int argc, char *argv[])
{
    QCoreApplication app(argc, argv);

    QStringList args = app.arguments();
    QString name, root, version, notes, type;
    QFileInfo rootDir;
    bool strip = false;
    
    int idx = args.indexOf("--name");
    if(idx != -1 && idx < args.count() -1) {
        name = args[idx + 1];
        args.removeAt(idx + 1);
        args.removeAt(idx);
    }

    idx = args.indexOf("--type");
    if(idx != -1 && idx < args.count() -1) {
        name = args[idx + 1];
        args.removeAt(idx + 1);
        args.removeAt(idx);
    }

    idx = args.indexOf("--root");
    if(idx != -1 && idx < args.count() -1) {
        root = args[idx + 1];
        rootDir = QFileInfo(root);
        args.removeAt(idx + 1);
        args.removeAt(idx);
    }

    idx = args.indexOf("--version");
    if(idx != -1 && idx < args.count() -1) {
        version = args[idx + 1];
        args.removeAt(idx + 1);
        args.removeAt(idx);
    }

    idx = args.indexOf("--strip");
    if(idx != -1) {
        strip = 1;
    }

    idx = args.indexOf("--notes");
    if(idx != -1 && idx < args.count() -1) {
        notes = args[idx + 1];
        args.removeAt(idx + 1);
        args.removeAt(idx);
    }

    if(name.isEmpty())
       printHelp("--name not specified");
    if(root.isEmpty())
       printHelp("--root not specified");
    if(version.isEmpty())
       printHelp("--version not specified");
    
    if(!rootDir.isDir() || !rootDir.isReadable())
       printHelp(QString("Root path %1 is not accessible").arg(root));
    
    if (!type.isEmpty())
        name += "-"+type;

    Packager packager(name, version, notes);
    if (strip)
       packager.stripFiles(rootDir.filePath());
    packager.makePackage(rootDir.filePath());

    return 0;
}

