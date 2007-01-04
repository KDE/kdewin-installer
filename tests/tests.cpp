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

bool testDownloadApacheModIndex()
{
	Downloader downloader(/*blocking=*/ true);
	PackageList packageList(&downloader);
	
	qDebug() << "trying to download kde related package list";
	downloader.start("http://software.opensuse.org/download/KDE:/KDE3/SUSE_Linux_10.1/noarch/","packages.html");
	if (!packageList.readFromHTMLFile("packages.html",PackageList::ApacheModIndex)) {
		qDebug() << "... failed ";
		return false; 
	}
	packageList.listPackages("Package List");
	return true;
}

bool testDownloadSourceForge()
{
	Downloader downloader(/*blocking=*/ true);
	PackageList packageList(&downloader);

	qDebug() << "trying to download sourceforge related package list";
	downloader.start("http://sourceforge.net/project/showfiles.php?group_id=23617","packages.html");

	if (!packageList.readFromHTMLFile("packages.html",PackageList::SourceForge)) {
		qDebug() << "... failed ";
		return false; 
	}
	packageList.listPackages("Package List");
	return true;
}

int main(int argc, char *argv[])
{
	QCoreApplication app(argc, argv);

	testDownloadApacheModIndex();
	testDownloadSourceForge();

	return 0;
	
}
