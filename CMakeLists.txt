project(kdewin-installer)
set (VERSION 0.6.7)

# where to look first for cmake modules, before ${CMAKE_ROOT}/Modules/ is checked
set(CMAKE_MODULE_PATH "${CMAKE_SOURCE_DIR}/cmake/modules")

find_package(Qt4 REQUIRED)
find_package(UPX)


OPTION(SEVENZIP_UNPACK_SUPPORT "Enable 7-zip unpacking support" OFF)

# -------------------------------------------------------
# build type settings 
# -------------------------------------------------------
# Set a default build type for single-configuration
# CMake generators if no build type is set.
if (NOT CMAKE_BUILD_TYPE)
   set(CMAKE_BUILD_TYPE Release)
endif (NOT CMAKE_BUILD_TYPE)

MESSAGE(STATUS "Build Type:" ${CMAKE_BUILD_TYPE})

set (CMAKE_PRODUCTION_BUILD 0)

if (CMAKE_BUILD_TYPE STREQUAL Release OR CMAKE_BUILD_TYPE STREQUAL MinSizeRel) 
    set (CMAKE_PRODUCTION_BUILD 1)
	# disable qDebug output
	add_definitions(-DQT_NO_DEBUG_OUTPUT)
endif (CMAKE_BUILD_TYPE STREQUAL Release OR CMAKE_BUILD_TYPE STREQUAL MinSizeRel) 

if (MSVC)
	if (CMAKE_BUILD_TYPE STREQUAL Release)
		set (CMAKE_C_FLAGS_RELEASE "/MT /O2 /Ob2 /D NDEBUG")
		set (CMAKE_CXX_FLAGS_RELEASE "/MT /O2 /Ob2 /D NDEBUG")
		set (CMAKE_EXE_LINKER_FLAGS_RELEASE "/INCREMENTAL:NO /NODEFAULTLIB:msvcrt")
	endif (CMAKE_BUILD_TYPE STREQUAL Release)
	if (CMAKE_BUILD_TYPE STREQUAL MinSizeRel)
		set (CMAKE_C_FLAGS_MINSIZEREL "/MT /O1 /Ob1 /D NDEBUG")
		set (CMAKE_CXX_FLAGS_MINSIZEREL "/MT /O1 /Ob1 /D NDEBUG")
		set (CMAKE_EXE_LINKER_FLAGS_MINSIZEREL "/INCREMENTAL:NO /NODEFAULTLIB:msvcrt")
	endif (CMAKE_BUILD_TYPE STREQUAL MinSizeRel)
endif (MSVC)

# we just need zlib include dir, linking to zlib is not needed
include_directories(${CMAKE_SOURCE_DIR} ${CMAKE_BINARY_DIR} ${QT_INCLUDES} ${CMAKE_SOURCE_DIR}/3rdparty/zlib)

# get quazip_SOURCES/quazip_HEADERS/quazip_INCLUDE_DIR
set(quazip_SOURCES
    ${CMAKE_CURRENT_SOURCE_DIR}/3rdparty/quazip/quazip/ioapi.c
    ${CMAKE_CURRENT_SOURCE_DIR}/3rdparty/quazip/quazip/unzip.c
    ${CMAKE_CURRENT_SOURCE_DIR}/3rdparty/quazip/quazip/zip.c
    ${CMAKE_CURRENT_SOURCE_DIR}/3rdparty/quazip/quazip/quazip.cpp
    ${CMAKE_CURRENT_SOURCE_DIR}/3rdparty/quazip/quazip/quazipfile.cpp
    ${CMAKE_CURRENT_SOURCE_DIR}/3rdparty/quazip/quazip/quazipnewinfo.cpp
)

set(quazip_HEADERS
    ${CMAKE_CURRENT_SOURCE_DIR}/3rdparty/quazip/quazip/ioapi.h
    ${CMAKE_CURRENT_SOURCE_DIR}/3rdparty/quazip/quazip/unzip.h
    ${CMAKE_CURRENT_SOURCE_DIR}/3rdparty/quazip/quazip/zip.h
    ${CMAKE_CURRENT_SOURCE_DIR}/3rdparty/quazip/quazip/quazip.h
    ${CMAKE_CURRENT_SOURCE_DIR}/3rdparty/quazip/quazip/quazipfile.h
    ${CMAKE_CURRENT_SOURCE_DIR}/3rdparty/quazip/quazip/quazipnewinfo.h
)
set(quazip_INCLUDE_DIR ${CMAKE_CURRENT_SOURCE_DIR}/3rdparty/quazip/quazip)

