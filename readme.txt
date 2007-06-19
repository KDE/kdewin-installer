About
=====

The KDEWIN Installer is aimed to be a installer tool to ease kde installation on windows. 
See http://commit-digest.org/issues/2007-01-14/ for an article about the background of this installer.

State
=====
- The gui installer allows to download, install and update packages from different web location 

NOTES
===== 
- see doc/readme.txt for more informations about the idea and backgrounds of the installer

- sources are located on http://websvn.kde.org/trunk/kdesupport/kdewin32/installer/

- the released version is build using a static qt release with specific configure 
  parameters, see section "static compile hints" for more informations

- to be able to use the update-mime-database package with kde please add the installation path to the 
  enviromment variable KDEDIRS


 
Known Bugs
==========
-  On large package install like qt doc package, the QListView Widget performs 
   huge memory realloctions, which slow done install detail display very much. 
   This seems to be a qt 4.x internal problem.
-  manifest files for source packages does not contain the full path, there must 
   be prefixed src/<package>-<version>
-  packager: stripping of dll's which are in use results in deleting the related dll. 
   Please make sure no one uses a dll when using the -strip option
   
   
TODO
====

1 installer 

  1 common 
    2 complete methods for writing windows start menues entries -> ready, but not tested 
    3 write environment settings scripts (KDELIBS,PATH ??), this may also affect 
      windows start menu entries writing 
    4 add mode for installing from local package directory 
      Q: how to restore the categories ? 
      Q: May be by storing packages into category dirs 
    5 add support for relocatinng files in the root zip path 
       example: the depends package installs all files in the install root
                but all files should be located in bin dir
    6 remove empty directories when removing packages
    
   
  2 command line installer 
      1 add long command line options like operation mode --install, --erase, --list --query, ... 
      2 test
  
  3 gui 
      1 add check for partial downloads
  	4 center checkboxes in package selector page -> not possible yet, see comment in source
  	8 add reinstall package support 
  	  -> currently by using a remove and install by hand 
  	10 add a switch to select end user/developer operation (end user by default)
  	  -> in end user mode only the bin packages are visible 
  	12 if remote config is not available only display installed packages
  	13 add update support
  	  -> currently implemented by removing old package, installing new package by hand
  	17 add support for tar + gz/bzip2 package by using external win32 tools 
  	18 only display firefox proxy option is firefox is installed 
  

2 packager
    1 add 7zip-suppport (at first by using external 7zip tool, because there is no usable 7zip compress library)
    2 exlude *d.exe in strip 
    3 exclude .#* *.rej *.orig files in src package
    4 add *d.exe files to lib package 
    5 don't exclude exe's ending with d from bin package when a debug version using dd is present, only the debug version should go into lib 



cmake support
=============

kdewin-installer allows to add binary packaging suppport to cmake based projects by providing a cmake module named KDEWIN_Packager. 

1. copy share/apps/cmake/modules/FindKDEWIN_Packager.cmake into cmake/modules of your proejct

2. add the following lines to the top level cmake 

set(CMAKE_MODULE_PATH "${CMAKE_SOURCE_DIR}/cmake/modules")
find_package(KDEWIN_Packager)

if (KDEWIN_PACKAGER_FOUND)
KDEWIN_PACKAGER(
   "projectname"
   "version"
   "description" 
   "additional options"
)
endif (KDEWIN_PACKAGER_FOUND)

3. after compiling, run <make-tool> kdewin_package. For more information see FindKDEWIN_Packager.cmake. 


static compile hints
====================

- msvc: To have a size optimised static msvc version replace all occurrence 
  of /MD with /MT in mkspecs/win32-msvc2005/qmake.conf 

  Then you should configure qt using the command line options 
	
    qconfigure msvc2005 -static -debug-and-release -no-stl -no-exceptions -no-qt3support -no-rtti

  and compile qt with 
  
    nmake sub-src

  After qt is compiled, then you should checkout kdewin-installer source, the configure and compile 
  kdewin-installer using default settings
  
	cmake -G "NMake Makefiles" ..\kdewin-installer   -DSTATIC_BUILD=1 -> Release Build 
	<make-tool> 

  or 

	cmake -G "NMake Makefiles" ..\kdewin-installer  -DSTATIC_BUILD=1 -DCMAKE_BUILD_TYPE=MinSizeRel  -> size optimized
	<make-tool> 


Happy hacking




History
=======

see ChangeLog.txt
