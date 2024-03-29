/****************************************************************************
**
** Copyright (C) 2007  Christian Ehrlicher <ch.ehrlicher@gmx.de>.
** Copyright (C) 2009  Ralf Habacker
** All rights reserved.
**
** This file is part of the KDE installer for windows
**
** SPDX-License-Identifier: GPL-2.0-only
****************************************************************************/

#include "config.h"
#include "packager.h"
#include "qio.h"
#include "xmltemplatepackager.h"

#include <QtDebug>
#include <QCoreApplication>
#include <QFileInfo>
#include <QUrl>
#include <QDir>

QString name, root, srcRoot, symRoot, srcExclude, version, notes, type, destdir;
QString checkSumMode = "md5";
bool bComplete = false;
QFileInfo rootDir;
QFileInfo srcRootDir;
QFileInfo symRootDir;
bool strip = false;
bool verbose = false;
bool debugPackage = false;
bool hashFirst = true;
bool specialPackage = false;
unsigned int compressionMode = 1; // zip

QMap<QString, QString> internalTemplateList;

void initTemplateList()
{
    internalTemplateList["kde"] = ":/template-kde.xml";
    internalTemplateList["qt4"] = ":/template-qt4.xml";
    internalTemplateList["qt5"] = ":/template-qt5.xml";
    internalTemplateList["example"] = ":/template-example.xml";
}


void printBuildInTemplate()
{
    QMap<QString, QString>::iterator i;
    for (i = internalTemplateList.begin(); i != internalTemplateList.end(); ++i)
    {
        qout << "------- kdewin-packager module template: " << i.key() << "-------\n";
        QFile file(i.value());
        if (!file.open(QIODevice::ReadOnly | QIODevice::Text))
            continue;
        
         while (!file.atEnd())
             qout << file.readLine();
	}
}

static void printHelp(const QString &addInfo)
{
#if QT_VERSION >= 0x050000
    qerr << QDir::toNativeSeparators(QCoreApplication::applicationFilePath());
#else
    qerr << QDir::convertSeparators(QCoreApplication::applicationFilePath());
#endif
    if(!addInfo.isEmpty())
        qerr << ": " << addInfo;
    qerr << "\n";
    qerr << "Qt: " QT_VERSION_STR "\n";
    qerr << "kdewin-packager: " VERSION_PATCH "\n";
    qerr << "Options:"
       << "\n\t\t"      << "-complete              create all-in-one package with all files"
       << "\n\t\t"      << "-checksum <md5|sha1>   set checksum mode (default: md5)"
       << "\n\t\t"      << "-compression <1|2>     set compression mode to"
       << "\n\t\t"      << "                            1 - zip, default"
       << "\n\t\t"      << "                            2 - tar.bz2"
       << "\n\t\t"      << "-debug-package         create also debug package"
       << "\n\t\t"      << "-destdir <path>        directory where to store the zip files to"
       << "\n\t\t"      << "-name <packageName>    specify package name"
       << "\n\t\t"      << "-notes <text>          specify additional notes for manifest files"
       << "\n\t\t"      << "-root <path>           specify path to root directory of installed files"
       << "\n\t\t"      << "-special               use special packages for Qt packages (deprecated, use -template)"
       << "\n\t\t"      << "-srcroot <path>        specify path to source root"
       << "\n\t\t"      << "-srcexclude <pattern>  path pattern to exclude from src package (deprecated, use template)"
       << "\n\t\t"      << "-symroot <path>        the root directory where you symbols can be found"
       << "\n\t\t"      << "-verbose               display verbose processing informations"
       << "\n\t\t"      << "-version <version>     specify package version (e.g. 1.2.3 or 1.3.4-5)"
       << "\n\t\t"      << "-strip                 strip debug infos (MinGW only)"
       << "\n\t\t"      << "-type <type>           specify type of package (mingw, mingw4, x86-mingw4, x64-mingw4,"
       << "\n\t\t"      << "                           msvc{=vc80}, vc90, vc100, vc110, vc120, vc140, x64-vc100)"
       << "\n\t\t"      << "-print-templates       print internal xml templates (kde, qt4, qt5)"
       << "\n\t\t"      << "-template <filepath>   use xml template <filepath> for generating modules"
       << "\n\t\t"      << "-template kde|qt4|qt5  choose internal template (default is to use the old implementation)"
       << "\n";

    qerr.flush();
    exit(1);
}

int main(int argc, char *argv[])
{
    QCoreApplication app(argc, argv);
    initTemplateList();
    

    QStringList args = app.arguments();
    QString templateFile;
    bool useTemplate = false;
    bool internalTemplate = false;

    args.removeAt(0);   // name of executable

    int idx = args.indexOf("-print-templates");
    if(idx != -1 ) {
        printBuildInTemplate();
		return 0;
    }

    idx = args.indexOf("-template");
    if(idx != -1 && idx < args.count() -1) {
        QString templateName = args[idx + 1];
        useTemplate = true;
        args.removeAt(idx + 1);
        args.removeAt(idx);
        useTemplate = true;
        if (internalTemplateList.contains(templateName)) {
            internalTemplate = true;
            templateFile = internalTemplateList[templateName];
        }
        else
            templateFile = templateName;
    }

    idx = args.indexOf("-name");
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
        if (verbose) {
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

    idx = args.indexOf("-symroot");
    if(idx != -1 && idx < args.count() -1) {
        symRoot = args[idx + 1];
        symRootDir = QFileInfo(symRoot);
        args.removeAt(idx + 1);
        args.removeAt(idx);
    }

    if (!useTemplate) {
        idx = args.indexOf("-special");
        if(idx != -1) {
            specialPackage = true;
            args.removeAt(idx);
        }
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
		
    if(root.isEmpty())
       printHelp("-root not specified");

    if((!useTemplate || internalTemplate) && name.isEmpty())
       printHelp("-name not specified");

    if(version.isEmpty())
       printHelp("-version not specified");

    if(!rootDir.isDir() || !rootDir.isReadable())
       printHelp(QString("Root path %1 is not accessible").arg(root));

    if(!srcRoot.isEmpty() &&(!srcRootDir.isDir() || !srcRootDir.isReadable()))
       printHelp(QString("Source Root path %1 is not accessible").arg(srcRoot));
    
    if (debugPackage)
    {
        if (symRoot.isEmpty())
            printHelp(QString("Specify the root to the symbol files if you also provide -debug-package"));
        else
        if (!symRootDir.isDir() || !symRootDir.isReadable())
        {
            printHelp(QString("Symbol root path %1 is not accessible").arg(symRoot));
        }
    }

    Packager *packager = useTemplate ?  new XmlTemplatePackager(name, version, notes) : new Packager(name, version, notes);
    if (!type.isEmpty())
		packager->setType(type);

    if (!srcRoot.isEmpty())
        packager->setSourceRoot(srcRootDir.filePath());

    if (!srcExclude.isEmpty())
        packager->setSourceExcludes(srcExclude);

    if (!symRoot.isEmpty())
        packager->setSymbolsRoot(symRootDir.filePath());

    packager->setVerbose(verbose);
    packager->setWithDebugPackage(debugPackage);
    packager->setCompressionMode(compressionMode);
    packager->setCheckSumMode(checkSumMode);

	if (!useTemplate && specialPackage)
       packager->setSpecialPackageMode( true );
    if (strip)
       packager->stripFiles(rootDir.filePath());

    if (useTemplate)
        packager->parseConfig(templateFile);

    packager->makePackage(rootDir.filePath(), destdir, bComplete);

    return 0;
}
