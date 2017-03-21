set (NAME kdewin-installer)
project(${NAME})

cmake_minimum_required(VERSION 2.8.11)

set (INSTALLER_VERSION_MAJOR 1)
set (INSTALLER_VERSION_MINOR 0)
set (INSTALLER_VERSION_PATCH 1)
set (VERSION "${INSTALLER_VERSION_MAJOR}.${INSTALLER_VERSION_MINOR}")
set (VERSION_PATCH "${INSTALLER_VERSION_MAJOR}.${INSTALLER_VERSION_MINOR}.${INSTALLER_VERSION_PATCH}")

# where to look first for cmake modules, before ${CMAKE_ROOT}/Modules/ is checked
set(CMAKE_MODULE_PATH "${CMAKE_SOURCE_DIR}/cmake/modules")

# make cmake 2.6 happy
if(COMMAND cmake_policy)
    cmake_policy(SET CMP0003 NEW)
endif(COMMAND cmake_policy)

if(CMAKE_MAJOR_VERSION STREQUAL "3")
    cmake_policy(SET CMP0026 OLD)
endif()

include(cmake/macros.cmake)

#set (CMAKE_SUPPRESS_REGENERATION 1)
# experimental 7zip file support
# the qua7zip library do not support creating of 7zip archives yet,
# which means that the packager is not able to write 7zip archives
# do not enable by default
OPTION(SEVENZIP_UNPACK_SUPPORT "Enable 7-zip unpacking support" OFF)

OPTION(BZIP2_UNPACK_SUPPORT "Enable tar.bz2 unpacking support" ON)

# static qt build is detected automatically
OPTION(STATIC_BUILD "enable static build support" ON)

# this option is intended for standalone shared builds, do not enable by default
OPTION(INSTALL_SYSTEM_LIBS "install required system libraries too (only in shared mode)" OFF)

OPTION(BUILD_TESTS "build test applications " OFF)

# experimental translations support - integration of languages files in kde4-l10n is missing
# see http://websvn.kde.org/trunk/l10n-kde4/templates/messages/qt
OPTION(BUILD_TRANSLATIONS "build translations" OFF)

OPTION(BUILD_WITH_PACKAGER "build with packager" OFF)

OPTION(BUILD_PACKAGER_ONLY "Only build packager" OFF)

OPTION(QT5_BUILD "Build with QT5" OFF)
OPTION(QT4_BUILD "Build with QT4" ON)

if(QT5_BUILD)
    find_package(Qt5Core)
    find_package(Qt5Gui)
    find_package(Qt5Widgets)
    find_package(Qt5Xml)

    if (Qt5Core_FOUND AND Qt5Gui_FOUND AND Qt5Xml_FOUND AND Qt5Widgets_FOUND)
        set(QT5_FOUND 1)
        message(STATUS "Building with Qt5 support")
        set(STATIC_BUILD 0)
        MESSAGE(STATUS "Qt is build in shared mode - use shared build mode too")
        set(QT_CONSOLE_LIBRARIES Qt5::Core Qt5::Xml)
        set(QT_GUI_LIBRARIES Qt5::Core Qt5::Gui Qt5::Xml Qt5::Widgets)
        if(WIN32)
            list(APPEND QT_GUI_LIBRARIES qt5main)
        endif()
        if(UNIX)
            set(QT_DEFINITIONS -fpic)
        endif()
        set(CMAKE_AUTOMOC ON)
        set(QT4_BUILD OFF)
        # QT_INCLUDES is not used; includes are provided by imported targets
    endif()
endif()
if(NOT QT5_BUILD AND QT4_BUILD)
    set(QT_MIN_VERSION "4.7.0")
    find_package(Qt4 COMPONENTS QtCore QtGui QtXml QtNetwork REQUIRED )
    include(${QT_USE_FILE})
    set(QT_GUI_LIBRARIES ${QT_QTGUI_LIBRARY} ${QT_QTNETWORK_LIBRARY} ${QT_QTCORE_LIBRARY} ${QT_QTXML_LIBRARY} ${QT_QTMAIN_LIBRARY})
    set(QT_CONSOLE_LIBRARIES ${QT_QTNETWORK_LIBRARY} ${QT_QTCORE_LIBRARY} ${QT_QTXML_LIBRARY} ${QT_QTMAIN_LIBRARY})
    #message(${QT_INCLUDES})
