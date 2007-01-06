KDE-Installer 


The idea of the kde-installer is to have a command line and/or gui application, 
which makes it possible to download and install kde applications on windows. 

KDE on windows needs several runtime package, which are placed on different 
locations and in different formats: 

package		     site                          	   format 							notes 

dbus       http://sf.net/projects/windbus      source/svn             
           
qt4        http://ftp.iasi.roedu.net/mirrors/ftp.trolltech.com/qt/source/qt-win-opensource-4.2.1-mingw.exe
																               source/zip					needs qdbus patch 
			    																		
bzip2			 http://sf.net/projects/gnuwin32		 binary/zip					required by kdelibs
giflib							... 											 binary/zip					required by kdelibs
jpeg      					...     									 binary/zip					required by kdelibs
libagg23  					...												 binary/zip					required by kdelibs
libiconv  					...												 binary/zip         required by kdelibs
libintl   					...												 binary/zip         required by kdelibs
libpng    					...												 binary/zip         required by kdelibs
libxml2   					...												 binary/zip         required by kdelibs
libxslt   					...												 binary/zip         required by kdelibs
openssl   					...												 binary/zip         required by kdelibs
pcre      					...												 binary/zip         required by kdelibs
tiff      					...												 binary/zip         required by kdelibs
zlib      					...												 binary/zip         required by kdelibs

aspell				http://sf.net/projects/gnuwin32  binary/zip					required by kdelibs
bzip2			 		 			...												 binary/zip					required by kdelibs
expat								... 											 binary/zip					required by kdelibs
giflib							... 											 binary/zip					required by kdelibs
jasper              ... 											 binary/zip		      required by kdelibs
jpeg      					...     									 binary/zip					required by kdelibs
libagg23  					...												 binary/zip					required by kdelibs
libiconv  					...												 binary/zip         required by kdelibs
libintl   					...												 binary/zip         required by kdelibs
libpng    					...												 binary/zip         required by kdelibs
libxml2   					...												 binary/zip         required by kdelibs
libxslt   					...												 binary/zip         required by kdelibs
openssl   					...												 binary/zip         required by kdelibs
pcre      					...												 binary/zip         required by kdelibs
tiff      					...												 binary/zip         required by kdelibs
zlib      					...												 binary/zip         required by kdelibs
        					   or 
        http://www.abi-neuhaus.de/chris/win32libs/

kdewin32		anonsvn.kde.org										source/svn					required by all kde packages

perl				http://downloads.activestate.com/ActivePerl/Windows/5.8/ActivePerl-5.8.8.819-MSWin32-x86-267479.msi
																							binary/msi				  required by kdelibs

python			http://www.python.org/ftp/python/2.5/python-2.5.msi
						 																	 binary/msi					required by kdelibs
        
db					http://www.oracle.com/technology/software/products/berkeley-db/db/index.html
																							 binary/msi 				required by kdevelop

boost				http://sf.net/projects/boost 			 binary/exe         required by kdevelop

kdelibs			anonsvn.kde.org										 source/svn					
																							 source/tar.bz2

kdepimlibs	anonsvn.kde.org										 source/svn					
																							 source/tar.bz2

kdebase			anonsvn.kde.org										 source/svn					
																							 source/tar.bz2

kdepim			anonsvn.kde.org										 source/svn					
																							 source/tar.bz2

kdeedu			anonsvn.kde.org										 source/svn					
																							 source/tar.bz2

kdevelop		anonsvn.kde.org										 source/svn					
																							 source/tar.bz2

... 

For developers the following package are required too 

cmake 	http://www.cmake.org/files/v2.4/			binary/exe+zip

svn     http://subversion.tigris.org/files/documents/15/34093/svn-1.4.0-setup.exe
																							binary/exe

c++ compiler   either gcc or msvc 
																							
															
This installer will get all informations where the related packages 
are located and would be able to download packages and to 
install packages in gz, tar.bz2 or zip format. 
The zip format is included directly in the installer. 
Other formats are supported by using external unpack tools or (later directly, 
in case a good library is available).
The gnuwin32 site for example has bzip2 and tar tools available, which could be 
downloaded on demand. 

The installer checks to see if said packages are _already_ installed and 
whether they of a high enough version. This requires a kind of dependency 
tracking between several download locations. 


Technical specifications
========================

The installer is a qt4 application using shared qtcore, qtgui and qtnetwork 
libraries. It will be possible to use static versions to be independent from 
any other dll.

How the installer works: 
1. On each start it downloads a configuration file (see design.txt for format) 
   with specification about supported sites from a well known and stable server 
   (for example download.kde.org) which the installer is able to use packages. 
   (This help also to track the usage of the installer) 
   
2. The installer asks the user for his preferences, 
		- installation root [2.1]
		- mingw or msvc based installation 
	 and may be some additional options like proxy settings 
 
3. Then it downloads either
		1. a recent package list from the specified site or 
		2. a predefined package list from the server used in 1. 
	 Case 2 allows a dependency tracking between packages on the site 
	 and which kde package required which package on the site [3.1]
	 while case 1 may be more actual. 

4. The installer displays a list of available packages and it's current 
   installation state. The user can select packages, the installer 
   selects additional required packages automatically. 

5. If all packages are selected, the installer download the selected 
   packages. If there are multiple mirrors for a specific site available, 
   the installer ask the user for the mirror to use before downloading. 
   
6. After downloading then packages will be installed. If a package is 
   already installed the old version will be deleted before the new 
   one is installed. [6.1]
   
   If a package is of type zip or tar.bz2 the installer unpack it into 
   the installation root, if the package is of type .exe or .msi, it 
   starts the related installer and wait until it is finished. 
   
   While installing the installer saves the name, version and file list 
   of the installed package in a local database (probably text based)[6.2]

The design allows to use different download types (for example from a 
sourceforge project or other locations reachable by http/ftp) and/or 
different download mirrors.

[2.1] Not sure, if we can install all in one root or if we need more 
      installation roots

[3.1] This list could be generated by collecting the (Find... statement) in the 
      CMakeLists.txt files of kde sources 

[4.1] Before installing a file it must be checked if a file is in use and 
      may be rename before or other valid file-in-use handling (see cygwin installer)

[6.2] gnuwin32 packages contains a file list and version informations, which are 
      unpacked by the installer into the manifest directory of the installation root. 
      This informations are used by the installer to determine installation status 

      


Ralf

