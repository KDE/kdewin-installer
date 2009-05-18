/****************************************************************************
**
** Copyright (C) 2007  Christian Ehrlicher <ch.ehrlicher@gmx.de>.
** Copyright (C) 2009  Ralf Habacker
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

#include "packager.h"
#include "qio.h"

#include <QtDebug>
#include <QCoreApplication>
#include <QFileInfo>
#include <QUrl>
#include <QDir>


static void printHelp(const QString &addInfo)
{
    qerr << QDir::convertSeparators(QCoreApplication::applicationFilePath());
    if(!addInfo.isEmpty())
        qerr << ": " << addInfo;
    qerr << "\n";
    qerr << "Options:"
       << "\n\t\t"      << "-complete create all-in-one package with all files"
       << "\n\t\t"      << "-checksum <md5|sha1> - set checksum mode (default: md5)"
       << "\n\t\t"      << "-compression <1|2> - set compression mode to"
       << "\n\t\t\t"    << "1 - zip, default"
       << "\n\t\t\t"    << "2 - tar.bz2"
// not enabled yet
//       << "\n\t\t"      << "-debug-package create also debug package"
       << "\n\t\t"      << "-destdir directory where to store the zip files to"
       << "\n\t\t"      << "-name <packageName>"
       << "\n\t\t"      << "-notes <additional notes for manifest files>"
       << "\n\t\t"      << "-root <path to package files>"
       << "\n\t\t"      << "-srcroot <path to source files>"
       << "\n\t\t"      << "-srcexclude <patterns> path pattern to exclude from src package"
       << "\n\t\t"      << "-verbose display verbose processing informations"
       << "\n\t\t"      << "-version <package version>"
       << "\n\t\t"      << "-strip <strip debug infos> (MinGW only)"
       << "\n\t\t"      << "-special make special assumptions about package content (only for Qt)"
       << "\n\t\t"      << "-type <type of package> (mingw, msvc)"
       << "\n";

    qerr.flush();
    exit(1);
}

int main(int argc, char *argv[])
{
    QCoreApplication app(argc, argv);

    QStringList args = app.arguments();
    QString name, root, srcRoot, srcExclude, version, notes, type, destdir;
    QString checkSumMode = "md5";
    bool bComplete = false;
    QFileInfo rootDir;
    QFileInfo srcRootDir;
    bool strip = false;
    bool verbose = false;
    bool debugPackage = false;
    bool hashFirst = true;
    bool specialPackage = false;
    unsigned int compressionMode = 1; // zip

    args.removeAt(0);   // name of executable

    int idx = args.indexOf("-name");
    if(idx != -1 && idx < args.count() -1) {
        name = args[idx + 1];
        args.removeAt(idx + 1);
        args.removeAt(idx);
    }

    idx = args.indexOf("-type");
    if(idx != -1 && idx < args.count() -1) {
        type = args[idx + 1];
        args.removeAt(idx + 1);
        args.removeAt(idx);
    }

    idx = args.indexOf("-root");
    if(idx != -1 && idx < args.count() -1) {
        root = args[idx + 1];
        rootDir = QFileInfo(root);
        args.removeAt(idx + 1);
        args.removeAt(idx);
    }

    idx = args.indexOf("-srcroot");
    if(idx != -1 && idx < args.count() -1) {
        srcRoot = args[idx + 1];
        srcRootDir = QFileInfo(srcRoot);
        args.removeAt(idx + 1);
        args.removeAt(idx);
    }

    idx = args.indexOf("-srcexclude");
    if(idx != -1 && idx < args.count() -1) {
        srcExclude = args[idx + 1];
        args.removeAt(idx + 1);
        args.removeAt(idx);
    }

    idx = args.indexOf("-version");
    if(idx != -1 && idx < args.count() -1) {
        version = args[idx + 1];
        args.removeAt(idx + 1);
        args.removeAt(idx);
    }

    idx = args.indexOf("-strip");
    if(idx != -1) {
        strip = 1;
        args.removeAt(idx);
    }

    idx = args.indexOf("-hashfirst");
    if(idx != -1) {
        hashFirst = true;
        args.removeAt(idx);
        if (verbose) 
        {
            QTextStream qerr(stderr);
            qerr << "-hashfirst switch ignored, hashes are written always at first\n";
        }
    }

    idx = args.indexOf("-verbose");
    if(idx != -1) {
        verbose = 1;
        args.removeAt(idx);
    }

    idx = args.indexOf("-debug-package");
    if(idx != -1) {
        debugPackage = 1;
        args.removeAt(idx);
    }
    
    idx = args.indexOf("-special");
    if(idx != -1) {
        specialPackage = true;
        args.removeAt(idx);
    }

    idx = args.indexOf("-notes");
    if(idx != -1 && idx < args.count() -1) {
        notes = args[idx + 1];
        args.removeAt(idx + 1);
        args.removeAt(idx);
    }

    idx = args.indexOf("-destdir");
    if(idx != -1 && idx < args.count() -1) {
        destdir = args[idx + 1];
        args.removeAt(idx + 1);
        args.removeAt(idx);
    }

    idx = args.indexOf("-complete");
    if(idx != -1 && idx < args.count()) {
        bComplete = true;
        args.removeAt(idx);
    }

    idx = args.indexOf("-compression");
    if(idx != -1 && idx < args.count() -1) {
      compressionMode = args[idx + 1].toUInt();
      args.removeAt(idx + 1);
      args.removeAt(idx);
      if(compressionMode < 1 || compressionMode > 2)
        printHelp(QString("Unknown compression mode %1").arg(compressionMode));
    }
    idx = args.indexOf("-checksum");
    if(idx != -1 && idx < args.count() -1) {
      checkSumMode = args[idx + 1];
      args.removeAt(idx + 1);
      args.removeAt(idx);
      if(checkSumMode != "md5" && checkSumMode != "sha1")
        printHelp(QString("Unknown checksum mode %1").arg(checkSumMode));
    }

    // Qt-package needs '-type foo'
    if(type.isEmpty()) {
      QString lName = name.toLower();
      if (lName.startsWith("qt") || lName.startsWith("q++") || lName.startsWith("q.."))
        printHelp("Please provide \'-type' for qt package");
    }

    if(args.count() > 0)
        printHelp(QString("unknown command line parameter(s): '%1'").arg(args.join(" ")));
    if(name.isEmpty())
       printHelp("-name not specified");
    if(root.isEmpty())
       printHelp("-root not specified");
    if(version.isEmpty())
       printHelp("-version not specified");

    if(!rootDir.isDir() || !rootDir.isReadable())
       printHelp(QString("Root path %1 is not accessible").arg(root));

    if(!srcRoot.isEmpty() &&(!srcRootDir.isDir() || !srcRootDir.isReadable()))
       printHelp(QString("Source Root path %1 is not accessible").arg(srcRoot));

    if (!type.isEmpty())
        name += '-' + type;

    Packager packager(name, version, notes);
    if (!srcRoot.isEmpty())
        packager.setSourceRoot(srcRootDir.filePath());

    if (!srcExclude.isEmpty())
        packager.setSourceExcludes(srcExclude);

    packager.setVerbose(verbose);
    packager.setWithDebugPackage(debugPackage);
    packager.setCompressionMode(compressionMode);
    packager.setCheckSumMode(checkSumMode);

    if (specialPackage)
        packager.setSpecialPackageMode( true );
    if (strip)
       packager.stripFiles(rootDir.filePath());

    packager.makePackage(rootDir.filePath(), destdir, bComplete);

    return 0;
}
