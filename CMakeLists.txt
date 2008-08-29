set (NAME kdewin-installer)
project(${NAME})

cmake_minimum_required(VERSION 2.4.5)

set (VERSION_MAJOR 0)
set (VERSION_MINOR 9)
set (VERSION_REVISION 3)
set (VERSION_PATCH 0)
set (VERSION "${VERSION_MAJOR}.${VERSION_MINOR}.${VERSION_REVISION}")
set (VERSION_PATCH "${VERSION_MAJOR}.${VERSION_MINOR}.${VERSION_REVISION}-${VERSION_PATCH}")

# where to look first for cmake modules, before ${CMAKE_ROOT}/Modules/ is checked
set(CMAKE_MODULE_PATH "${CMAKE_SOURCE_DIR}/cmake/modules")

#set (CMAKE_SUPPRESS_REGENERATION 1)
find_package(Qt4 REQUIRED)

if (WIN32 AND CMAKE_MAJOR_VERSION EQUAL 2 AND CMAKE_MINOR_VERSION EQUAL 6)
# emerge workaround for cmake 2.6. qt-static 4.3.2 combination which is not able to detect uic and moc correctly
# we need also to set the environment variable QMAKESPEC to %KDEROOT%\mkspecs/<compiler>
    FIND_PROGRAM(_QT_UIC_EXECUTABLE
        NAMES uic
        PATHS ${QT_BINARY_DIR}
        NO_DEFAULT_PATH
    )
    set (QT_UIC_EXECUTABLE ${_QT_UIC_EXECUTABLE})

    FIND_PROGRAM(_QT_MOC_EXECUTABLE
        NAMES moc
        PATHS ${QT_BINARY_DIR}
        NO_DEFAULT_PATH
    )
    set (QT_MOC_EXECUTABLE ${_QT_MOC_EXECUTABLE})
endif (WIN32 AND CMAKE_MAJOR_VERSION EQUAL 2 AND CMAKE_MINOR_VERSION EQUAL 6)

find_package(UPX)
find_package(KDEWIN_Packager)
find_package(WinSCP)

# make cmake 2.6 happy
if(COMMAND cmake_policy)
    cmake_policy(SET CMP0003 NEW)
endif(COMMAND cmake_policy)
	      
OPTION(SEVENZIP_UNPACK_SUPPORT "Enable 7-zip unpacking support" OFF)
OPTION(BZIP2_UNPACK_SUPPORT "Enable tar.bz2 unpacking support" ON)
# todo detect static qt library automatically
OPTION(STATIC_BUILD "enable static build support" ON)
OPTION(INSTALL_SYSTEM_LIBS "install required system libraries too (only in shared mode)" OFF)
OPTION(BUILD_TESTS "build test applications " ON)

ENABLE_TESTING()

# -------------------------------------------------------
# build type settings
# -------------------------------------------------------
# load qt configuration
QT_QUERY_QMAKE(QT_CONFIG "CONFIG")

if (QT_CONFIG MATCHES ".*shared.*")
    set (STATIC_BUILD 0)
    MESSAGE(STATUS "Qt is build in shared mode - use shared build mode too")
else (QT_CONFIG MATCHES ".*shared.*")
    set (STATIC_BUILD 1)
    if (MSVC)
        QT_QUERY_QMAKE(QMAKE_CFLAGS_RELEASE "QMAKE_CFLAGS_RELEASE")
        if (QMAKE_CFLAGS_RELEASE MATCHES ".*-MT.*")
            MESSAGE(STATUS "Qt is build in static mode - use static build mode too")
            set (MSVCRT_COMPILE_FLAGS "-MT /Os /Oy /GL")
            set (MSVCRT_LINK_FLAGS "/NODEFAULTLIB:msvcrt")
        else (QMAKE_CFLAGS_RELEASE MATCHES ".*-MT.*")
            set (MSVCRT_COMPILE_FLAGS "-MD")
            set (MSVCRT_LINK_FLAGS "")
            MESSAGE(STATUS "Qt is build in static mode using shared runtime - use static build mode too")
        endif (QMAKE_CFLAGS_RELEASE MATCHES ".*-MT.*")
    endif (MSVC)
endif (QT_CONFIG MATCHES ".*shared.*")

