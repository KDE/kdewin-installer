;kdewin installer configuration file 
@format 1.3
; this format is *prelimary* and may be changed without prior notice
@minversion 0.8.6
@timestamp 200802190830

; category notes 
@categorynotes qt Qt related packages 
@categorynotes devel-tools tools required or usefull for development
@categorynotes debug-tools tools required or usefull for debugging
@categorynotes KDE KDE packages

; to which category packages belongs to
@categorypackages win32libs aspell expat giflib iconv jasper jpeg lcms libbzip2 libidn libintl libintl libpng libxml2 libxslt openslp openssl tiff zlib shared-mime-info 
@categorypackages win32libs pcre-msvc  dbus-msvc 
@categorypackages win32libs pcre-mingw dbus-mingw
@categorypackages tools findutils grep mingw-utils
@categorypackages kdesupport kdewin32-msvc  kdewin-installer-msvc  vcredist      qimageblitz-msvc  qca-msvc  strigi-msvc  soprano-msvc  taglib-msvc  qt-msvc  phonon-msvc  fontconfig freetype
@categorypackages kdesupport kdewin32-mingw kdewin-installer-mingw runtime-mingw qimageblitz-mingw qca-mingw strigi-mingw soprano-mingw taglib-mingw qt-mingw phonon-mingw fontconfig freetype
@categorypackages KDE amarok-msvc  kdelibs-msvc  kdepimlibs-msvc  kdebase-apps-msvc  kdebase-runtime-msvc  kdeedu-msvc  kdegames-msvc  kdegraphics-msvc  kdenetwork-msvc  kdesdk-msvc  kdewebdev-msvc
@categorypackages KDE amarok-mingw kdelibs-mingw kdepimlibs-mingw kdebase-apps-mingw kdebase-runtime-mingw kdeedu-mingw kdegames-mingw kdegraphics-mingw kdenetwork-mingw kdesdk-mingw kdewebdev-mingw 
@categorypackages qt qt-msvc qt-mingw qt-static-msvc

