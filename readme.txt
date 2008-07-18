About
=====

The KDEWIN Installer is aimed to be a installer tool to ease kde installation on windows. 
See http://commit-digest.org/issues/2007-01-14/ for an article about the background of this installer.

State
=====
The gui installer is able to download, install and update packages from different internet ftp 
or http location with or without using a proxy. The command line installer works basically.

NOTES
===== 
- see doc/readme.txt for more informations about the idea and backgrounds of the installer

- sources are located on http://websvn.kde.org/trunk/kdesupport/kdewin-installer/

- the released version is build using a static qt release with specific configure 
  parameters, see section "static compile hints" for more informations

- runtime dependencies should be separated from the build requirement. perl for example is a build requirement of kdebase. 
- some msi files are started without displaying a gui like perl. This should be fixed
   
Known Bugs
==========
-  manifest files for gnuwin32 source packages does not contain the full path, there must 
   be prefixed src/<package>-<version> - repackaging of the related package will help
   
-  packager: stripping of dll's which are in use results in deleting the related dll. 
   Please make sure no one uses a dll when using the -strip option

 
TODO
====

1 installer 

  1 common 
    2 add source documentation (when api is mostly stable)
    2 complete package descriptor format (separate build/runtime requirements) 
    3 add mode for installing from local package directory
      Q: how to restore the categories ? 
      A: May be by storing packages into category dirs 
    4 remove empty directories when removing packages 
    5 test creating start menu entries support
    6 add support to create/remove start menu entries for a specific package, which is required for the installer 
    7 how to handle packages which requires administrative access like vcredist and others (which package exactly ?)

  2 command line installer 
    3 test
  
  3 gui 
    1 add reinstall package support -> currently by using a remove and install by hand 
    2 add return to the package selector package after installing package 
    3 if remote config is not available only display installed packages
    4 only display firefox proxy option is firefox is installed 
    5 center checkboxes in package selector page -> not possible yet, see comment in source
    6 implement end user package selector page 
    7 create a msi package which installs the kde installer and adds an entry in the software control panel 

2 packager
    1 exlude *d.exe in strip 
    2 exclude .#* *.rej *.orig files in src package
    3 add *d.exe files to lib package 
    4 don't exclude exe's ending with d from bin package when a debug version using dd is present, only the debug version should go into lib 

3 configer 
    1 add support to config.txt for using flat directory structures (= only one directory), which is required for sourceforge mirrors
      this requires also extending config.txt parser 
   

cmake support
=============

kdewin-installer allows to add binary packaging suppport to cmake based projects by providing a cmake module named KDEWIN_Packager. 

1. copy share/apps/cmake/modules/FindKDEWIN_Packager.cmake into cmake/modules of your project

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
