Initial version of a KDE installer currently used to download and install
the required libraries from the GNUWIN32 project mirrors 

syntax: 

bin\kde-installer-console.exe <package-name>

NOTES
===== 
The installer created a file packages.txt, which contains all packages 
available on the download mirror. 

TODO
====
- add command line options like 
	- destdir/installation root 
	- operation mode --install, --erase, --list --query, ... 

- create gui version in wizard form (qt's simplewizard example could be taken as base) 



History
=======

version 0.5
-----------
- added support for downloading packages
- added blocking mode to downloader 
- added prelimary installation support using external unzip tool


version  0.4
------------
- separated code into shared, console and gui
- created Downloader class

version 0.3
-----------
- added support for downloading package list

version 0.2
-----------
- separated code into classes Package, PackageList 

version 0.1
-----------
- initial created from qt http example 
