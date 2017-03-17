include_directories(${CMAKE_CURRENT_SOURCE_DIR})

set(bzip2_SOURCES
    blocksort.c
    bzlib.c
    compress.c
    crctable.c
    decompress.c
    huffman.c
    randtable.c
)
set(bzip2_HEADERS
    bzlib.h
    bzlib_private.h
)

add_library(bzip2 STATIC ${bzip2_SOURCES} ${bzip2_HEADERS})
target_link_libraries(bzip2)
get_target_property(BZIP2_LIBRARY bzip2 LOCATION)

# public interface
set(BZIP2_DEFINITIONS "" CACHE STRING "compiler definitions" FORCE)
set(BZIP2_INCLUDE_DIR ${CMAKE_SOURCE_DIR}/3rdparty/bzip2 CACHE PATH "include directories" FORCE)
set(BZIP2_LIBRARIES ${BZIP2_LIBRARY} CACHE FILEPATH "libraries" FORCE)
set(BZIP2_FOUND TRUE CACHE BOOL "state" FORCE)
