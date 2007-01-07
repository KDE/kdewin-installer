Initial version of a KDE installer currently used to download and install
the required libraries from the GNUWIN32 project mirrors 

State
=====

- The command line tool works currently well for gnuwin32 related packages. 

- The gui installer is in a very early state and is able to download and install 
  gnuwin32 packages too. 


NOTES
===== 
- The installer created a file packages.txt, which contains all packages 
  available on the download mirror. 
 
- see doc/readme.txt for more informations about the idea and 
  backgrounds of the installer

- sources are located on http://websvn.kde.org/trunk/kdesupport/kdewin32/installer/


TODO
====
- add long command line options like 
	- destdir/installation root 
	- operation mode --install, --erase, --list --query, ... 

- use QT_NO_DEBUG_OUTPUT instead of #ifdef DEBUG to avoid debug output 

- gui 
	- display file installing in gui, currently redraw does not work 
	- goto finish page directly when installing is ready 
	- only enable package type when a related package is available
	- center checkboxes in package selector page
	- reduce column width of package types in package selector page
	- save selections in package selector page different from initial 
	  installed packages to avoid reinstalling by default; before 
	  downloading/installing a check must be performed, which packages 
	  really have to be installed. 
	  

Happy hacking




History
=======

version 0.5.1
-------------
- added initial gui client

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
