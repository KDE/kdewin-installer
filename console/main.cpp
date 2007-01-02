/****************************************************************************
**
**
****************************************************************************/

#include <QCoreApplication>

#include "packagelist.h"
#include "downloader.h"
#include "installer.h"

// this functions will go into class PackageList 
QStringList filterPackageFiles(const QStringList &list,const QString &mode)
{
	QStringList result; 
  for (int j = 0; j < list.size(); ++j) {
  	QUrl url(list.at(j));
    QFileInfo fileInfo(url.path());
    QString fileName = fileInfo.fileName();

    // only download package not already downloaded and only bin and lib packages
		if (mode == "URL" && QFile::exists(fileName))
	    qDebug() << fileName << " - already downloaded";
		else if(fileName.contains("src") ) 
	    qDebug() << fileName << " - ignored";
		else {
	    if (mode == "URL")
		    qDebug() << fileName << " - downloading";
		 	else
		    qDebug() << fileName << " - installing";
	    if (mode == "URL")
	    	result << list.at(j);
	    else
	    	result << fileName;
  	}
	}
	return result;
}



int main(int argc, char *argv[])
{
	QCoreApplication app(argc, argv);

	PackageList packageList;
	Installer installer(&packageList);
	Downloader downloader(/*blocking=*/ true);

	if (!QFile::exists("packages.txt")) {
		// download package list 
		downloader.start("http://sourceforge.net/project/showfiles.php?group_id=23617","packages.html");

		// load and parse 
		if (!packageList.readFromHTMLFile("packages.html"))
			return 1; 

		// save into file
		if (!packageList.writeToFile("packages.txt"))
			return 1; 

		// print list 
		packageList.listPackages("Package List");

		// remove temporay files 
		QFile::remove("packages.html");
	}
	else {
		// read list from file 
		if (!packageList.readFromFile("packages.txt"))
			return 1; 

		if ( !QFile::exists("bin\\unzip.exe") ) {
			QStringList files = packageList.getFilesForDownload("unzip");
			files = filterPackageFiles(files,"URL");
		  for (int j = 0; j < files.size(); ++j)
				downloader.start(files.at(j));
		}
		
		// print list 
		packageList.listPackages("Package List");

		if ( !QFile::exists("bin\\unzip.exe") ) {
			qDebug() 	<< "Please unpack " 
							<< packageList.getPackage("unzip").getFileName(Package::BIN) 
							<< " into the current dir"
			<< "\n then restart installer to download and install additional packages."
			<< "\n\n" << app.arguments().at(0) << "<package-name> <package-name>";
			return 0;
		}
	}

	QStringList packages;

  for (int i = 1; i < app.arguments().size(); ++i) {
  	if (!app.arguments().at(i).startsWith("-"))
			packages << app.arguments().at(i);
	}
	
	if (packages.size() > 0) {
		qDebug() << "the following packages are considered for downloading: " << packages;

	  for (int i = 0; i < packages.size(); ++i) {
			QStringList files = packageList.getFilesForDownload(packages.at(i));
			files = filterPackageFiles(files,"URL");
		  for (int j = 0; j < files.size(); ++j)
				downloader.start(files.at(j));
		}
		// install packages 
		if ( QFile::exists("bin/unzip.exe")) {
			qDebug() << "prelimary installer found, now installing";
		  for (int i = 0; i < packages.size(); ++i) {
				QStringList files = packageList.getFilesToInstall(packages.at(i));
				files = filterPackageFiles(files,"PATH");
			  for (int j = 0; j < files.size(); ++j)
					installer.install(files.at(j),"");
			}	  	
		}

	}
	else
		qDebug() << "no packages selected ";
		

	return 0;
}
