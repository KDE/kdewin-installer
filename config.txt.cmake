;kdewin installer configuration file 
@format 1.3
; this format is *prelimary* and may be changed without prior notice
@minversion 0.9.2
@timestamp 200807292000

; category notes 
@categorynotes qt Qt related packages 
@categorynotes devel-tools tools required or usefull for development
@categorynotes debug-tools tools required or usefull for debugging
@categorynotes KDE KDE packages

; to which category packages belongs to
@categorypackages win32libs aspell expat giflib iconv jasper jpeg lcms libbzip2 libidn libintl libintl libpng libxml2 libxslt openslp openssl tiff zlib shared-mime-info 
@categorypackages win32libs pcre-msvc  dbus-msvc 
@categorypackages win32libs pcre-mingw dbus-mingw
@categorypackages tools findutils grep mingw-utils diffutils patch
@categorypackages kdesupport kdewin32-msvc  kdewin-installer-msvc  vcredist      qimageblitz-msvc  qca-msvc  strigi-msvc  soprano-msvc  taglib-msvc  qt-msvc  phonon-msvc  fontconfig-msvc  freetype-msvc  poppler-msvc
@categorypackages kdesupport kdewin32-mingw kdewin-installer-mingw runtime-mingw qimageblitz-mingw qca-mingw strigi-mingw soprano-mingw taglib-mingw qt-mingw phonon-mingw fontconfig-mingw freetype-mingw poppler-mingw
@categorypackages KDE amarok-msvc  kdelibs-msvc  kdepimlibs-msvc  kdebase-apps-msvc  kdebase-runtime-msvc  kdeedu-msvc  kdegames-msvc  kdegraphics-msvc  kdenetwork-msvc  kdesdk-msvc  kdewebdev-msvc
@categorypackages KDE amarok-mingw kdelibs-mingw kdepimlibs-mingw kdebase-apps-mingw kdebase-runtime-mingw kdeedu-mingw kdegames-mingw kdegraphics-mingw kdenetwork-mingw kdesdk-mingw kdewebdev-mingw 
@categorypackages KDE koffice-msvc  ktorrent-msvc  
@categorypackages KDE koffice-mingw  ktorrent-mingw  
@categorypackages qt qt-msvc qt-mingw qt-static-msvc
@categorypackages KDE kde-l10n-af kde-l10n-ar kde-l10n-be kde-l10n-bg kde-l10n-bn kde-l10n-bn_IN kde-l10n-br kde-l10n-ca kde-l10n-cs kde-l10n-csb kde-l10n-cy kde-l10n-da kde-l10n-de kde-l10n-el kde-l10n-en_GB 
@categorypackages KDE kde-l10n-eo kde-l10n-es kde-l10n-et kde-l10n-eu kde-l10n-fa kde-l10n-fi kde-l10n-fr kde-l10n-fy kde-l10n-ga kde-l10n-gl kde-l10n-gu kde-l10n-ha kde-l10n-he kde-l10n-hi kde-l10n-hr kde-l10n-hsb 
@categorypackages KDE kde-l10n-hu kde-l10n-hy kde-l10n-is kde-l10n-it kde-l10n-ja kde-l10n-ka kde-l10n-kk kde-l10n-km kde-l10n-kn kde-l10n-ko kde-l10n-ku kde-l10n-lb kde-l10n-lt kde-l10n-lv kde-l10n-ml kde-l10n-mk 
@categorypackages KDE kde-l10n-ms kde-l10n-mt kde-l10n-nb kde-l10n-nds kde-l10n-ne kde-l10n-nl kde-l10n-nn kde-l10n-nso kde-l10n-oc kde-l10n-pa kde-l10n-pl kde-l10n-pt kde-l10n-pt_BR kde-l10n-ro kde-l10n-ru 
@categorypackages KDE kde-l10n-rw kde-l10n-se kde-l10n-sk kde-l10n-sl kde-l10n-sr kde-l10n-sr@latin kde-l10n-sv kde-l10n-ta kde-l10n-te kde-l10n-tg kde-l10n-th kde-l10n-tr kde-l10n-uk kde-l10n-uz kde-l10n-vi 
@categorypackages KDE kde-l10n-wa kde-l10n-xh kde-l10n-x-test kde-l10n-zh_CN kde-l10n-zh_HK kde-l10n-zh_TW

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
@deps kdelibs-mingw phonon-mingw pcre-mingw dbus-mingw kdewin32-mingw qimageblitz-mingw qca-mingw strigi-mingw soprano-mingw taglib-mingw aspell libbzip2 expat giflib iconv jasper jpeg libintl libpng libxml2 libxslt openssl  tiff zlib shared-mime-info
@deps kdepimlibs-mingw kdelibs-mingw
@deps kdebase-runtime-mingw kdepimlibs-mingw kdelibs-mingw
@deps kdebase-apps-mingw kdebase-runtime-mingw
@deps kdeedu-mingw kdebase-runtime-mingw kdelibs-mingw
@deps kdegames-mingw kdebase-runtime-mingw kdelibs-mingw phonon-mingw
@deps kdegraphics-mingw kdebase-runtime-mingw kdelibs-mingw fontconfig-mingw freetype-mingw poppler-mingw
@deps kdenetwork-mingw kdebase-runtime-mingw kdelibs-mingw
@deps kdesdk-mingw kdebase-runtime-mingw kdelibs-mingw kdepimlibs-mingw astyle
@deps kdetoys-mingw kdebase-runtime-mingw kdelibs-mingw
@deps kdewebdev-mingw kdebase-runtime-mingw kdelibs-mingw
@deps kdewin-installer-mingw qt-mingw
@deps kdewin32-mingw qt-mingw
@deps koffice-mingw kdebase-runtime-mingw kdelibs-mingw lcms
@deps ktorrent-mingw libgmp-mingw kdelibs-mingw qca-mingw
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
@deps kdelibs-msvc phonon-msvc pcre-msvc vcredist qt-msvc dbus-msvc kdewin32-msvc qimageblitz-msvc qca-msvc strigi-msvc soprano-msvc taglib-msvc aspell libbzip2 expat giflib iconv jasper jpeg libintl libpng libxml2 libxslt openssl tiff zlib shared-mime-info
@deps kdenetwork-msvc kdebase-runtime-msvc kdelibs-msvc
@deps kdepimlibs-msvc kdelibs-msvc
@deps kdesdk-msvc kdebase-runtime-msvc kdelibs-msvc astyle
@deps kdewebdev-msvc kdebase-runtime-msvc kdelibs-msvc
@deps kdewin-installer-msvc qt-msvc
@deps kdewin32-msvc qt-msvc
@deps kdetoys-msvc kdebase-runtime-msvc kdelibs-msvc
@deps koffice-msvc kdebase-runtime-msvc kdelibs-msvc lcms
@deps ktorrent-msvc libgmp-msvc kdelibs-msvc qca-msvc
@deps qt-msvc dbus-msvc vcredist
@deps strigi-msvc libbzip2 openssl zlib iconv libxml2
; compiler independent
@pkgnotes clucene high-performance, full-featured text search engine (required for compiling strigi) 
; mingw
@pkgnotes akonadi-mingw KDE4 Personal Information Manager framework
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
@pkgnotes koffice-mingw KDE Office application suite
@pkgnotes ktorrent-mingw KDE Torrent client
@pkgnotes libgmp-mingw devel-tools GNU MP library for abritary precision arithmetic
@pkgnotes phonon-mingw KDE KDE4 multimedia API
@pkgnotes poppler-mingw PDF rendering library based on xpdf-3.0
@pkgnotes pcre-mingw Perl-Compatible Regular Expressions (utilities)
@pkgnotes qca-mingw Qt Cryptographic Architecture (QCA)
@pkgnotes qimageblitz-mingw KDE image effect library
@pkgnotes qt-mingw C++ GUI application framework
@pkgnotes runtime-mingw Mingw runtime files
@pkgnotes soprano-mingw RDF storage solutions library
@pkgnotes strigi-mingw desktop search engine
@pkgnotes subversion-mingw software revision system
@pkgnotes taglib-mingw audio meda-data library
; msvc 
@pkgnotes akonadi-msvc KDE4 Personal Information Manager framework
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
@pkgnotes koffice-msvc KDE Office application suite
@pkgnotes ktorrent-msvc KDE Torrent client
@pkgnotes libgmp-msvc devel-tools GNU MP library for abritary precision arithmetic
@pkgnotes poppler-msvc PDF rendering library based on the xpdf-3.0
@pkgnotes phonon-msvc KDE KDE4 multimedia API
@pkgnotes pcre-msvc Perl-Compatible Regular Expressions (utilities)
@pkgnotes qca-msvc Qt Cryptographic Architecture (QCA)
@pkgnotes qimageblitz-msvc KDE image effect library
@pkgnotes qt-msvc C++ GUI application framework
@pkgnotes qt-static-msvc static release of qt package, which is used for build kdewin installer
@pkgnotes soprano-msvc RDF storage solutions library
@pkgnotes strigi-msvc desktop search engine
@pkgnotes taglib-msvc audio meda-data library
;other
@pkgnotes astyle source code formatter
@pkgnotes aspell A powerful spell checker, designed to replace ispell
@pkgnotes boost-headers portable C++ libraries
@pkgnotes byacc Berkeley parser generator
@pkgnotes bzip2 Bzip compression utility
@pkgnotes cfitsio library for the FITS (Flexible Image Transport System) file format
@pkgnotes diffutils Tools to find difference between two files
@pkgnotes dbus-mingw Freedesktop message bus system (daemon and clients)
@pkgnotes dbus-msvc Freedesktop message bus system (daemon and clients)
@pkgnotes eigen Lightweight C++ template library for vector and matrix math
@pkgnotes expat XML parser library written in C (development/documentation package)
@pkgnotes findutils Utilities to find files
@pkgnotes flex Fast lexical analyzer generator
@pkgnotes gawk GNU Awk programming language
@pkgnotes gettext GNU internationalization (i18n)
@pkgnotes gettext-tools GNU internationalization (i18n) tools
@pkgnotes gmm C++ template library for advanced linear algebra supporting dense and sparse representation of matrices and vectors
@pkgnotes grep Global Regular Expression Print
@pkgnotes giflib GIF file manipulation library (utilities and docs)
@pkgnotes gpgme GnuPG cryptography support library (runtime)
@pkgnotes gzip GNU zip compression tool
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
@pkgnotes md5sums md5 checksum tool
@pkgnotes mingw-utils Mingw Utilities
@pkgnotes openslp openslp client and libraries
@pkgnotes openssl The OpenSSL runtime environment
@pkgnotes patch Tool to merge diff files
@pkgnotes redland Resource Description Framework (RDF)
@pkgnotes sed Stream Editor
@pkgnotes shared-mime-info common mimetype library
@pkgnotes tar Tape ARchive
@pkgnotes tiff TIFF image library (utilities)
@pkgnotes wget Tool to get websites / files
@pkgnotes zlib The zlib compression and decompression library
; language packages
@pkgnotes kde-l10n-af Afrikaans language package
@pkgnotes kde-l10n-ar Arabic language package
@pkgnotes kde-l10n-az Azerbaijani language package
@pkgnotes kde-l10n-be Belarusian language package
@pkgnotes kde-l10n-bg Bulgarian language package
@pkgnotes kde-l10n-bn Bengali language package
@pkgnotes kde-l10n-bn_IN Bengali (India) language package
@pkgnotes kde-l10n-bo Tibetan language package
@pkgnotes kde-l10n-br Breton language package
@pkgnotes kde-l10n-bs Bosnian language package
@pkgnotes kde-l10n-ca Catalan language package
@pkgnotes kde-l10n-cs Czech language package
@pkgnotes kde-l10n-csb Kashubian language package
@pkgnotes kde-l10n-cy Welsh language package
@pkgnotes kde-l10n-da Danish language package
@pkgnotes kde-l10n-de German language package
@pkgnotes kde-l10n-el Greek language package
@pkgnotes kde-l10n-en_GB British English language package
@pkgnotes kde-l10n-eo Esperanto language package
@pkgnotes kde-l10n-es Spanish language package
@pkgnotes kde-l10n-et Estonian language package
@pkgnotes kde-l10n-eu Basque language package
@pkgnotes kde-l10n-fa Farsi language package
@pkgnotes kde-l10n-fi Finnish language package
@pkgnotes kde-l10n-fo Faroese language package
@pkgnotes kde-l10n-fr French language package
@pkgnotes kde-l10n-fy Frisian language package
@pkgnotes kde-l10n-ga Irish Gaelic language package
@pkgnotes kde-l10n-gl Galician language package
@pkgnotes kde-l10n-gu Gujarati language package
@pkgnotes kde-l10n-ha Hausa language package
@pkgnotes kde-l10n-he Hebrew language package
@pkgnotes kde-l10n-hi Hindi language package
@pkgnotes kde-l10n-hr Croatian language package
@pkgnotes kde-l10n-hsb Upper Sorbian language package
@pkgnotes kde-l10n-hu Hungarian language package
@pkgnotes kde-l10n-hy Armenian language package
@pkgnotes kde-l10n-id Indonesian language package
@pkgnotes kde-l10n-is Icelandic language package
@pkgnotes kde-l10n-it Italian language package
@pkgnotes kde-l10n-ja Japanese language package
@pkgnotes kde-l10n-ka Georgian language package
@pkgnotes kde-l10n-kk Kazakh language package
@pkgnotes kde-l10n-km Khmer language package
@pkgnotes kde-l10n-kn Kannada language package
@pkgnotes kde-l10n-ko Korean language package
@pkgnotes kde-l10n-ku Kurdish language package
@pkgnotes kde-l10n-lb Luxembourgish language package
@pkgnotes kde-l10n-lo Lao language package
@pkgnotes kde-l10n-lt Lithuanian language package
@pkgnotes kde-l10n-lv Latvian language package
@pkgnotes kde-l10n-mi Maori language package
@pkgnotes kde-l10n-ml Malayalam language package
@pkgnotes kde-l10n-mk Macedonian language package
@pkgnotes kde-l10n-mn Mongolian language package
@pkgnotes kde-l10n-ms Malay language package
@pkgnotes kde-l10n-mt Maltese language package
@pkgnotes kde-l10n-ne Nepali language package
@pkgnotes kde-l10n-nl Dutch language package
@pkgnotes kde-l10n-nb Norwegian Bokmal language package
@pkgnotes kde-l10n-nds Low Saxon language package
@pkgnotes kde-l10n-nso Northern Sotho language package
@pkgnotes kde-l10n-nn Norwegian Nynorsk language package
@pkgnotes kde-l10n-oc Occitan language package
@pkgnotes kde-l10n-pa Panjabi/Punjabi language package
@pkgnotes kde-l10n-pl Polish language package
@pkgnotes kde-l10n-pt Portuguese language package
@pkgnotes kde-l10n-pt_BR Brazilian Portuguese language package
@pkgnotes kde-l10n-ro Romanian language package
@pkgnotes kde-l10n-ru Russian language package
@pkgnotes kde-l10n-rw Kinyarwanda language package
@pkgnotes kde-l10n-se Northern Sami language package
@pkgnotes kde-l10n-sk Slovak language package
@pkgnotes kde-l10n-sl Slovenian language package
@pkgnotes kde-l10n-sq Albanian language package
@pkgnotes kde-l10n-sr Serbian language package
@pkgnotes kde-l10n-sr@latin Serbian Latin language package
@pkgnotes kde-l10n-ss Swati language package
@pkgnotes kde-l10n-sv Swedish language package
@pkgnotes kde-l10n-ta Tamil language package
@pkgnotes kde-l10n-te Telugu language package
@pkgnotes kde-l10n-tg Tajik language package
@pkgnotes kde-l10n-th Thai language package
@pkgnotes kde-l10n-tr Turkish language package
@pkgnotes kde-l10n-tt Tatar language package
@pkgnotes kde-l10n-uk Ukrainian language package
@pkgnotes kde-l10n-uz Uzbek language package
@pkgnotes kde-l10n-ven Venda language package
@pkgnotes kde-l10n-vi Vietnamese language package
@pkgnotes kde-l10n-wa Walloon language package
@pkgnotes kde-l10n-xh Xhosa language package
@pkgnotes kde-l10n-zh_CN Chinese Simplified language package
@pkgnotes kde-l10n-zh_HK Chinese (Hong Kong) language package
@pkgnotes kde-l10n-zh_TW Chinese Traditional language package
@pkgnotes kde-l10n-zu Zulu language package

