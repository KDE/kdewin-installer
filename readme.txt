Initial version of a KDE installer currently used to download and install
the required libraries from the GNUWIN32 project mirrors 

State
=====
- The gui installer is able to download and install/remove packages. 
- The command line tool is broken and need to be fixed

NOTES
===== 
- see doc/readme.txt for more informations about the idea and 
  backgrounds of the installer

- sources are located on http://websvn.kde.org/trunk/kdesupport/kdewin32/installer/

- the released version is build using a static qt release with specific configure 
  parameters to reduce the application size: 

  1. replace all occurrence of /MD with /MT in mkspecs/win32-msvc2005/qmake.conf 
  2. qconfigure msvc2005 -static -debug-and-release -no-stl -no-exceptions -no-qt3support -no-rtti
  3. nmake sub-src

  4. check out kdewin-installer source 

=======

Known Bugs
==========
-  On large package install like qt doc package, the QListView Widget performs 
   huge memory realloctions, which slow done install detail display very much. 
   This seems to be a qt 4.x internal problem. 
-  manifest files for source packages does not contain the full path, there must 
   be prefixed src/<package>-<version>
   
   
TODO
====
1 main 
  1 use QT_NO_DEBUG_OUTPUT instead of #ifdef DEBUG to avoid debug output 
  2 complete methods for writing windows start menues entries 
  3 write environment settings scripts (KDELIBS,PATH ??), this may also affect 
    windows start menu entries writing 
  4 add mode for installing from local package directory
  5 test, test, test
  
2 command line installer 
    1 add long command line options like operation mode --install, --erase, --list --query, ... 

3 gui 
    1 add check for partial downloads
	4 center checkboxes in package selector page -> not possible yet, see comment in source
	5 reduce column width of package types in package selector page
	8 add reinstall package support 
	10 add a switch to select end user/developer operation (end user by default)
	   in end user mode only the bin packages are visible 
	12 if remote config is not available only display installed packages
	13 add update support
	16 add msvc/ming mode, with different installation roots and package filtering 


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

- msvc: To have a size optimised static msvc version all occurrence 
  of /MD should be replaced by /MT in the related qt's qmake.conf. 

  Then you should configure qt using the command line options 
	
	-platform ... -release -static -no-qt3support -no-rtti -no-exceptions -no-stl 

  and compile qt. 

  After qt is compiled, then you should configure and compile kdewin-installer 
  using default settings
  
	cmake -G "NMake Makefiles" ..\kdewin-installer    -> Release Build 
	<make-tool> 

  or 

	cmake -G "NMake Makefiles" ..\kdewin-installer  -DCMAKE_BUILD_TYPE=MinSizeRel  -> size optimized
	<make-tool> 





Happy hacking



History
=======

see ChangeLog.txt
