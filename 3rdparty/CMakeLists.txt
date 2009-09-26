#-------------------------------------------------------------------
# BZIP2
#-------------------------------------------------------------------
if (BZIP2_UNPACK_SUPPORT)
    set(filters_INCLUDE_DIR ${CMAKE_SOURCE_DIR}/3rdparty/filters)

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

    set(filters_SOURCES
        ${CMAKE_SOURCE_DIR}/3rdparty/filters/tarfilter.cpp
        ${CMAKE_SOURCE_DIR}/3rdparty/filters/bzip2iodevice.cpp
    )
    set(filters_HEADERS
        ${CMAKE_SOURCE_DIR}/3rdparty/filters/tarfilter.h
        ${CMAKE_SOURCE_DIR}/3rdparty/filters/bzip2iodevice.h
    )
    include_directories(${CMAKE_SOURCE_DIR}/3rdparty/bzip2)
    add_library(filters ${filters_SOURCES} ${filters_HEADERS})
    target_link_libraries(filters)

    # public interface
    set(BZIP2_INCLUDE_DIR ${CMAKE_SOURCE_DIR}/3rdparty/bzip2 ${CMAKE_SOURCE_DIR}/3rdparty/filters)
    set(BZIP2_LIBRARIES filters bzip2)

endif (BZIP2_UNPACK_SUPPORT)