;======================================================
; external packages
;======================================================
;------------------------------------------------------
; debugging tools
;------------------------------------------------------
@package dbg_x86
@category debug-tools
@version 6.6.07.5
@notes msvc Debugging Tools
@url-bin http://msdl.microsoft.com/download/symbols/debuggers/dbg_x86_6.6.07.5.exe
@nomd5

@package regmon
@category debug-tools
@version 7.04
@notes registry monitor
@url-bin http://download.sysinternals.com/Files/Regmon.zip
@relocate ^ bin/
@nomd5

@package filemon
@category debug-tools
@version 7.04
@notes filesystem monitor
@url-bin http://download.sysinternals.com/Files/Filemon.zip
@relocate ^ bin/
@nomd5

@package procmon
@category debug-tools
@version 7.04
@notes process monitor
@url-bin http://download.sysinternals.com/Files/ProcessMonitor.zip
@relocate ^ bin/
@nomd5

@package gdb
@category debug-tools
@version 6.3-2
@notes mingw debugger 
@url-bin http://heanet.dl.sourceforge.net/sourceforge/mingw/gdb-6.3-2.exe
@nomd5

@package debugview
@category debug-tools
@version 4.64
@notes debug message monitor
@url-bin http://download.sysinternals.com/Files/DebugView.zip
@relocate ^ bin/
@nomd5

