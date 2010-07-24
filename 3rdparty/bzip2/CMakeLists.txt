set(bzip2_SOURCES
    ${CMAKE_SOURCE_DIR}/3rdparty/bzip2/blocksort.c
    ${CMAKE_SOURCE_DIR}/3rdparty/bzip2/bzlib.c
    ${CMAKE_SOURCE_DIR}/3rdparty/bzip2/compress.c
    ${CMAKE_SOURCE_DIR}/3rdparty/bzip2/crctable.c
    ${CMAKE_SOURCE_DIR}/3rdparty/bzip2/decompress.c
    ${CMAKE_SOURCE_DIR}/3rdparty/bzip2/huffman.c
    ${CMAKE_SOURCE_DIR}/3rdparty/bzip2/randtable.c
)
set(bzip2_HEADERS
    ${CMAKE_SOURCE_DIR}/3rdparty/bzip2/bzlib.h
    ${CMAKE_SOURCE_DIR}/3rdparty/bzip2/bzlib_private.h
)

add_library(bzip2  ${bzip2_SOURCES} ${bzip2_HEADERS})
target_link_libraries(bzip2)
get_target_property(BZIP2_LIBRARY bzip2 LOCATION)

# public interface
set(BZIP2_DEFINITIONS "-DBZ_IMPORT" CACHE STRING "compiler definitions" FORCE)
set(BZIP2_INCLUDE_DIR ${CMAKE_SOURCE_DIR}/3rdparty/bzip2 CACHE PATH "include directories" FORCE)
set(BZIP2_LIBRARIES ${BZIP2_LIBRARY} CACHE FILEPATH "libraries" FORCE)
set(BZIP2_FOUND TRUE CACHE BOOL "state" FORCE)
