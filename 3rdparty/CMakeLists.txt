#-------------------------------------------------------------------
# ZLIB
#-------------------------------------------------------------------
# limit external zlib library usage to shared build mode
# in static build mode we cannot rely that the external library is a static build
if (NOT STATIC_BUILD)
    find_package(ZLIB)
    if (ZLIB_FOUND)
        set (ZLIB_LIBRARIES ${ZLIB_LIBRARY} CACHE FILEPATH "library" FORCE)
    endif ()
endif ()
if (STATIC_BUILD OR NOT ZLIB_FOUND)
    message(STATUS "using local static zlib library")
    add_subdirectory(zlib)
endif ()

#-------------------------------------------------------------------
# bzip2 and filters
#-------------------------------------------------------------------

if (BZIP2_UNPACK_SUPPORT)
    # limit external static bzip2 library usage to shared build mode 
    # in static build mode we cannot rely that the external library is a static build
    if (NOT STATIC_BUILD)
        find_package(BZip2)
        set(BZIP2_DEFINITIONS "" CACHE STRING "BZIP compiler definitions" FORCE)
    endif ()
    if (STATIC_BUILD OR NOT BZIP2_FOUND)
        message(STATUS "using local static bzip2 library")
        add_subdirectory(bzip2)
    endif ()
    add_subdirectory(filters)
endif (BZIP2_UNPACK_SUPPORT)

#-------------------------------------------------------------------
# LIBCURL
#-------------------------------------------------------------------
# limit external curl library usage to shared build mode 
# in static build mode we cannot rely that the external library is a static build
if (NOT STATIC_BUILD)
    find_package(CURL)
    if (CURL_FOUND)
        set (CURL_LIBRARIES ${CURL_LIBRARY} CACHE FILEPATH "library" FORCE)
    endif ()
endif ()
if (STATIC_BUILD OR NOT CURL_FOUND)
    message(STATUS "using local static curl library")
    add_subdirectory(curl)
endif ()

#-------------------------------------------------------------------
# QUAZIP library

#-------------------------------------------------------------------
# limit external quazip library usage to shared build mode 
# in static build mode we cannot rely that the external library is a static build
if (NOT STATIC_BUILD)
    find_package(quazip)
endif ()
if (STATIC_BUILD OR NOT QUAZIP_FOUND)
    message(STATUS "using local static quazip library")
    add_subdirectory(quazip)
endif ()

#-------------------------------------------------------------------
# 7zip library
#-------------------------------------------------------------------
if(SEVENZIP_UNPACK_SUPPORT)
    add_subdirectory(qua7zip)
endif(SEVENZIP_UNPACK_SUPPORT)