;------------------------------------------------------
; all packages on the mirror
;------------------------------------------------------
@site all
@type apachemodindex
@SITE_URL@
@SITE_URL_LIST@
@exclude config.txt
; compiler independent deps 
@deps findutils libintl
@deps grep pcre-mingw
@deps update-mime-database libintl libxml2 iconv
@deps strigi-msvc libbzip2 openssl zlib iconv libxml2
; mingw dependencies 
@deps dbus-mingw expat 
@deps amarok-mingw kdebase-runtime-mingw phonon-mingw
@deps kdelibs-mingw pcre-mingw dbus-mingw kdewin32-mingw qimageblitz-mingw qca-mingw strigi-mingw soprano-mingw taglib-mingw aspell libbzip2 expat giflib iconv jasper jpeg libintl libpng libxml2 libxslt openssl  tiff zlib shared-mime-info
@deps kdepimlibs-mingw kdelibs-mingw
@deps kdebase-runtime-mingw kdepimlibs-mingw kdelibs-mingw
@deps kdebase-apps-mingw kdebase-runtime-mingw
@deps kdeedu-mingw kdebase-runtime-mingw kdelibs-mingw
@deps kdegames-mingw kdebase-runtime-mingw kdelibs-mingw phonon-mingw
@deps kdegraphics-mingw kdebase-runtime-mingw kdelibs-mingw fontconfig freetype
@deps kdenetwork-mingw kdebase-runtime-mingw kdelibs-mingw
@deps kdesdk-mingw kdebase-runtime-mingw kdelibs-mingw kdepimlibs-mingw
@deps kdetoys-mingw kdebase-runtime-mingw kdelibs-mingw
@deps kdewebdev-mingw kdebase-runtime-mingw kdelibs-mingw
@deps kdewin-installer-mingw qt-mingw
@deps kdewin32-mingw qt-mingw
@deps qt-mingw dbus-mingw runtime-mingw
@deps strigi-mingw libbzip2 openssl zlib iconv libxml2
; msvc dependencies 
@deps amarok-msvc kdebase-runtime-msvc phonon-msvc
; dbus may be compiled with libxml2 or expat, so use both dependencies
@deps dbus-msvc libxml2 iconv expat 
@deps kdebase-runtime-msvc kdepimlibs-msvc kdelibs-msvc
@deps kdebase-apps-msvc kdebase-runtime-msvc
@deps kdeedu-msvc kdebase-runtime-msvc kdelibs-msvc
@deps kdegames-msvc kdebase-runtime-msvc kdelibs-msvc phonon-msvc
@deps kdegraphics-msvc kdebase-runtime-msvc kdelibs-msvc fontconfig freetype
@deps kdelibs-msvc pcre-msvc vcredist qt-msvc dbus-msvc kdewin32-msvc qimageblitz-msvc qca-msvc strigi-msvc soprano-msvc taglib-msvc aspell libbzip2 expat giflib iconv jasper jpeg libintl libpng libxml2 libxslt openssl tiff zlib shared-mime-info
@deps kdenetwork-msvc kdebase-runtime-msvc kdelibs-msvc
@deps kdepimlibs-msvc kdelibs-msvc
@deps kdesdk-msvc kdebase-runtime-msvc kdelibs-msvc
@deps kdewebdev-msvc kdebase-runtime-msvc kdelibs-msvc
@deps kdewin-installer-msvc qt-msvc
@deps kdewin32-msvc qt-msvc
@deps kdetoys-msvc kdebase-runtime-msvc kdelibs-msvc
@deps qt-msvc dbus-msvc vcredist
@deps strigi-msvc libbzip2 openssl zlib iconv libxml2
; compiler independent
@pkgnotes clucene high-performance, full-featured text search engine (required for compiling strigi) 
; mingw
@pkgnotes amarok-mingw media player 
@pkgnotes kdelibs-mingw KDE libraries (support applications)
@pkgnotes kdebase-runtime-mingw KDE runtime package 
@pkgnotes kdebase-apps-mingw KDE base applicaton package 
@pkgnotes kdesdk-mingw software development package (umbrello, kate)
@pkgnotes kdegames-mingw KDE games
@pkgnotes kdeedu-mingw KDE education apps
@pkgnotes kdepimlibs-mingw KDE PIM library
@pkgnotes kdenetwork-mingw KDE Networking applications
@pkgnotes kdetoys-mingw collection of several programs in KDE
@pkgnotes kdewebdev-mingw KDE Web Development applications
@pkgnotes kdewin32-mingw kde supplementary package for win32
@pkgnotes qca-mingw Qt Cryptographic Architecture (QCA)
@pkgnotes qimageblitz-mingw KDE image effect library
@pkgnotes qt-mingw C++ GUI application framework
@pkgnotes soprano-mingw RDF storage solutions library
@pkgnotes strigi-mingw desktop search engine
@pkgnotes taglib-mingw audio meda-data library
; msvc 
@pkgnotes amarok-msvc media player 
@pkgnotes kdelibs-msvc KDE libraries (support applications)
@pkgnotes kdebase-runtime-msvc KDE runtime package
@pkgnotes kdebase-apps-msvc KDE base applicaton package 
@pkgnotes kdesdk-msvc software development package (umbrello, kate)
@pkgnotes kdegames-msvc KDE games
@pkgnotes kdeedu-msvc KDE education apps
@pkgnotes kdepimlibs-msvc KDE PIM library
@pkgnotes kdenetwork-msvc KDE Networking applications
@pkgnotes kdetoys-msvc collection of several programs in KDE
@pkgnotes kdewebdev-msvc KDE Web Development applications
@pkgnotes kdewin32-msvc kde supplementary package for win32
@pkgnotes qca-msvc Qt Cryptographic Architecture (QCA)
@pkgnotes qimageblitz-msvc KDE image effect library
@pkgnotes qt-msvc C++ GUI application framework
@pkgnotes qt-static-msvc static release of qt package, which is used for build kdewin installer
@pkgnotes soprano-msvc RDF storage solutions library
@pkgnotes strigi-msvc desktop search engine
@pkgnotes taglib-msvc audio meda-data library
;other
@pkgnotes aspell A powerful spell checker, designed to replace ispell
@pkgnotes boost-headers portable C++ libraries
@pkgnotes cfitsio library for the FITS (Flexible Image Transport System) file format
@pkgnotes dbus-mingw Freedesktop message bus system (daemon and clients)
@pkgnotes dbus-msvc Freedesktop message bus system (daemon and clients)
@pkgnotes expat XML parser library written in C (development/documentation package)
@pkgnotes fontconfig library for font customization and configuration
@pkgnotes freetype A Free, High-Quality, and Portable Font Engine
@pkgnotes giflib GIF file manipulation library (utilities and docs)
@pkgnotes gpgme GnuPG cryptography support library (runtime)
@pkgnotes iconv GNU character set conversion library and utilities
@pkgnotes jasper JPEG-2000 Library
@pkgnotes jpeg A library for manipulating JPEG image format files
@pkgnotes lcms A small-footprint, speed optimized color management engine
@pkgnotes libbzip2 shared libraries for bzip2 (runtime)
@pkgnotes libidn internationalized domain names library
@pkgnotes libintl native language support library
@pkgnotes libpng PNG library (runtime)
@pkgnotes libxml2 XML C parser and toolkit (runtime and applications)
@pkgnotes libxslt The GNOME XSLT C library and tools
@pkgnotes openslp openslp client and libraries
@pkgnotes openssl The OpenSSL runtime environment
@pkgnotes pcre-mingw Perl-Compatible Regular Expressions (utilities)
@pkgnotes pcre-msvc Perl-Compatible Regular Expressions (utilities)
@pkgnotes redland Resource Description Framework (RDF)
@pkgnotes shared-mime-info common mimetype library
@pkgnotes tiff TIFF image library (utilities)
@pkgnotes zlib The zlib compression and decompression library