if(SEVENZIP_UNPACK_SUPPORT)
  set(qua7zip_SOURCES
    ${CMAKE_CURRENT_SOURCE_DIR}/3rdparty/qua7zip/qua7zip.cpp
    ${CMAKE_CURRENT_SOURCE_DIR}/3rdparty/qua7zip/utils.c
    ${CMAKE_CURRENT_SOURCE_DIR}/3rdparty/qua7zip/lzma_sdk/7zip/Archive/7z_C/7zAlloc.c
    ${CMAKE_CURRENT_SOURCE_DIR}/3rdparty/qua7zip/lzma_sdk/7zip/Archive/7z_C/7zBuffer.c
    ${CMAKE_CURRENT_SOURCE_DIR}/3rdparty/qua7zip/lzma_sdk/7zip/Archive/7z_C/7zCrc.c
    ${CMAKE_CURRENT_SOURCE_DIR}/3rdparty/qua7zip/lzma_sdk/7zip/Archive/7z_C/7zDecode.c
    ${CMAKE_CURRENT_SOURCE_DIR}/3rdparty/qua7zip/lzma_sdk/7zip/Archive/7z_C/7zExtract.c
    ${CMAKE_CURRENT_SOURCE_DIR}/3rdparty/qua7zip/lzma_sdk/7zip/Archive/7z_C/7zHeader.c
    ${CMAKE_CURRENT_SOURCE_DIR}/3rdparty/qua7zip/lzma_sdk/7zip/Archive/7z_C/7zIn.c
    ${CMAKE_CURRENT_SOURCE_DIR}/3rdparty/qua7zip/lzma_sdk/7zip/Archive/7z_C/7zItem.c
    ${CMAKE_CURRENT_SOURCE_DIR}/3rdparty/qua7zip/lzma_sdk/7zip/Archive/7z_C/7zMethodID.c
    ${CMAKE_CURRENT_SOURCE_DIR}/3rdparty/qua7zip/lzma_sdk/7zip/Compress/LZMA_C/LzmaDecode.c
    )

  set(qua7zip_HEADERS
    ${CMAKE_CURRENT_SOURCE_DIR}/3rdparty/qua7zip/qua7zip.h
    ${CMAKE_CURRENT_SOURCE_DIR}/3rdparty/qua7zip/utils.h
    ${CMAKE_CURRENT_SOURCE_DIR}/3rdparty/qua7zip/lzma_sdk/7zip/Archive/7z_C/7zAlloc.h
    ${CMAKE_CURRENT_SOURCE_DIR}/3rdparty/qua7zip/lzma_sdk/7zip/Archive/7z_C/7zBuffer.h
    ${CMAKE_CURRENT_SOURCE_DIR}/3rdparty/qua7zip/lzma_sdk/7zip/Archive/7z_C/7zCrc.h
    ${CMAKE_CURRENT_SOURCE_DIR}/3rdparty/qua7zip/lzma_sdk/7zip/Archive/7z_C/7zDecode.h
    ${CMAKE_CURRENT_SOURCE_DIR}/3rdparty/qua7zip/lzma_sdk/7zip/Archive/7z_C/7zExtract.h
    ${CMAKE_CURRENT_SOURCE_DIR}/3rdparty/qua7zip/lzma_sdk/7zip/Archive/7z_C/7zHeader.h
    ${CMAKE_CURRENT_SOURCE_DIR}/3rdparty/qua7zip/lzma_sdk/7zip/Archive/7z_C/7zIn.h
    ${CMAKE_CURRENT_SOURCE_DIR}/3rdparty/qua7zip/lzma_sdk/7zip/Archive/7z_C/7zItem.h
    ${CMAKE_CURRENT_SOURCE_DIR}/3rdparty/qua7zip/lzma_sdk/7zip/Archive/7z_C/7zMethodID.h
    ${CMAKE_CURRENT_SOURCE_DIR}/3rdparty/qua7zip/lzma_sdk/7zip/Compress/LZMA_C/LzmaDecode.h
    )

  set(qua7zip_INCLUDE_DIR ${CMAKE_CURRENT_SOURCE_DIR}/3rdparty/qua7zip)
endif(SEVENZIP_UNPACK_SUPPORT)

set(md5_SOURCES
    ${CMAKE_CURRENT_SOURCE_DIR}/3rdparty/md5/md5.cpp
	)

set(md5_HEADERS
    ${CMAKE_CURRENT_SOURCE_DIR}/3rdparty/md5/md5.h
	)