#-------------------------------------------------------------------
# LIBCURL
#-------------------------------------------------------------------
set(curl_SOURCES
    ${CMAKE_SOURCE_DIR}/3rdparty/curl/lib/base64.c
    ${CMAKE_SOURCE_DIR}/3rdparty/curl/lib/connect.c
    ${CMAKE_SOURCE_DIR}/3rdparty/curl/lib/content_encoding.c
    ${CMAKE_SOURCE_DIR}/3rdparty/curl/lib/cookie.c
    ${CMAKE_SOURCE_DIR}/3rdparty/curl/lib/curl_addrinfo.c
    ${CMAKE_SOURCE_DIR}/3rdparty/curl/lib/dict.c
    ${CMAKE_SOURCE_DIR}/3rdparty/curl/lib/easy.c
    ${CMAKE_SOURCE_DIR}/3rdparty/curl/lib/escape.c
    ${CMAKE_SOURCE_DIR}/3rdparty/curl/lib/file.c
    ${CMAKE_SOURCE_DIR}/3rdparty/curl/lib/formdata.c
    ${CMAKE_SOURCE_DIR}/3rdparty/curl/lib/ftp.c
    ${CMAKE_SOURCE_DIR}/3rdparty/curl/lib/getenv.c
    ${CMAKE_SOURCE_DIR}/3rdparty/curl/lib/getinfo.c
    ${CMAKE_SOURCE_DIR}/3rdparty/curl/lib/gtls.c
    ${CMAKE_SOURCE_DIR}/3rdparty/curl/lib/hash.c
    ${CMAKE_SOURCE_DIR}/3rdparty/curl/lib/hostares.c
    ${CMAKE_SOURCE_DIR}/3rdparty/curl/lib/hostasyn.c
    ${CMAKE_SOURCE_DIR}/3rdparty/curl/lib/hostip.c
    ${CMAKE_SOURCE_DIR}/3rdparty/curl/lib/hostip4.c
    ${CMAKE_SOURCE_DIR}/3rdparty/curl/lib/hostip6.c
    ${CMAKE_SOURCE_DIR}/3rdparty/curl/lib/hostsyn.c
    ${CMAKE_SOURCE_DIR}/3rdparty/curl/lib/hostthre.c
    ${CMAKE_SOURCE_DIR}/3rdparty/curl/lib/http.c
    ${CMAKE_SOURCE_DIR}/3rdparty/curl/lib/http_chunks.c
    ${CMAKE_SOURCE_DIR}/3rdparty/curl/lib/http_digest.c
    ${CMAKE_SOURCE_DIR}/3rdparty/curl/lib/http_negotiate.c
    ${CMAKE_SOURCE_DIR}/3rdparty/curl/lib/http_ntlm.c
    ${CMAKE_SOURCE_DIR}/3rdparty/curl/lib/if2ip.c
    ${CMAKE_SOURCE_DIR}/3rdparty/curl/lib/inet_ntop.c
    ${CMAKE_SOURCE_DIR}/3rdparty/curl/lib/inet_pton.c
    ${CMAKE_SOURCE_DIR}/3rdparty/curl/lib/krb4.c
    ${CMAKE_SOURCE_DIR}/3rdparty/curl/lib/krb5.c
    ${CMAKE_SOURCE_DIR}/3rdparty/curl/lib/ldap.c
    ${CMAKE_SOURCE_DIR}/3rdparty/curl/lib/llist.c
    ${CMAKE_SOURCE_DIR}/3rdparty/curl/lib/md5.c
    ${CMAKE_SOURCE_DIR}/3rdparty/curl/lib/memdebug.c
    ${CMAKE_SOURCE_DIR}/3rdparty/curl/lib/mprintf.c
    ${CMAKE_SOURCE_DIR}/3rdparty/curl/lib/multi.c
    ${CMAKE_SOURCE_DIR}/3rdparty/curl/lib/netrc.c
    ${CMAKE_SOURCE_DIR}/3rdparty/curl/lib/nonblock.c
    ${CMAKE_SOURCE_DIR}/3rdparty/curl/lib/nss.c
    ${CMAKE_SOURCE_DIR}/3rdparty/curl/lib/parsedate.c
    ${CMAKE_SOURCE_DIR}/3rdparty/curl/lib/progress.c
    ${CMAKE_SOURCE_DIR}/3rdparty/curl/lib/qssl.c
    ${CMAKE_SOURCE_DIR}/3rdparty/curl/lib/rawstr.c
    ${CMAKE_SOURCE_DIR}/3rdparty/curl/lib/security.c
    ${CMAKE_SOURCE_DIR}/3rdparty/curl/lib/select.c
    ${CMAKE_SOURCE_DIR}/3rdparty/curl/lib/sendf.c
    ${CMAKE_SOURCE_DIR}/3rdparty/curl/lib/share.c
    ${CMAKE_SOURCE_DIR}/3rdparty/curl/lib/slist.c
    ${CMAKE_SOURCE_DIR}/3rdparty/curl/lib/socks.c
    ${CMAKE_SOURCE_DIR}/3rdparty/curl/lib/speedcheck.c
    ${CMAKE_SOURCE_DIR}/3rdparty/curl/lib/splay.c
    ${CMAKE_SOURCE_DIR}/3rdparty/curl/lib/ssh.c
    ${CMAKE_SOURCE_DIR}/3rdparty/curl/lib/sslgen.c
    ${CMAKE_SOURCE_DIR}/3rdparty/curl/lib/ssluse.c
    ${CMAKE_SOURCE_DIR}/3rdparty/curl/lib/strdup.c
    ${CMAKE_SOURCE_DIR}/3rdparty/curl/lib/strequal.c
    ${CMAKE_SOURCE_DIR}/3rdparty/curl/lib/strerror.c
    ${CMAKE_SOURCE_DIR}/3rdparty/curl/lib/strtok.c
    ${CMAKE_SOURCE_DIR}/3rdparty/curl/lib/strtoofft.c
    ${CMAKE_SOURCE_DIR}/3rdparty/curl/lib/telnet.c
    ${CMAKE_SOURCE_DIR}/3rdparty/curl/lib/tftp.c
    ${CMAKE_SOURCE_DIR}/3rdparty/curl/lib/timeval.c
    ${CMAKE_SOURCE_DIR}/3rdparty/curl/lib/transfer.c
    ${CMAKE_SOURCE_DIR}/3rdparty/curl/lib/url.c
    ${CMAKE_SOURCE_DIR}/3rdparty/curl/lib/version.c
)
set(curl_HEADERS
    ${CMAKE_SOURCE_DIR}/3rdparty/curl/include/curl/curl.h
    ${CMAKE_SOURCE_DIR}/3rdparty/curl/include/curl/curlver.h
    ${CMAKE_SOURCE_DIR}/3rdparty/curl/include/curl/easy.h
    ${CMAKE_SOURCE_DIR}/3rdparty/curl/include/curl/mprintf.h
    ${CMAKE_SOURCE_DIR}/3rdparty/curl/include/curl/multi.h
    ${CMAKE_SOURCE_DIR}/3rdparty/curl/include/curl/stdcheaders.h
    ${CMAKE_SOURCE_DIR}/3rdparty/curl/include/curl/types.h
)
# public interface
set(CURL_INCLUDE_DIR ${CMAKE_SOURCE_DIR}/3rdparty/curl/include)

