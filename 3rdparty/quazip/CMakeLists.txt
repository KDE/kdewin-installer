
add_definitions(
    ${QT_DEFINITIONS}
)

include_directories(
    ${CMAKE_SOURCE_DIR}/3rdparty/zlib
    ${QT_INCLUDES}
)

set(quazip_SOURCES
    quazip/ioapi.c
    quazip/unzip.c
    quazip/zip.c
    quazip/quazip.cpp
    quazip/quazipfile.cpp
    quazip/quazipnewinfo.cpp
)
set(quazip_HEADERS
    quazip/ioapi.h
    quazip/unzip.h
    quazip/zip.h
    quazip/quazip.h
    quazip/quazipfile.h
    quazip/quazipnewinfo.h
)

add_library(quazip STATIC ${quazip_SOURCES} ${quazip_HEADERS})
target_link_libraries(quazip)
get_target_property(QUAZIP_LIBRARY quazip LOCATION)

# public interface
set(QUAZIP_DEFINITIONS "" CACHE STRING "compiler definitions" FORCE)
set(QUAZIP_INCLUDE_DIR ${CMAKE_SOURCE_DIR}/3rdparty/quazip/quazip ${CMAKE_SOURCE_DIR}/3rdparty/zlib CACHE PATH "include directories" FORCE)
set(QUAZIP_LIBRARIES ${QUAZIP_LIBRARY} CACHE FILEPATH "libraries" FORCE)
set(QUAZIP_FOUND TRUE CACHE BOOL "state" FORCE)