set(md5_INCLUDE_DIR ${CMAKE_CURRENT_SOURCE_DIR}/3rdparty/md5)

# set installer_shared_SOURCES/installer_shared_HEADERS
set(installer_shared_SOURCES
    ${CMAKE_CURRENT_SOURCE_DIR}/shared/downloader.cpp
    ${CMAKE_CURRENT_SOURCE_DIR}/shared/downloaderprogress.cpp
    ${CMAKE_CURRENT_SOURCE_DIR}/shared/installerprogress.cpp
    ${CMAKE_CURRENT_SOURCE_DIR}/shared/installer.cpp
    ${CMAKE_CURRENT_SOURCE_DIR}/shared/package.cpp
    ${CMAKE_CURRENT_SOURCE_DIR}/shared/packagelist.cpp
    ${CMAKE_CURRENT_SOURCE_DIR}/shared/site.cpp
    ${CMAKE_CURRENT_SOURCE_DIR}/shared/mirrors.cpp
    ${CMAKE_CURRENT_SOURCE_DIR}/shared/globalconfig.cpp
    ${CMAKE_CURRENT_SOURCE_DIR}/shared/settings.cpp
    ${CMAKE_CURRENT_SOURCE_DIR}/shared/installerengine.cpp
    ${CMAKE_CURRENT_SOURCE_DIR}/shared/misc.cpp
    ${CMAKE_CURRENT_SOURCE_DIR}/shared/database.cpp
    ${CMAKE_CURRENT_SOURCE_DIR}/shared/uninstall.cpp
)

set(installer_shared_HEADERS
    ${CMAKE_CURRENT_SOURCE_DIR}/shared/downloader.h
    ${CMAKE_CURRENT_SOURCE_DIR}/shared/downloaderprogress.h
    ${CMAKE_CURRENT_SOURCE_DIR}/shared/installerprogress.h
    ${CMAKE_CURRENT_SOURCE_DIR}/shared/installer.h
    ${CMAKE_CURRENT_SOURCE_DIR}/shared/package.h
    ${CMAKE_CURRENT_SOURCE_DIR}/shared/packagelist.h
    ${CMAKE_CURRENT_SOURCE_DIR}/shared/site.h
    ${CMAKE_CURRENT_SOURCE_DIR}/shared/settings.h
    ${CMAKE_CURRENT_SOURCE_DIR}/shared/installerengine.h
    ${CMAKE_CURRENT_SOURCE_DIR}/shared/misc.h
    ${CMAKE_CURRENT_SOURCE_DIR}/shared/globalconfig.h
    ${CMAKE_CURRENT_SOURCE_DIR}/shared/database.h
    ${CMAKE_CURRENT_SOURCE_DIR}/shared/uninstall.h
)
set(installer_shared_INCLUDE_DIR ${CMAKE_CURRENT_SOURCE_DIR}/shared)

# we need 'UNICODE'
add_definitions(-DUNICODE)
include_directories(
	${quazip_INCLUDE_DIR}
	${qua7zip_INCLUDE_DIR}
	${md5_INCLUDE_DIR}
	${installer_shared_INCLUDE_DIR}
)
configure_file(${CMAKE_SOURCE_DIR}/config.txt ${CMAKE_BINARY_DIR}/config.txt)
configure_file(${CMAKE_SOURCE_DIR}/config.h.cmake ${CMAKE_BINARY_DIR}/config.h)

if (UNIX)
  find_package(X11)
  find_package(Freetype)
  set (LIBS pthread dl z)
  set (GUI_LIBS pthread dl z)
else(UNIX)
  if (MINGW)
    set (FIRST_LIBS mingw32)
  endif(MINGW)
  # required for static linking
  set (LIBS kernel32 user32 shell32 uuid ole32 advapi32 ws2_32)
  set (GUI_LIBS gdi32 comdlg32 oleaut32 imm32 winmm winspool ws2_32 ole32 uuid user32 advapi32)
endif (UNIX)


# our targets
add_subdirectory(console)
add_subdirectory(gui)
add_subdirectory(tests)
add_subdirectory(packager)
add_subdirectory(doc)

if (MSVC)
    configure_file(${CMAKE_SOURCE_DIR}/kdewin-installer-sln.bat.cmake ${CMAKE_BINARY_DIR}/kdewin-installer-sln.bat)
endif (MSVC)

install( FILES cmake/modules/FINDKDEWIN_INSTALLER.cmake  DESTINATION  share/apps/cmake/modules )