endif()

find_package(UPX)
find_package(KDEWIN_Packager)
find_package(WinSCP)

ENABLE_TESTING()

include(CTest)
include(CTestConfig.cmake)

if(WIN32)
    ADD_DEFINITIONS(-D_WIN32_WINNT=0x0500)
endif()
# -------------------------------------------------------
# build type settings
# -------------------------------------------------------
if (QT_CONFIG MATCHES ".*shared.*")
    set (STATIC_BUILD 0)
    MESSAGE(STATUS "Qt is build in shared mode - use shared build mode too")
else (QT_CONFIG MATCHES ".*shared.*")
    if(WIN32)
        set (STATIC_BUILD 1)
    endif()
    if (MSVC)
        if (MSVC_VERSION EQUAL 1600)
            FILE(READ "${QT_MKSPECS_DIR}/win32-msvc2010/qmake.conf" _qmake_FILE_contents)
            STRING(REGEX MATCH "QMAKE_CFLAGS_RELEASE[^\n]+" QMAKE_CFLAGS_RELEASE "${_qmake_FILE_contents}")
        ENDIF()
        if (MSVC_VERSION EQUAL 1500)
            FILE(READ "${QT_MKSPECS_DIR}/win32-msvc2008/qmake.conf" _qmake_FILE_contents)
            STRING(REGEX MATCH "QMAKE_CFLAGS_RELEASE[^\n]+" QMAKE_CFLAGS_RELEASE "${_qmake_FILE_contents}")
        ENDIF()
        if (QMAKE_CFLAGS_RELEASE MATCHES ".*-MT.*")
            MESSAGE(STATUS "Qt is build in static mode with static runtime - use static build mode with static runtime too")
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

