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
target_link_libraries(quazip PRIVATE ${QT_CONSOLE_LIBRARIES} ${ZLIB_LIBRARIES})
target_include_directories(quazip PUBLIC ${CMAKE_SOURCE_DIR}/3rdparty/quazip/quazip)
set(QUAZIP_LIBRARIES quazip CACHE FILEPATH "libraries" FORCE)
