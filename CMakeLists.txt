project(kdewin-installer)
set (VERSION 0.6.4)

# where to look first for cmake modules, before ${CMAKE_ROOT}/Modules/ is checked
set(CMAKE_MODULE_PATH "${CMAKE_SOURCE_DIR}/cmake/modules")

find_package(Qt4 REQUIRED)

if (CMAKE_BUILD_TYPE EQUAL Release) 
  # disable qDebug output
	add_definitions(-DQT_NO_DEBUG_OUTPUT)
endif (CMAKE_BUILD_TYPE EQUAL Release)

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
    ${CMAKE_CURRENT_SOURCE_DIR}/shared/packager.cpp
    ${CMAKE_CURRENT_SOURCE_DIR}/shared/misc.cpp
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
)
set(installer_shared_INCLUDE_DIR ${CMAKE_CURRENT_SOURCE_DIR}/shared)

# we need 'UNICODE'
add_definitions(-DUNICODE)
include_directories(${quazip_INCLUDE_DIR} ${qua7zip_INCLUDE_DIR} ${installer_shared_INCLUDE_DIR})
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
