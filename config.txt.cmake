;kdewin installer configuration file 
@format 1.3
; this format is *prelimary* and may be changed without prior notice
@minversion 0.9.1
@timestamp 200803160830

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
@categorypackages kdesupport kdewin32-msvc  kdewin-installer-msvc  vcredist      qimageblitz-msvc  qca-msvc  strigi-msvc  soprano-msvc  taglib-msvc  qt-msvc  phonon-msvc  fontconfig-msvc  freetype-msvc  poppler-msvc
@categorypackages kdesupport kdewin32-mingw kdewin-installer-mingw runtime-mingw qimageblitz-mingw qca-mingw strigi-mingw soprano-mingw taglib-mingw qt-mingw phonon-mingw fontconfig-mingw freetype-mingw poppler-mingw
@categorypackages KDE amarok-msvc  kdelibs-msvc  kdepimlibs-msvc  kdebase-apps-msvc  kdebase-runtime-msvc  kdeedu-msvc  kdegames-msvc  kdegraphics-msvc  kdenetwork-msvc  kdesdk-msvc  kdewebdev-msvc
@categorypackages KDE amarok-mingw kdelibs-mingw kdepimlibs-mingw kdebase-apps-mingw kdebase-runtime-mingw kdeedu-mingw kdegames-mingw kdegraphics-mingw kdenetwork-mingw kdesdk-mingw kdewebdev-mingw 
@categorypackages qt qt-msvc qt-mingw qt-static-msvc
@categorypackages KDE kde-i18n-af kde-i18n-ar kde-i18n-be kde-i18n-bg kde-i18n-bn kde-i18n-bn_IN kde-i18n-br kde-i18n-ca kde-i18n-cs kde-i18n-csb kde-i18n-cy kde-i18n-da kde-i18n-de kde-i18n-el kde-i18n-en_GB 
@categorypackages KDE kde-i18n-eo kde-i18n-es kde-i18n-et kde-i18n-eu kde-i18n-fa kde-i18n-fi kde-i18n-fr kde-i18n-fy kde-i18n-ga kde-i18n-gl kde-i18n-gu kde-i18n-ha kde-i18n-he kde-i18n-hi kde-i18n-hr kde-i18n-hsb 
@categorypackages KDE kde-i18n-hu kde-i18n-hy kde-i18n-is kde-i18n-it kde-i18n-ja kde-i18n-ka kde-i18n-kk kde-i18n-km kde-i18n-kn kde-i18n-ko kde-i18n-ku kde-i18n-lb kde-i18n-lt kde-i18n-lv kde-i18n-ml kde-i18n-mk 
@categorypackages KDE kde-i18n-ms kde-i18n-mt kde-i18n-nb kde-i18n-nds kde-i18n-ne kde-i18n-nl kde-i18n-nn kde-i18n-nso kde-i18n-oc kde-i18n-pa kde-i18n-pl kde-i18n-pt kde-i18n-pt_BR kde-i18n-ro kde-i18n-ru 
@categorypackages KDE kde-i18n-rw kde-i18n-se kde-i18n-sk kde-i18n-sl kde-i18n-sr kde-i18n-sr@latin kde-i18n-sv kde-i18n-ta kde-i18n-te kde-i18n-tg kde-i18n-th kde-i18n-tr kde-i18n-uk kde-i18n-uz kde-i18n-vi 
@categorypackages KDE kde-i18n-wa kde-i18n-xh kde-i18n-x-test kde-i18n-zh_CN kde-i18n-zh_HK kde-i18n-zh_TW

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
@deps kdegraphics-mingw kdebase-runtime-mingw kdelibs-mingw fontconfig-mingw freetype-mingw poppler-mingw
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
@deps kdegraphics-msvc kdebase-runtime-msvc kdelibs-msvc fontconfig-msvc freetype-msvc poppler-msvc
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
@pkgnotes fontconfig-mingw library for font customization and configuration
@pkgnotes freetype-mingw A Free, High-Quality, and Portable Font Engine
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
@pkgnotes poppler-mingw PDF rendering library based on the xpdf-3.0
@pkgnotes qca-mingw Qt Cryptographic Architecture (QCA)
@pkgnotes qimageblitz-mingw KDE image effect library
@pkgnotes qt-mingw C++ GUI application framework
@pkgnotes soprano-mingw RDF storage solutions library
@pkgnotes strigi-mingw desktop search engine
@pkgnotes taglib-mingw audio meda-data library
; msvc 
@pkgnotes amarok-msvc media player 
@pkgnotes fontconfig-msvc library for font customization and configuration
@pkgnotes freetype-msvc A Free, High-Quality, and Portable Font Engine
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
@pkgnotes poppler-msvc PDF rendering library based on the xpdf-3.0
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
; language packages
@pkgnotes kde-i18n-af Afrikaans language package
@pkgnotes kde-i18n-ar Arabic language package
@pkgnotes kde-i18n-az Azerbaijani language package
@pkgnotes kde-i18n-be Belarusian language package
@pkgnotes kde-i18n-bg Bulgarian language package
@pkgnotes kde-i18n-bn Bengali language package
@pkgnotes kde-i18n-bn_IN Bengali (India) language package
@pkgnotes kde-i18n-bo Tibetan language package
@pkgnotes kde-i18n-br Breton language package
@pkgnotes kde-i18n-bs Bosnian language package
@pkgnotes kde-i18n-ca Catalan language package
@pkgnotes kde-i18n-cs Czech language package
@pkgnotes kde-i18n-csb Kashubian language package
@pkgnotes kde-i18n-cy Welsh language package
@pkgnotes kde-i18n-da Danish language package
@pkgnotes kde-i18n-de German language package
@pkgnotes kde-i18n-el Greek language package
@pkgnotes kde-i18n-en_GB British English language package
@pkgnotes kde-i18n-eo Esperanto language package
@pkgnotes kde-i18n-es Spanish language package
@pkgnotes kde-i18n-et Estonian language package
@pkgnotes kde-i18n-eu Basque language package
@pkgnotes kde-i18n-fa Farsi language package
@pkgnotes kde-i18n-fi Finnish language package
@pkgnotes kde-i18n-fo Faroese language package
@pkgnotes kde-i18n-fr French language package
@pkgnotes kde-i18n-fy Frisian language package
@pkgnotes kde-i18n-ga Irish Gaelic language package
@pkgnotes kde-i18n-gl Galician language package
@pkgnotes kde-i18n-gu Gujarati language package
@pkgnotes kde-i18n-ha Hausa language package
@pkgnotes kde-i18n-he Hebrew language package
@pkgnotes kde-i18n-hi Hindi language package
@pkgnotes kde-i18n-hr Croatian language package
@pkgnotes kde-i18n-hsb Upper Sorbian language package
@pkgnotes kde-i18n-hu Hungarian language package
@pkgnotes kde-i18n-hy Armenian language package
@pkgnotes kde-i18n-id Indonesian language package
@pkgnotes kde-i18n-is Icelandic language package
@pkgnotes kde-i18n-it Italian language package
@pkgnotes kde-i18n-ja Japanese language package
@pkgnotes kde-i18n-ka Georgian language package
@pkgnotes kde-i18n-kk Kazakh language package
@pkgnotes kde-i18n-km Khmer language package
@pkgnotes kde-i18n-kn Kannada language package
@pkgnotes kde-i18n-ko Korean language package
@pkgnotes kde-i18n-ku Kurdish language package
@pkgnotes kde-i18n-lb Luxembourgish language package
@pkgnotes kde-i18n-lo Lao language package
@pkgnotes kde-i18n-lt Lithuanian language package
@pkgnotes kde-i18n-lv Latvian language package
@pkgnotes kde-i18n-mi Maori language package
@pkgnotes kde-i18n-ml Malayalam language package
@pkgnotes kde-i18n-mk Macedonian language package
@pkgnotes kde-i18n-mn Mongolian language package
@pkgnotes kde-i18n-ms Malay language package
@pkgnotes kde-i18n-mt Maltese language package
@pkgnotes kde-i18n-ne Nepali language package
@pkgnotes kde-i18n-nl Dutch language package
@pkgnotes kde-i18n-nb Norwegian Bokmal language package
@pkgnotes kde-i18n-nds Low Saxon language package
@pkgnotes kde-i18n-nso Northern Sotho language package
@pkgnotes kde-i18n-nn Norwegian Nynorsk language package
@pkgnotes kde-i18n-oc Occitan language package
@pkgnotes kde-i18n-pa Panjabi/Punjabi language package
@pkgnotes kde-i18n-pl Polish language package
@pkgnotes kde-i18n-pt Portuguese language package
@pkgnotes kde-i18n-pt_BR Brazilian Portuguese language package
@pkgnotes kde-i18n-ro Romanian language package
@pkgnotes kde-i18n-ru Russian language package
@pkgnotes kde-i18n-rw Kinyarwanda language package
@pkgnotes kde-i18n-se Northern Sami language package
@pkgnotes kde-i18n-sk Slovak language package
@pkgnotes kde-i18n-sl Slovenian language package
@pkgnotes kde-i18n-sq Albanian language package
@pkgnotes kde-i18n-sr Serbian language package
@pkgnotes kde-i18n-sr@latin Serbian Latin language package
@pkgnotes kde-i18n-ss Swati language package
@pkgnotes kde-i18n-sv Swedish language package
@pkgnotes kde-i18n-ta Tamil language package
@pkgnotes kde-i18n-te Telugu language package
@pkgnotes kde-i18n-tg Tajik language package
@pkgnotes kde-i18n-th Thai language package
@pkgnotes kde-i18n-tr Turkish language package
@pkgnotes kde-i18n-tt Tatar language package
@pkgnotes kde-i18n-uk Ukrainian language package
@pkgnotes kde-i18n-uz Uzbek language package
@pkgnotes kde-i18n-ven Venda language package
@pkgnotes kde-i18n-vi Vietnamese language package
@pkgnotes kde-i18n-wa Walloon language package
@pkgnotes kde-i18n-xh Xhosa language package
@pkgnotes kde-i18n-zh_CN Chinese Simplified language package
@pkgnotes kde-i18n-zh_HK Chinese (Hong Kong) language package
@pkgnotes kde-i18n-zh_TW Chinese Traditional language package
@pkgnotes kde-i18n-zu Zulu language package

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

