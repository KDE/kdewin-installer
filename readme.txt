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

 
Known Bugs
==========
-  On large package install like qt doc package, the QListView Widget performs 
   huge memory realloctions, which slow done install detail display very much. 
   This seems to be a qt 4.x internal problem.
-  manifest files for source packages does not contain the full path, there must 
   be prefixed src/<package>-<version>
   
   
TODO
====

1 installer 

  1 common 
    2 complete methods for writing windows start menues entries 
    3 write environment settings scripts (KDELIBS,PATH ??), this may also affect 
      windows start menu entries writing 
    4 add mode for installing from local package directory 
      Q: how to restore the categories ? 
      Q: May be by storing packages into category dirs 
   
  2 command line installer 
      1 add long command line options like operation mode --install, --erase, --list --query, ... 
  
  3 gui 
      1 add check for partial downloads
  	4 center checkboxes in package selector page -> not possible yet, see comment in source
  	5 reduce column width of package types in package selector page
  	8 add reinstall package support 
  	  -> currently by using a remove and install by hand 
  	10 add a switch to select end user/developer operation (end user by default)
  	  -> in end user mode only the bin packages are visible 
  	12 if remote config is not available only display installed packages
  	13 add update support
  	  -> currently implemented by removing old package, installing new package by hand
  	16 add msvc/ming mode, with different installation roots and package filtering 
  	  -> different install roots are implemented by switching the install root in the settings 
  	     which reloads the config
  	17 add support for tar + gz/bzip2 package by using external win32 tools 
  

2 packager
    1 add 7zip-suppport (at first by using external 7zip tool, because there is no usable 7zip compress library)



cmake support
=============

kdewin-installer allows to add binary packaging suppport to cmake based projects. 

1. copy cmake/modules/FindKDEWIN_Installer.cmake into cmake/modules of the related proejct

2. add the following lines to the top level cmake 

set(CMAKE_MODULE_PATH "${CMAKE_SOURCE_DIR}/cmake/modules")
find_package(KDEWIN_INSTALLER)

if (KDEWIN_INSTALLER_FOUND)
    add_custom_target( kdewinpackage
        COMMAND ${KDEWIN_PACKAGER} 
            -name <packagename>
            -root ${CMAKE_INSTALL_PREFIX} 
            -srcroot ${CMAKE_SOURCE_DIR} 
            -srcexclude "<excluded dirs in source dir>"
            -version "version"
            -notes "description"
    )
endif (KDEWIN_INSTALLER_FOUND)

3. after compiling, install the package and run kdewinpackage



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
  
	cmake -G "NMake Makefiles" ..\kdewin-installer    -> Release Build 
	<make-tool> 

  or 

	cmake -G "NMake Makefiles" ..\kdewin-installer  -DCMAKE_BUILD_TYPE=MinSizeRel  -> size optimized
	<make-tool> 


Happy hacking




History
=======

see ChangeLog.txt