@package Process Explorer
@category debug-tools
@version 11.04
@notes Process Explorer
@url-bin http://download.sysinternals.com/Files/ProcessExplorer.zip
@relocate ^ bin/
@nomd5

@package perl 
@notes Perl Installer
@category tools
@version 5.8.8
@url-bin http://downloads.activestate.com/ActivePerl/Windows/5.8/ActivePerl-5.8.8.819-MSWin32-x86-267479.msi 
;@packageoptions
@nomd5

@package zip
@category tools 
@version 2.31
@notes zip tool 
@url-bin http://switch.dl.sourceforge.net/sourceforge/gnuwin32/zip-2.31-bin.zip
@url-lib http://switch.dl.sourceforge.net/sourceforge/gnuwin32/zip-2.31-lib.zip
@url-doc http://switch.dl.sourceforge.net/sourceforge/gnuwin32/zip-2.31-doc.zip
@url-src http://switch.dl.sourceforge.net/sourceforge/gnuwin32/zip-2.31-src.zip
@nomd5

;------------------------------------------------------
; devel-tools
;------------------------------------------------------
@package cmake
@category devel-tools 
@notes platform independent build tool 
@version 2.6.0
@url-bin http://www.cmake.org/files/v2.6/cmake-2.6.0-win32-x86.zip cmake-2.6.0-bin.zip
@url-src http://www.cmake.org/files/v2.4/cmake-2.6.0.zip cmake-2.6.0-src.zip
@relocate cmake-2.6.0-win32-x86 .
@nomd5

