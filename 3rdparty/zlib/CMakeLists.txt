add_library(zlib STATIC
    adler32.c
    compress.c
    crc32.c
    deflate.c
    gzio.c
    infback.c
    inffast.c
    inflate.c
    inftrees.c
    trees.c
    uncompr.c
    zutil.c
)

get_target_property(ZLIB_LIBRARY zlib LOCATION)

# public interface
set(ZLIB_DEFINITIONS ""  CACHE STRING "definitions" FORCE)
set(ZLIB_INCLUDE_DIR ${CMAKE_SOURCE_DIR}/3rdparty/zlib CACHE FILEPATH "include directories" FORCE)
set(ZLIB_LIBRARIES ${ZLIB_LIBRARY} CACHE FILEPATH "libraries" FORCE)
set(ZLIB_FOUND TRUE CACHE BOOL "state" FORCE)