if (STATIC_BUILD AND CMAKE_COMPILER_IS_GNUCXX)
    set (CMAKE_C_FLAGS "${CMAKE_C_FLAGS} -static, -Wl,-static")
    set (CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -static, -Wl,-static")
endif ()

# where to store executables
if (WIN32 OR CYGWIN)
    set (LIBRARY_OUTPUT_PATH  ${CMAKE_BINARY_DIR}/bin)
else (WIN32 OR CYGWIN)
    set (LIBRARY_OUTPUT_PATH  ${CMAKE_BINARY_DIR}/lib)
endif (WIN32 OR CYGWIN)
set (EXECUTABLE_OUTPUT_PATH ${CMAKE_BINARY_DIR}/bin)
 
# add 3rdparty sources and definitions
add_subdirectory(3rdparty)
# this will define xxx_INCLUDE_DIR, xxx_LIBRARIES and xxx_DEFINITIONS for xxx = quazip, qua7zip, bzip2, curl

include_directories(
  ${CMAKE_SOURCE_DIR}
  ${CMAKE_BINARY_DIR}
  $ENV{QMAKESPEC}
  ${CMAKE_SOURCE_DIR}/shared
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

#-----------------------------------------------------------------------------------------
# set libs required by console and gui apps 
#-----------------------------------------------------------------------------------------
if (UNIX)
  set (CONSOLE_LIBS pthread dl z rt ${QT_CONSOLE_LIBRARIES} shared-console ${CURL_LIBRARIES} ${QUAZIP_LIBRARIES} ${BZIP2_LIBRARIES})
  set (GUI_LIBS pthread dl z rt ${QT_GUI_LIBRARIES} shared-gui ${CURL_LIBRARIES} ${QUAZIP_LIBRARIES} ${BZIP2_LIBRARIES} shared-gui)
  if (STATIC_BUILD)
    find_package(X11)
    find_package(Freetype)
# does not work with cmake provided find_packages
#    find_package(Fontconfig)
#    find_package(png)
     set (GUI_LIBS ${GUI_LIBS} ${X11_LIBRARIES} ${FREETYPE_LIBRARIES} crypto ssl fontconfig png Xrandr Xfixes)
  endif(STATIC_BUILD)

else(UNIX)
  if (MINGW)
    set (FIRST_LIBS mingw32)
  endif(MINGW)
  # required for static linking
  set (CONSOLE_LIBS shared-console ${QUAZIP_LIBRARIES} ${QUA7ZIP_LIBRARIES} ${QT_CONSOLELIBRARIES}
        ${BZIP2_LIBRARIES} ${CURL_LIBRARIES})
  set (GUI_LIBS shared-gui ${QUAZIP_LIBRARIES} ${QUA7ZIP_LIBRARIES} ${QT_GUI_LIBRARIES} ${QT_WIDGETS_LIBRARIES}
        ${BZIP2_LIBRARIES} ${CURL_LIBRARIES})
  if(BZIP2_UNPACK_SUPPORT)
    list(APPEND CONSOLE_LIBS ${FILTERS_LIBRARIES})
    list(APPEND GUI_LIBS ${FILTERS_LIBRARIES})
  endif()
  if(WIN32)
      list(APPEND CONSOLE_LIBS kernel32 user32 shell32 uuid ole32 advapi32 ws2_32 shlwapi psapi)
      list(APPEND GUI_LIBS gdi32 comdlg32 oleaut32 imm32 winmm winspool ws2_32 ole32 uuid user32 advapi32 shlwapi psapi)
  endif(WIN32)
endif (UNIX)

if (STATIC_BUILD)
    find_package(OpenSSL REQUIRED)
    list(APPEND CONSOLE_LIBS ${OPENSSL_LIBRARIES})
    list(APPEND GUI_LIBS ${OPENSSL_LIBRARIES})
    if(WIN32)
        list(APPEND CONSOLE_LIBS Crypt32)
        list(APPEND GUI_LIBS Crypt32)
    endif()
endif ()

set (CONSOLE_DEFINITIONS ${QT_DEFINITIONS} ${CURL_DEFINITIONS} ${BZIP2_DEFINITIONS} ${FILTERS_DEFINITIONS} ${QUAZIP_DEFINITIONS} ${QUA7ZIP_DEFINITIONS})
set (GUI_DEFINITIONS     "-DUSE_GUI ${QT_DEFINITIONS} ${BZIP2_DEFINITIONS} ${CURL_DEFINITIONS} ${FILTERS_DEFINITIONS} ${QUAZIP_DEFINITIONS} ${QUA7ZIP_DEFINITIONS}")

set (CONSOLE_INCLUDE_DIR  ${BZIP2_INCLUDE_DIR} ${CMAKE_SOURCE_DIR}/shared  ${CURL_INCLUDE_DIR} ${FILTERS_INCLUDE_DIR} ${QUAZIP_INCLUDE_DIR} ${QUA7ZIP_INCLUDE_DIR} )
set (GUI_INCLUDE_DIR      ${BZIP2_INCLUDE_DIR} ${CMAKE_SOURCE_DIR}/shared  ${CURL_INCLUDE_DIR} ${FILTERS_INCLUDE_DIR} ${QUAZIP_INCLUDE_DIR} ${QUA7ZIP_INCLUDE_DIR})

if (BUILD_PACKAGER_ONLY)
    add_subdirectory(shared)
    add_subdirectory(sha1sum)
    add_subdirectory(packager)
else(BUILD_PACKAGER_ONLY)

#-----------------------------------------------------------------------------------------
# our targets
#-----------------------------------------------------------------------------------------
add_subdirectory(sha1sum)

add_subdirectory(shared)

if(NOT BUILD_INSTALLER_THEME)
add_subdirectory(console)
endif(NOT BUILD_INSTALLER_THEME)
add_subdirectory(gui)
#add_subdirectory(enduser)
#add_subdirectory(package-manager)
if(NOT BUILD_INSTALLER_THEME)
add_subdirectory(tests)
if(BUILD_WITH_PACKAGER)
	add_subdirectory(packager)
endif(BUILD_WITH_PACKAGER)
add_subdirectory(doc)
endif(NOT BUILD_INSTALLER_THEME)
#add_subdirectory(tools)
add_subdirectory(single-package-installer)


if (WIN32)
    include(Win32Macros)
    addExplorerWrapper(kdewin-installer)
endif(WIN32)

if(NOT BUILD_INSTALLER_THEME)

if(BUILD_WITH_PACKAGER OR BUILD_PACKAGER_ONLY)
    install( FILES cmake/modules/FindKDEWIN_Packager.cmake  DESTINATION  share/apps/cmake/modules )
endif()

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

endif(NOT BUILD_INSTALLER_THEME)
endif(BUILD_PACKAGER_ONLY)