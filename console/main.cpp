/****************************************************************************
**
** Copyright (C) 2005-2011 Ralf Habacker. All rights reserved.
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

#include "config.h"

#include <QCoreApplication>
#include <QDebug>
#include <QFile>
#include <QStringList>

#include "packagelist.h"
#include "downloader.h"
#include "installer.h"
#include "installerengineconsole.h"
#include "misc.h"
#include "typehelper.h"

#include <iostream>

using namespace std;

static struct Options
{
    bool all;
    bool download;
    bool install;
    bool remove;
    bool list;
    bool listURL;
    bool query;
    bool description;
    bool categories;
    bool requires;
    bool verbose;
    bool debug;
    QString url;
    QString rootdir;
}
options;

static void usage(const QString &message=QString())
{
    if (!message.isEmpty())
        cout << "error: " << qPrintable(message);
    cout << "... [options] <packagename> [<packagename>]"
    << "Qt: " QTVERSION "\n"
    << "kdewin-installer-console: " VERSION_PATCH "\n"
    << "\nOptions: "
    << "\n -v|--verbose                                   print detailed process informations"
    << "\n --debug                                        print debug informations"
    << "\n"
    << "\n -u <path>|--url=<path>                         use download server <url> [1]"
    << "\n -r <path>|--root=<path>                        use install <root> [1]"
    << "\n"
    << "\n -i|--install <package>                         download and install package"
    << "\n -d|--download <package>                        download package"
    << "\n -e|--erase <package>                           remove installed package"
    << "\n"
    << "\n -l|--list <listoptions> [<package>]            list available package"
    << "\n\nOptions for available packages"
    // is search instead of list a better name ? 
    << "\n -l|--list -u|--url <package>                   list package items url of <package>" 
    << "\n -l|--list -u|--url -a|-all                     list package items url of all packages" 
    << "\n -l|--list -a|--all                             list all available packages"
    << "\n -l|--list -c|--categories <package>            print categories of <package>"
    << "\n -l|--list -d|--description <package>           print description of <package>"
    << "\n -l|--list -r|--requires <package>              list required packages of <package>"
    << "\n"
    << "\n -q|--query <queryoptions> [<package>]          query installed packages"
    << "\n\nOptions for installed packages"
    << "\n -q|--query <package>                           print generic information of <package>"
    << "\n -q|--query -l <package>                        list installed package files of <package>"
    << "\n -q|--query -a|-all                             query all installed packages"
    << "\n -q|--query -r|--requires                       query package dependencies"
    << "\n"
    << "\n notes: "
    << "\n[1] (url/path is shared with the gui installer and will be stored for further runs)"
    << "\n"

    ;
}
//#define CYGWIN_INSTALLER

int main(int argc, char *argv[])
{
    QCoreApplication app(argc, argv);

    QStringList packages;

    for(int i = 1; i < app.arguments().size(); i++)
    {
        if (app.arguments().at(i).startsWith('-'))
        {
            QString option = app.arguments().at(i);
            if (option == "-h" || option == "--help")
            {
                usage();
                exit(1);
            }
            else if (option == "-a" || option == "--all")
                options.all = true;
            else if (option == "-d" || option == "--download")
                options.download = true;
            else if (option == "-e" || option == "--erase")
                options.remove = true;
            else if (option == "-i" || option == "--install")
                options.download = options.install = true;
            else if (option == "-l" || option == "--list")
            {
                options.list = true;
            }
            else if (app.arguments().at(i) == "-u" || app.arguments().at(i) == "--url")
            {
                if (options.list)
                    options.listURL = true;
                else
                {
                    usage(app.arguments().at(i) + " could not be used in this context");
                    exit(1);
                }
            }
            else if (app.arguments().at(i) == "-c" || app.arguments().at(i) == "--categories")
            {
                if (options.list)
                    options.categories = true;
                else
                {
                    usage(app.arguments().at(i) + " could not be used in this context");
                    exit(1);
                }
            }
            else if (app.arguments().at(i) == "-d" || app.arguments().at(i) == "--description")
            {
                if (options.list)
                    options.description = true;
                else
                {
                    usage(app.arguments().at(i) + " could not be used in this context");
                    exit(1);
                }
            }
            else if (options.query && (app.arguments().at(i) == "-r" || app.arguments().at(i) == "--requires"))
            {
                options.requires = true;
            }
            else if (option == "-q" || option == "--query")
                options.query = true;
            else if (option == "-m" || option == "--mmm")
            {}
            else if (option.startsWith("--url"))
                options.url = option.replace("--url=","");
            else if (option == "-u")
                options.url = app.arguments().at(++i);                
            else if (option == "-r")
                options.rootdir = app.arguments().at(++i);
            else if (option.startsWith("--root"))
                options.rootdir = option.replace("--root=","");
            else if (option == "-v" || option == "--verbose")
                options.verbose = true;
            else if (option == "--debug")
                options.debug = true;
        }
        else
            packages << app.arguments().at(i);
    }

    setMessageHandler();

    InstallerEngineConsole engine;

    // set default url 
    if (!options.url.isEmpty())
        InstallerEngine::defaultConfigURL = options.url;
    else if (!Settings::instance().downloadDir().isEmpty())
        InstallerEngine::defaultConfigURL = "file:///" + Settings::instance().downloadDir().replace("\\","/");
    else if (Settings::instance().mirrorWithReleasePath().isValid())
        InstallerEngine::defaultConfigURL = Settings::instance().mirrorWithReleasePath().toString();

    if (options.verbose)
        qOut() << "using url " << InstallerEngine::defaultConfigURL << "\n";

    if (!options.rootdir.isEmpty())
        Settings::instance().setInstallDir(options.rootdir, false);
    else
    {    
        QFileInfo f(QCoreApplication::applicationDirPath() + "/../manifest");
        if (f.exists() && f.isDir())
        {
            Settings::instance().setInstallDir(f.canonicalPath(), false);
        }    
        else 
        {
            f.setFile(QCoreApplication::applicationDirPath() + "/manifest");
            if (f.exists() && f.isDir())
                Settings::instance().setInstallDir(f.canonicalPath(), false);
        }
    }

    if (options.verbose)
        qOut() << "using root " << Settings::instance().installDir() << "\n";
        
    engine.setRoot(Settings::instance().installDir());

    // query needs setting database root, this is performed by root
    if (options.query)
    {
        if (options.all)
            engine.queryPackage();
        else if (options.list)
            engine.queryPackageListFiles(packages);
        return 0;
    }

    if (options.list)
    {
        if (options.listURL) 
        {
            if (options.all)
                engine.listPackageURLs();
            else
                engine.listPackageURLs(packages);
        }
        else if (options.requires)
            engine.queryPackageWhatRequiresAll(packages);
        else if (options.all)
            engine.listPackage();
        else if (options.categories) 
            engine.listPackageCategories(packages);
        else if (options.description) 
            engine.listPackageDescription(packages);
        else 
            engine.listPackage(packages);
        return 0;
    }

    if(options.remove && packages.size() > 0)
        engine.removePackages(packages);
    else
    {
        if((options.download || options.install) && packages.size() > 0)
            engine.downloadPackages(packages);
        if(options.install && packages.size() > 0)
            engine.installPackages(packages);
    }
    return 0;

}