# Set a default build type for single-configuration
# CMake generators if no build type is set.
MESSAGE(STATUS "Build Type:" ${CMAKE_BUILD_TYPE})


# PACK_EXECUTABLES enables packing of executable
set (PACK_EXECUTABLE 0)

if (CMAKE_BUILD_TYPE STREQUAL Release OR CMAKE_BUILD_TYPE STREQUAL MinSizeRel OR CMAKE_BUILD_TYPE STREQUAL RelWithDebInfo)
    if (STATIC_BUILD)
        if (UPX_EXECUTABLE)
            set (PACK_EXECUTABLE 1)
            message(STATUS "upx found, packing of executables enabled")
        endif (UPX_EXECUTABLE)
        set (INSTALL_SYSTEM_LIBS 0)
    else (STATIC_BUILD)
        # add shared mode options
    endif (STATIC_BUILD)
    # disable qDebug output
    #add_definitions(-DQT_NO_DEBUG_OUTPUT)
else (CMAKE_BUILD_TYPE STREQUAL Release OR CMAKE_BUILD_TYPE STREQUAL MinSizeRel OR CMAKE_BUILD_TYPE STREQUAL RelWithDebInfo)
    if (STATIC_BUILD)
        set (INSTALL_SYSTEM_LIBS 0)
    endif (STATIC_BUILD)
endif (CMAKE_BUILD_TYPE STREQUAL Release OR CMAKE_BUILD_TYPE STREQUAL MinSizeRel OR CMAKE_BUILD_TYPE STREQUAL RelWithDebInfo)

if (STATIC_BUILD AND MSVC)
    set (CMAKE_C_FLAGS_RELEASE "${MSVCRT_COMPILE_FLAGS} /O2 /Ob2 /D NDEBUG")
    set (CMAKE_CXX_FLAGS_RELEASE "${MSVCRT_COMPILE_FLAGS} /O2 /Ob2 /D NDEBUG")
    set (CMAKE_EXE_LINKER_FLAGS_RELEASE "/INCREMENTAL:NO ${MSVCRT_LINK_FLAGS}")
    set (CMAKE_C_FLAGS_RELWITHDEBINFO "${MSVCRT_COMPILE_FLAGS} /O2 /Ob2 /D NDEBUG")
    set (CMAKE_CXX_FLAGS_RELWITHDEBINFO "${MSVCRT_COMPILE_FLAGS} /O2 /Ob2 /D NDEBUG")
    set (CMAKE_EXE_LINKER_FLAGS_RELWITHDEBINFO "/INCREMENTAL:NO ${MSVCRT_LINK_FLAGS}")
    set (CMAKE_C_FLAGS_MINSIZEREL "${MSVCRT_COMPILE_FLAGS} /O1 /Ob1 /D NDEBUG")
    set (CMAKE_CXX_FLAGS_MINSIZEREL "${MSVCRT_COMPILE_FLAGS} /O1 /Ob1 /D NDEBUG")
    set (CMAKE_EXE_LINKER_FLAGS_MINSIZEREL "/INCREMENTAL:NO ${MSVCRT_LINK_FLAGS}")
    set (CMAKE_C_FLAGS_DEBUG "/D_DEBUG ${MSVCRT_COMPILE_FLAGS}d /Zi  /Ob0 /Od /RTC1 ")
    set (CMAKE_CXX_FLAGS_DEBUG "/D_DEBUG ${MSVCRT_COMPILE_FLAGS}d /Zi /Ob0 /Od /RTC1")
    set (CMAKE_EXE_LINKER_FLAGS_DEBUG "/debug /INCREMENTAL:NO ${MSVCRT_LINK_FLAGS} /NODEFAULTLIB:libcmt libcmtd.lib")
endif (STATIC_BUILD AND MSVC)

# where to store executables
if (WIN32 OR CYGWIN)
    set (LIBRARY_OUTPUT_PATH  ${CMAKE_BINARY_DIR}/bin)
else (WIN32 OR CYGWIN)
    set (LIBRARY_OUTPUT_PATH  ${CMAKE_BINARY_DIR}/lib)
endif (WIN32 OR CYGWIN)
set (EXECUTABLE_OUTPUT_PATH ${CMAKE_BINARY_DIR}/bin)