@package depends
@notes Dependency Walker
@category debug-tools
@version 2.2
@url-bin http://www.dependencywalker.com/depends22_x86.zip depends-2.2-bin.zip
@relocate ^ bin/
@nomd5

@package directx-sdk
@category devel-tools
@notes DirectX SDK April 2007
@version 2007-04
@url-bin http://download.microsoft.com/download/3/b/d/3bd0a88d-4e99-49fc-931d-9eeb11ecd2a3/dxsdk_apr2007.exe
@nomd5

@package doxygen
@notes Source code documentation generator tool
@category devel-tools
@version 1.5.4
@url-bin http://ftp.stack.nl/pub/users/dimitri/doxygen-1.5.4.windows.bin.zip doxygen-1.5.4-bin.zip
@relocate ^ bin/
@nomd5

@package psdk-msvc
@category devel-tools
@notes MS Platform SDK 2003
@version r2
@url-bin http://download.microsoft.com/download/f/a/d/fad9efde-8627-4e7a-8812-c351ba099151/PSDK-x86.exe
@nomd5

@package gcc-mingw
@version 5.1.3
@category devel-tools
@notes gcc for windows 
@url-bin http://heanet.dl.sourceforge.net/sourceforge/mingw/MinGW-5.1.3.exe
@nomd5

