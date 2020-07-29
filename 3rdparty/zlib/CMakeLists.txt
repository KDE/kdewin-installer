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

target_include_directories(zlib PUBLIC ${CMAKE_SOURCE_DIR}/3rdparty/zlib)
# public interface
set(ZLIB_LIBRARIES zlib CACHE FILEPATH "libraries" FORCE)
