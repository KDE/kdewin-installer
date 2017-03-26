About
=====

The KDEWIN Installer is aimed to be a installer tool to ease kde installation on windows. 
See http://commit-digest.org/issues/2007-01-14/ for an article about the background of this installer.

State
=====
The gui installer is able to download, install and update packages from different internet ftp
or http or https location with or without using a proxy. The command line installer works as expected.

NOTES
===== 
- see doc/readme.txt for more informations about the idea and backgrounds of the installer

- sources are located on https://cgit.kde.org/kdewin-installer.git/

- the released version is build using a static qt release with specific configure 
  parameters, see section "static compile hints" for more informations
   
Known Bugs
==========
-  manifest files for gnuwin32 source packages does not contain the full path, there must 
   be prefixed src/<package>-<version> - repackaging of the related package will help


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

- msvc: To have a size optimized static msvc version replace all occurrence2
  of /MD with /MT in mkspecs/win32-msvcXXXX/qmake.conf where XXXX is the year of the related msvc release.

  Then you should configure qt using the command line options

    qconfigure msvcXXXX -static -debug-and-release -no-stl -no-exceptions -no-qt3support -no-rtti

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