find_package(CURL)
if(WIN32)
  include_directories(${CURL_INCLUDE_DIR})
  add_definitions(-DCURL_STATICLIB -DWITHOUT_MM_LIB -DCURL_DISABLE_TELNET -DCURL_DISABLE_LDAP)
  add_library(curl STATIC ${curl_SOURCES} ${curl_HEADERS})
  if(MINGW)
    target_link_libraries( curl ws2_32 )
  endif(MINGW)
  set_target_properties(curl PROPERTIES COMPILE_FLAGS "-DBUILDING_LIBCURL -I${CURL_INCLUDE_DIR}/../lib")

  set (CURL_LIBRARIES curl Wldap32)
else(WIN32)
  if (CURL_FOUND)
      set (CURL_LIBRARIES ${CURL_LIBRARY})
      include_directories( ${CURL_INCLUDE_DIR})
  else (CURL_FOUND)
    include_directories(${CURL_INCLUDE_DIR})
    add_definitions(-DCURL_STATICLIB -DWITHOUT_MM_LIB -DCURL_DISABLE_TELNET -DCURL_DISABLE_LDAP)
    add_library(curl STATIC ${curl_SOURCES} ${curl_HEADERS})
    set_target_properties(curl PROPERTIES COMPILE_FLAGS "-DBUILDING_LIBCURL -I${CURL_INCLUDE_DIR}/../lib")
    set (CURL_LIBRARIES curl)
  endif (CURL_FOUND) 
endif(WIN32)

#-------------------------------------------------------------------
# QUAZIP library
#-------------------------------------------------------------------
set(quazip_SOURCES
    ${CMAKE_SOURCE_DIR}/3rdparty/quazip/quazip/ioapi.c
    ${CMAKE_SOURCE_DIR}/3rdparty/quazip/quazip/unzip.c
    ${CMAKE_SOURCE_DIR}/3rdparty/quazip/quazip/zip.c
    ${CMAKE_SOURCE_DIR}/3rdparty/quazip/quazip/quazip.cpp
    ${CMAKE_SOURCE_DIR}/3rdparty/quazip/quazip/quazipfile.cpp
    ${CMAKE_SOURCE_DIR}/3rdparty/quazip/quazip/quazipnewinfo.cpp
)
set(quazip_HEADERS
    ${CMAKE_SOURCE_DIR}/3rdparty/quazip/quazip/ioapi.h
    ${CMAKE_SOURCE_DIR}/3rdparty/quazip/quazip/unzip.h
    ${CMAKE_SOURCE_DIR}/3rdparty/quazip/quazip/zip.h
    ${CMAKE_SOURCE_DIR}/3rdparty/quazip/quazip/quazip.h
    ${CMAKE_SOURCE_DIR}/3rdparty/quazip/quazip/quazipfile.h
    ${CMAKE_SOURCE_DIR}/3rdparty/quazip/quazip/quazipnewinfo.h
)
add_library(quazip  ${quazip_SOURCES} ${quazip_HEADERS})
target_link_libraries(quazip)

# public interface
set(QUAZIP_INCLUDE_DIR ${CMAKE_SOURCE_DIR}/3rdparty/quazip/quazip)
set(QUAZIP_LIBRARIES quazip)