# add 3rdparty sources and definitions
include(3rdparty/CMakeLists.txt)
# this will define .... _HEADERS, ...._SOURCES, ..._INCLUDE_DIR. for ... = quazip, bzip2, curl, filter,md5
# add_subdirectory does not work for unknown reason

include_directories(
  ${CMAKE_SOURCE_DIR}
  ${CMAKE_BINARY_DIR}
  ${CMAKE_CURRENT_SOURCE_DIR}/shared
  ${QT_INCLUDES}
  $ENV{QMAKESPEC}
)
configure_file(${CMAKE_SOURCE_DIR}/config.h.cmake ${CMAKE_BINARY_DIR}/config.h)

#
# create mirror config files 
# 
set (SITE_URL "@url .")
set (SITE_URL_LIST ";")
configure_file(${CMAKE_SOURCE_DIR}/config.txt.cmake ${CMAKE_BINARY_DIR}/config/www.winkde.org/config.txt)

set (SITE_URL "@url http://downloads.sourceforge.net/kde-windows")
set (SITE_URL_LIST "@url-list http://sourceforge.net/project/showfiles.php?group_id=214730")
configure_file(${CMAKE_SOURCE_DIR}/config.txt.cmake ${CMAKE_BINARY_DIR}/config/sourceforge.net/config.txt)

set (SITE_URL "@url .")
set (SITE_URL_LIST ";")
configure_file(${CMAKE_SOURCE_DIR}/config.txt.cmake ${CMAKE_BINARY_DIR}/config/ktown.kde.org/config.txt)

set (RELEASE_PATH "/unstable/4.0.83/win32")
set (RELEASE "4.0.83")
configure_file(${CMAKE_SOURCE_DIR}/mirrors.lst.cmake ${CMAKE_BINARY_DIR}/config/mirrors.lst)

#FIXME: ${BZIP2_LIBRARIES} -> 3rdparty
if (UNIX)
  set (CONSOLE_LIBS pthread dl z rt ${BZIP2_LIBRARIES} ${CURL_LIBRARIES} shared-console)
  set (GUI_LIBS pthread dl z rt ${BZIP2_LIBRARIES} ${CURL_LIBRARIES} shared-gui)
  if (STATIC_BUILD)
    find_package(X11)
    find_package(Freetype)
# does not work with cmake provided find_packages
#    find_package(Fontconfig)
#    find_package(png)
     set (GUI_LIBS ${GUI_LIBS} ${QT_QTCORE_LIBRARY} ${X11_LIBRARIES} ${FREETYPE_LIBRARIES} crypto ssl fontconfig png Xrandr Xfixes)
  endif(STATIC_BUILD)

else(UNIX)
  if (MINGW)
    set (FIRST_LIBS mingw32)
  endif(MINGW)
  # required for static linking
  set (CONSOLE_LIBS shared-console ${QUAZIP_LIBRARIES} ${BZIP2_LIBRARIES} ${QT_QTCORE_LIBRARY} ${QT_QTMAIN_LIBRARY} ${CURL_LIBRARIES} kernel32 user32 shell32 uuid ole32 advapi32 ws2_32 shlwapi )
  set (GUI_LIBS shared-gui ${QUAZIP_LIBRARIES} ${BZIP2_LIBRARIES} ${QT_QTCORE_LIBRARY} ${QT_QTMAIN_LIBRARY} ${QT_QTGUI_LIBRARY} ${CURL_LIBRARIES} gdi32 comdlg32 oleaut32 imm32 winmm winspool ws2_32 ole32 uuid user32 advapi32 shlwapi )
  # when qt 4.4 is build with directx support dxguid is required 
  # how to detect that qt 4.4 is build with directx ? 
  if (QT_VERSION_MAJOR EQUAL 4 AND QT_VERSION_MINOR GREATER 3) 
	  list(APPEND GUI_LIBS dxguid)
  endif (QT_VERSION_MAJOR EQUAL 4 AND QT_VERSION_MINOR GREATER 3) 
endif (UNIX)


