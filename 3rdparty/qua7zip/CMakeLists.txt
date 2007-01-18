project(qua7zip)

set(qua7zip_SRCS
    qua7zip.cpp
    example.cpp
    utils.c
)
set(qua7zip_HDRS
    qua7zip.h
    utils.h
)
set(lzma_SRCS
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

set(lzma_HDRS
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

include(FindQt4)
find_package(Qt4 REQUIRED)

if(CMAKE_COMPILER_2005)
     # to avoid a lot of deprecated warnings
     add_definitions( -D_CRT_SECURE_NO_DEPRECATE -D_CRT_NONSTDC_NO_DEPRECATE )
endif(CMAKE_COMPILER_2005)

source_group(lzma FILES ${lzma_SRCS} ${lzma_HDRS})

include_directories(${QT_INCLUDE_DIR} ${QT_QTCORE_INCLUDE_DIR})
add_executable(example ${qua7zip_SRCS} ${qua7zip_HDRS} ${lzma_SRCS} ${lzma_HDRS})
target_link_libraries(example ${QT_QTCORE_LIBRARY})