#-------------------------------------------------------------------
# 7zip library
#-------------------------------------------------------------------
if(SEVENZIP_UNPACK_SUPPORT)
  set(qua7zip_SOURCES
    ${CMAKE_SOURCE_DIR}/3rdparty/qua7zip/qua7zip.cpp
    ${CMAKE_SOURCE_DIR}/3rdparty/qua7zip/utils.c
    ${CMAKE_SOURCE_DIR}/3rdparty/qua7zip/lzma_sdk/7zip/Archive/7z_C/7zAlloc.c
    ${CMAKE_SOURCE_DIR}/3rdparty/qua7zip/lzma_sdk/7zip/Archive/7z_C/7zBuffer.c
    ${CMAKE_SOURCE_DIR}/3rdparty/qua7zip/lzma_sdk/7zip/Archive/7z_C/7zCrc.c
    ${CMAKE_SOURCE_DIR}/3rdparty/qua7zip/lzma_sdk/7zip/Archive/7z_C/7zDecode.c
    ${CMAKE_SOURCE_DIR}/3rdparty/qua7zip/lzma_sdk/7zip/Archive/7z_C/7zExtract.c
    ${CMAKE_SOURCE_DIR}/3rdparty/qua7zip/lzma_sdk/7zip/Archive/7z_C/7zHeader.c
    ${CMAKE_SOURCE_DIR}/3rdparty/qua7zip/lzma_sdk/7zip/Archive/7z_C/7zIn.c
    ${CMAKE_SOURCE_DIR}/3rdparty/qua7zip/lzma_sdk/7zip/Archive/7z_C/7zItem.c
    ${CMAKE_SOURCE_DIR}/3rdparty/qua7zip/lzma_sdk/7zip/Archive/7z_C/7zMethodID.c
    ${CMAKE_SOURCE_DIR}/3rdparty/qua7zip/lzma_sdk/7zip/Compress/LZMA_C/LzmaDecode.c
    )

  set(qua7zip_HEADERS
    ${CMAKE_SOURCE_DIR}/3rdparty/qua7zip/qua7zip.h
    ${CMAKE_SOURCE_DIR}/3rdparty/qua7zip/utils.h
    ${CMAKE_SOURCE_DIR}/3rdparty/qua7zip/lzma_sdk/7zip/Archive/7z_C/7zAlloc.h
    ${CMAKE_SOURCE_DIR}/3rdparty/qua7zip/lzma_sdk/7zip/Archive/7z_C/7zBuffer.h
    ${CMAKE_SOURCE_DIR}/3rdparty/qua7zip/lzma_sdk/7zip/Archive/7z_C/7zCrc.h
    ${CMAKE_SOURCE_DIR}/3rdparty/qua7zip/lzma_sdk/7zip/Archive/7z_C/7zDecode.h
    ${CMAKE_SOURCE_DIR}/3rdparty/qua7zip/lzma_sdk/7zip/Archive/7z_C/7zExtract.h
    ${CMAKE_SOURCE_DIR}/3rdparty/qua7zip/lzma_sdk/7zip/Archive/7z_C/7zHeader.h
    ${CMAKE_SOURCE_DIR}/3rdparty/qua7zip/lzma_sdk/7zip/Archive/7z_C/7zIn.h
    ${CMAKE_SOURCE_DIR}/3rdparty/qua7zip/lzma_sdk/7zip/Archive/7z_C/7zItem.h
    ${CMAKE_SOURCE_DIR}/3rdparty/qua7zip/lzma_sdk/7zip/Archive/7z_C/7zMethodID.h
    ${CMAKE_SOURCE_DIR}/3rdparty/qua7zip/lzma_sdk/7zip/Compress/LZMA_C/LzmaDecode.h
    )

  add_library(qua7zip  ${qua7zip_SOURCES} ${qua7zip_HEADERS})
  target_link_libraries(qua7zip)

  # public interface
  set(QUA7ZIP_INCLUDE_DIR ${CMAKE_SOURCE_DIR}/3rdparty/qua7zip)
  set(QUA7ZIP_LIBRARIES quaz7ip)
  
  endif(SEVENZIP_UNPACK_SUPPORT)

include_directories(
  ${CMAKE_SOURCE_DIR}/3rdparty/zlib
  ${CURL_INCLUDE_DIR}
  ${BZIP2_INCLUDE_DIR}
  ${FILTERS_INCLUDE_DIR}
  ${QUAZIP_INCLUDE_DIR}
  ${QUA7ZIP_INCLUDE_DIR}
)