@package subversion
@version 1.4.5
@category devel-tools
@notes software revision system 
@url-bin http://subversion.tigris.org/files/documents/15/39559/svn-1.4.5-setup.exe
@nomd5

@package tortoisecvs
@category devel-tools 
@version 1.8.31
@notes cvs explorer integration 
@url-bin http://prdownloads.sourceforge.net/tortoisecvs/TortoiseCVS-1.8.31.exe
@nomd5

@package tortoisesvn
@category devel-tools 
@version 1.4.3
@notes svn explorer integration 
@url-bin http://heanet.dl.sourceforge.net/sourceforge/tortoisesvn/TortoiseSVN-1.4.3.8645-win32-svn-1.4.3.msi
@nomd5

@package upx
@notes Ultimate Packer for eXecutables with lzma support
@category devel-tools
@version 3.0.0
@url-bin http://upx.sourceforge.net/download/upx300w.zip upx-3.0.0-bin.zip
; relocate directories in zip files 
@relocate upx300w bin
@nomd5

@package vcexpress-en-msvc
@category devel-tools
@version 2005
@notes Visual Studio Express 2005 (english version)
@url-bin http://download.microsoft.com/download/8/3/a/83aad8f9-38ba-4503-b3cd-ba28c360c27b/ENU/vcsetup.exe
@nomd5

@package vcexpress-de-msvc
@category devel-tools
@version 2005
@notes Visual Studio Express 2005 (german version)
@url-bin http://download.microsoft.com/download/8/3/a/83aad8f9-38ba-4503-b3cd-ba28c360c27b/DEU/vcsetup.exe
@nomd5

@package vcredist
@category devel-tools
@version 8.0.50727.762
@notes Microsoft Visual C++ 2005 SP1 Redistributable Package (x86)
@url-bin http://download.microsoft.com/download/e/1/c/e1c773de-73ba-494a-a5ba-f24906ecf088/vcredist_x86.exe
@nomd5

@package winscp
@category devel-tools 
@version 3.8.2
@notes secure transfer copy tool 
@url-bin http://kent.dl.sourceforge.net/sourceforge/winscp/winscp382setup.exe
@nomd5
