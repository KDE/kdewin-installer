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

#include <QCoreApplication>
#include <QDebug>
#include <QFile>

#include "packagelist.h"
#include "downloader.h"
#include "installer.h"

static struct Options
{
    bool verbose;
    bool query;
    bool download;
    bool install;
    bool list;
    QString rootdir;
}
options;

void usage()
{
	qDebug() << "... [options] <packagename> [<packagename>]"
	 << "\nOptions"
	 << "\n -l list packages"
	 << "\n -q <packagename> query packages"
	 << "\n -i <packagename> download and install package"
	 << "\n -d <packagename> download package"
	 ;
}
//#define CYGWIN_INSTALLER

int main(int argc, char *argv[])
{
	QCoreApplication app(argc, argv);

	QStringList packages;

	int i=1;
  while(i < app.arguments().size()) {
		if (app.arguments().at(i).startsWith("-")) {
			QString option = app.arguments().at(i);
			if (option == "-h") {
				usage();
				exit(0);
			}
			if (option == "-l") 
				options.list = true;
			else if (option == "-v") 
				options.verbose = true;
			else if (option == "-q")
				options.query = true;
			else if (option == "-d") 
				options.download = true; 
			else if (option == "-i") {
				options.download = true; 
				options.install = true; 
			}
			else if (option == "-m") {
			}
			else if (option == "-r") {
				options.rootdir = app.arguments().at(++i);
			}	
		}
		else
			packages << app.arguments().at(i);
		i++;
	}
	
#ifdef CYGWIN_INSTALLER
	Cygwin::main(app);
#else
	Package::baseURL = "http://heanet.dl.sourceforge.net/sourceforge/gnuwin32/";
	// other mirror http://www.mesh-solutions.com/sf/
	
	Downloader downloader(/*blocking=*/ true);
	PackageList packageList(&downloader);
	Installer installer(&packageList);
//	installer.setVerbose(options.verbose);
	installer.setRoot(options.rootdir.isEmpty() ? "packages" : options.rootdir);

	if ( !packageList.hasConfig() ) {
		// download package list 
		downloader.start("http://sourceforge.net/project/showfiles.php?group_id=23617","packages.html");

		// load and parse 
		if (!packageList.readHTMLFromFile("packages.html"))
			return 1; 

		// save into file
		if (!packageList.writeToFile())
			return 1; 

		// remove temporay files 
		QFile::remove("packages.html");

#ifdef USE_EXTERNAL_ZIP
		if ( !installer.isEnabled() ) {
			packageList.downloadPackage("unzip");
			qDebug() 	<< "Please unpack " 
								<< packageList.getPackage("unzip")->getFileName(Package::BIN) 
								<< " into the current dir"
								<< "\n then restart installer to download and install additional packages."
								<< "\n\n" << app.arguments().at(0)  << "[options] <package-name> <package-name>";
			return 0;
		}
#endif
	}
	else {
		// read list from file 
		if (!packageList.readFromFile())
			return 1; 
	}

	if (options.list)
		packageList.listPackages("Package List");
	
	else if(options.download && packages.size() > 0) {
		qDebug() << "the following packages are considered for downloading: " << packages;

		Package *pkg;
		for (int i = 0; i < packages.size(); ++i) {
	  		if ((pkg = packageList.getPackage(packages.at(i))))
				packageList.downloadPackage(packages.at(i));
			else
				qDebug() << "package '" << packages.at(i) << " not found: ";
				
		}
	}
	// install packages 
	if (options.install && packages.size() > 0 && installer.isEnabled()) {
		qDebug() << "prelimary installer found, now installing";
		Package *pkg;
		for (int i = 0; i < packages.size(); ++i) {
			if ((pkg = packageList.getPackage(packages.at(i))))
				packageList.installPackage(packages.at(i));
		}	  	
	}

#endif
	return 0;
}
