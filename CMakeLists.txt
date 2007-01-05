project(kdewin-installer)
set (VERSION 0.1)

find_package(Qt4 REQUIRED)

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

# set installer_shared_SOURCES/installer_shared_HEADERS
set(installer_shared_SOURCES
    ${CMAKE_CURRENT_SOURCE_DIR}/shared/downloader.cpp
    ${CMAKE_CURRENT_SOURCE_DIR}/shared/downloaderprogress.cpp
    ${CMAKE_CURRENT_SOURCE_DIR}/shared/installerprogress.cpp
    ${CMAKE_CURRENT_SOURCE_DIR}/shared/installer.cpp
    ${CMAKE_CURRENT_SOURCE_DIR}/shared/package.cpp
    ${CMAKE_CURRENT_SOURCE_DIR}/shared/packagelist.cpp
)

set(installer_shared_HEADERS
    ${CMAKE_CURRENT_SOURCE_DIR}/shared/downloader.h
    ${CMAKE_CURRENT_SOURCE_DIR}/shared/downloaderprogress.h
    ${CMAKE_CURRENT_SOURCE_DIR}/shared/installerprogress.h
    ${CMAKE_CURRENT_SOURCE_DIR}/shared/installer.h
    ${CMAKE_CURRENT_SOURCE_DIR}/shared/package.h
    ${CMAKE_CURRENT_SOURCE_DIR}/shared/packagelist.h
)
set(installer_shared_INCLUDE_DIR ${CMAKE_CURRENT_SOURCE_DIR}/shared)

include_directories(${quazip_INCLUDE_DIR} ${installer_shared_INCLUDE_DIR})

add_definitions(-DINCLUDE_DOC_AND_SRC_PACKAGES)
# our targets
add_subdirectory(console)
add_subdirectory(gui)
add_subdirectory(tests)
