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
    ${CMAKE_SOURCE_DIR}/3rdparty/curl/lib/http_ntlm.c
    ${CMAKE_SOURCE_DIR}/3rdparty/curl/lib/http_negotiate.c
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
set(CURL_INCLUDE_DIR ${CMAKE_SOURCE_DIR}/3rdparty/curl/include)

include_directories(${CURL_INCLUDE_DIR})
add_definitions(-DWITHOUT_MM_LIB -DCURL_DISABLE_TELNET -DCURL_DISABLE_LDAP)
add_definitions(-DCURL_STATICLIB)
if(QT5_BUILD)
    add_definitions(-Dssize_t=long)
endif()

add_library(curl STATIC ${curl_SOURCES})
set_target_properties(curl PROPERTIES COMPILE_FLAGS "-DBUILDING_LIBCURL -I${CURL_INCLUDE_DIR}/../lib")
target_link_libraries(curl)
get_target_property(CURL_LIBRARY curl LOCATION)

# public interface
set(CURL_DEFINITIONS "-DCURL_STATICLIB" CACHE STRING "compiler definitions" FORCE)
set(CURL_INCLUDE_DIR ${CMAKE_SOURCE_DIR}/3rdparty/curl/include CACHE PATH "include directories" FORCE)
set(CURL_LIBRARIES ${CURL_LIBRARY} CACHE FILEPATH "libraries" FORCE)
set(CURL_FOUND TRUE CACHE BOOL "state" FORCE)
