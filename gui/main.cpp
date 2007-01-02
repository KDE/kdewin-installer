/****************************************************************************
**
**
****************************************************************************/

#include <QApplication>

#include "downloader.h"
#include "packagelist.h"

int main(int argc, char *argv[])
{
	QApplication app(argc, argv);

// 1. test 
//		Downloader downloader(/*wait=*/ true);
//		downloader.start("http://sourceforge.net/project/showfiles.php?group_id=23617","packages.html");

// the following does not work for unknown reasons 
//	  app.connect(&downloader, SIGNAL(done),&app, SLOT(quit));


// 2. scenario download package files 

	Downloader downloader(/*wait=*/ true);
	downloader.start("http://sourceforge.net/project/showfiles.php?group_id=23617","packages.html");

	PackageList packageList;
	if (!packageList.readFromHTMLFile("packages.html"))
		return 1; 
	packageList.listPackages("Package List");
	if (!packageList.writeToFile("packages.txt"))
		return 1; 
	QFile::remove("packages.html");

	QStringList	list = packageList.getPackageFiles("unzip");
  for (int i = 0; i < list.size(); ++i) {
  	qDebug() << list.at(i);
    downloader.start(list.at(i));
	}

//    return app.exec();
}