;------------------------------------------------------
; debugging tools
;------------------------------------------------------
@package dbg_x86
@category debug-tools
@version 6.6.07.5
@notes msvc Debugging Tools
@url-bin http://msdl.microsoft.com/download/symbols/debuggers/dbg_x86_6.6.07.5.exe

@package regmon
@category debug-tools
@version 7.04
@notes registry monitor
@url-bin http://download.sysinternals.com/Files/Regmon.zip
@relocate ^ bin/

@package filemon
@category debug-tools
@version 7.04
@notes filesystem monitor
@url-bin http://download.sysinternals.com/Files/Filemon.zip
@relocate ^ bin/

@package procmon
@category debug-tools
@version 7.04
@notes process monitor
@url-bin http://download.sysinternals.com/Files/ProcessMonitor.zip
@relocate ^ bin/

@package gdb
@category debug-tools
@version 6.3-2
@notes mingw debugger 
@url-bin http://heanet.dl.sourceforge.net/sourceforge/mingw/gdb-6.3-2.exe

@package debugview
@category debug-tools
@version 4.64
@notes debug message monitor
@url-bin http://download.sysinternals.com/Files/DebugView.zip
@relocate ^ bin/

@package Process Explorer
@category debug-tools
@version 11.04
@notes Process Explorer
@url-bin http://download.sysinternals.com/Files/ProcessExplorer.zip
@relocate ^ bin/

@package perl 
@notes Perl Installer
@category tools
@version 5.8.8
@url-bin http://downloads.activestate.com/ActivePerl/Windows/5.8/ActivePerl-5.8.8.819-MSWin32-x86-267479.msi 
;@packageoptions

@package zip
@category tools 
@version 2.31
@notes zip tool 
@url-bin http://switch.dl.sourceforge.net/sourceforge/gnuwin32/zip-2.31-bin.zip
@url-lib http://switch.dl.sourceforge.net/sourceforge/gnuwin32/zip-2.31-lib.zip
@url-doc http://switch.dl.sourceforge.net/sourceforge/gnuwin32/zip-2.31-doc.zip
@url-src http://switch.dl.sourceforge.net/sourceforge/gnuwin32/zip-2.31-src.zip

