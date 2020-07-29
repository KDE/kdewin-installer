set(qua7zip_SOURCES
    qua7zip.cpp
    utils.c
    lzma_sdk/7zip/Archive/7z_C/7zAlloc.c
    lzma_sdk/7zip/Archive/7z_C/7zBuffer.c
    lzma_sdk/7zip/Archive/7z_C/7zCrc.c
    lzma_sdk/7zip/Archive/7z_C/7zDecode.c
    lzma_sdk/7zip/Archive/7z_C/7zExtract.c
    lzma_sdk/7zip/Archive/7z_C/7zHeader.c
    lzma_sdk/7zip/Archive/7z_C/7zIn.c
    lzma_sdk/7zip/Archive/7z_C/7zItem.c
    lzma_sdk/7zip/Archive/7z_C/7zMethodID.c
    lzma_sdk/7zip/Compress/LZMA_C/LzmaDecode.c
)

set(qua7zip_HEADERS
    qua7zip.h
    utils.h
    lzma_sdk/7zip/Archive/7z_C/7zAlloc.h
    lzma_sdk/7zip/Archive/7z_C/7zBuffer.h
    lzma_sdk/7zip/Archive/7z_C/7zCrc.h
    lzma_sdk/7zip/Archive/7z_C/7zDecode.h
    lzma_sdk/7zip/Archive/7z_C/7zExtract.h
    lzma_sdk/7zip/Archive/7z_C/7zHeader.h
    lzma_sdk/7zip/Archive/7z_C/7zIn.h
    lzma_sdk/7zip/Archive/7z_C/7zItem.h
    lzma_sdk/7zip/Archive/7z_C/7zMethodID.h
    lzma_sdk/7zip/Compress/LZMA_C/LzmaDecode.h
)

add_library(qua7zip STATIC ${qua7zip_SOURCES} ${qua7zip_HEADERS})
target_link_libraries(qua7zip PRIVATE ${QT_CONSOLE_LIBRARIES})
target_include_directories(qua7zip PUBLIC ${CMAKE_SOURCE_DIR}/3rdparty/qua7zip)
# public interface
set(QUA7ZIP_LIBRARIES qua7zip CACHE FILEPATH "libraries" FORCE)