# our targets
add_subdirectory(shared)
add_subdirectory(console)
add_subdirectory(gui)
#add_subdirectory(enduser)
add_subdirectory(tests)
add_subdirectory(packager)
add_subdirectory(doc)
add_subdirectory(configer)
if(WIN32)
#  add_subdirectory(menubuilder)
endif(WIN32)
#add_subdirectory(tools)

if (WIN32)
    include(Win32Macros)
    addExplorerWrapper()
endif(WIN32)

install( FILES cmake/modules/FindKDEWIN_Packager.cmake  DESTINATION  share/apps/cmake/modules )

# install required shared libraries
if (INSTALL_SYSTEM_LIBS)
    set (QT_QTCORE_LIBRARY ${QT_LIBRARY_DIR}/../bin/QtCore4.dll)
    set (QT_QTGUI_LIBRARY ${QT_LIBRARY_DIR}/../bin/QtGui4.dll)
    set (QT_QTNETWORK_LIBRARY ${QT_LIBRARY_DIR}/../bin/QtNetwork4.dll)

    install_files (/bin FILES ${QT_QTCORE_LIBRARY})
    install_files (/bin FILES  ${QT_QTGUI_LIBRARY})
    install_files (/bin FILES  ${QT_QTNETWORK_LIBRARY})

    if (MINGW)
        find_file(MINGW_DLL mingwm10.dll)
        install_files (/bin FILES  ${MINGW_DLL})
    else (MINGW)
        INCLUDE(InstallRequiredSystemLibraries)
    endif (MINGW)
endif (INSTALL_SYSTEM_LIBS)

if (KDEWIN_PACKAGER_FOUND)
    KDEWIN_PACKAGER(${NAME} ${VERSION} "KDEWIN installer" "")
endif (KDEWIN_PACKAGER_FOUND)

SET(CPACK_PACKAGE_DESCRIPTION_SUMMARY "KDEWIN Installer")
SET(CPACK_PACKAGE_VENDOR "KDE Windows Team")
#SET(CPACK_PACKAGE_DESCRIPTION_FILE "${CMAKE_SOURCE_DIR}/README")
#SET(CPACK_RESOURCE_FILE_LICENSE "${CMAKE_SOURCE_DIR}/../COPYING")
# duplicated from VERSION
SET(CPACK_PACKAGE_VERSION_MAJOR ${VERSION_MAJOR})
SET(CPACK_PACKAGE_VERSION_MINOR ${VERSION_MINOR})
SET(CPACK_PACKAGE_VERSION_PATCH ${VERSION_PATCH})
SET(CPACK_PACKAGE_INSTALL_DIRECTORY "kdewin-installer-${VERSION_MAJOR}.${VERSION_MINOR}")
IF(WIN32 AND NOT UNIX)
  SET(CPACK_GENERATOR NSIS)     # can be NSIS, STGZ, TBZ2, TGZ, TZ and ZIP
  SET(CPACK_NSIS_COMPRESSOR "/SOLID lzma")
  # There is a bug in NSI that does not handle full unix paths properly. Make
  # sure there is at least one set of four (4) backlasshes.
# SET(CPACK_PACKAGE_ICON "${CMake_SOURCE_DIR}/Utilities/Release\\\\InstallIcon.bmp")
  SET(CPACK_NSIS_INSTALLED_ICON_NAME "bin\\\\kdewin-installer-gui.exe")
  SET(CPACK_NSIS_DISPLAY_NAME "KDEWIN Installer")
  SET(CPACK_NSIS_HELP_LINK "http:\\\\\\\\www.kdelibs.com")
  SET(CPACK_NSIS_URL_INFO_ABOUT "http:\\\\\\\\www.kdelibs.com")
  SET(CPACK_NSIS_CONTACT "kde-windows@kde.org")
  SET(CPACK_NSIS_MODIFY_PATH ON)
ELSE(WIN32 AND NOT UNIX)
  SET(CPACK_STRIP_FILES "bin/MyExecutable")
  SET(CPACK_SOURCE_STRIP_FILES "")
ENDIF(WIN32 AND NOT UNIX)
SET(CPACK_PACKAGE_EXECUTABLES "kdewin-installer-gui" "KDEWIN Installer")
INCLUDE(CPack)


if (WINSCP_FOUND)
    UPLOAD()
endif (WINSCP_FOUND)