;------------------------------------------------------
; devel-tools
;------------------------------------------------------
@package cmake
@category devel-tools 
@notes platform independent build tool 
@version 2.4.7
@url-bin http://www.cmake.org/files/v2.4/cmake-2.4.7-win32-x86.zip cmake-2.4.7-bin.zip
@url-src http://www.cmake.org/files/v2.4/cmake-2.4.7.zip cmake-2.4.7-src.zip
@relocate cmake-2.4.7-win32-x86 .
;@options

@package astyle
@category devel-tools 
@notes source code formatter
@version 1.20.2
@url-bin http://mesh.dl.sourceforge.net/sourceforge/astyle/astyle_1.20.2_windows.zip astyle-1.20.2-bin.zip 
@relocate astyle/ ./

@package depends
@notes Dependency Walker
@category debug-tools
@version 2.2
@url-bin http://www.dependencywalker.com/depends22_x86.zip depends-2.2-bin.zip
@relocate ^ bin/

@package doxygen
@notes Source code documentation generator tool
@category devel-tools
@version 1.5.4
@url-bin http://ftp.stack.nl/pub/users/dimitri/doxygen-1.5.4.windows.bin.zip doxygen-1.5.4-bin.zip
@relocate ^ bin/

@package psdk-msvc
@category devel-tools
@notes MS Platform SDK 2003
@version r2
@url-bin http://download.microsoft.com/download/f/a/d/fad9efde-8627-4e7a-8812-c351ba099151/PSDK-x86.exe

@package gcc-mingw
@version 5.1.3
@category devel-tools
@notes gcc for windows 
@url-bin http://heanet.dl.sourceforge.net/sourceforge/mingw/MinGW-5.1.3.exe

@package subversion
@version 1.4.5
@category devel-tools
@notes software revision system 
@url-bin http://subversion.tigris.org/files/documents/15/39559/svn-1.4.5-setup.exe

@package tortoisecvs
@category devel-tools 
@version 1.8.31
@notes cvs explorer integration 
@url-bin http://prdownloads.sourceforge.net/tortoisecvs/TortoiseCVS-1.8.31.exe

@package tortoisesvn
@category devel-tools 
@version 1.4.3
@notes svn explorer integration 
@url-bin http://heanet.dl.sourceforge.net/sourceforge/tortoisesvn/TortoiseSVN-1.4.3.8645-win32-svn-1.4.3.msi

@package upx
@notes Ultimate Packer for eXecutables with lzma support
@category devel-tools
@version 3.0.0
@url-bin http://upx.sourceforge.net/download/upx300w.zip upx-3.0.0-bin.zip
; relocate directories in zip files 
@relocate upx300w bin

@package vcexpress-en-msvc
@category devel-tools
@version 2005
@notes Visual Studio Express 2005 (english version)
@url-bin http://download.microsoft.com/download/8/3/a/83aad8f9-38ba-4503-b3cd-ba28c360c27b/ENU/vcsetup.exe

@package vcexpress-de-msvc
@category devel-tools
@version 2005
@notes Visual Studio Express 2005 (german version)
@url-bin http://download.microsoft.com/download/8/3/a/83aad8f9-38ba-4503-b3cd-ba28c360c27b/DEU/vcsetup.exe

@package vcredist
@category devel-tools
@version 8.0.50727.762
@notes Microsoft Visual C++ 2005 SP1 Redistributable Package (x86)
@url-bin http://download.microsoft.com/download/e/1/c/e1c773de-73ba-494a-a5ba-f24906ecf088/vcredist_x86.exe

@package winscp
@category devel-tools 
@version 3.8.2
@notes secure transfer copy tool 
@url-bin http://kent.dl.sourceforge.net/sourceforge/winscp/winscp382setup.exe

