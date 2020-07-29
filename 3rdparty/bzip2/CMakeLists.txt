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
target_include_directories(bzip2 PUBLIC ${CMAKE_SOURCE_DIR}/3rdparty/bzip2)

# public interface
set(BZIP2_LIBRARIES bzip2 CACHE FILEPATH "libraries" FORCE)
