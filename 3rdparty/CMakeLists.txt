#-------------------------------------------------------------------
# bzip2 and filters
#-------------------------------------------------------------------

if (BZIP2_UNPACK_SUPPORT)
    # limit external static bzip2 library usage to shared build mode 
    # in static build mode we cannot rely that the external library is a static build
    #if (NOT STATIC_BUILD)
    #    find_package(bzip2)
    #    set(BZIP2_DEFINITIONS "" CACHE STRING "BZIP compiler definitions" FORCE)
    #else (NOT STATIC_BUILD)
        message(STATUS "using local static bzip2 library")
        add_subdirectory(bzip2)
    #endif (NOT STATIC_BUILD)
    
    add_subdirectory(filters)
endif (BZIP2_UNPACK_SUPPORT)

#-------------------------------------------------------------------
# LIBCURL
#-------------------------------------------------------------------
# limit external curl library usage to shared build mode 
# in static build mode we cannot rely that the external library is a static build
if (NOT STATIC_BUILD)
    find_package(curl)
    if (CURL_FOUND)
        set (CURL_LIBRARIES ${CURL_LIBRARY} CACHE FILEPATH "library" FORCE)
    endif (CURL_FOUND)
endif (NOT STATIC_BUILD)
    
if (NOT CURL_FOUND)
    message(STATUS "using local static curl library")
    add_subdirectory(curl)
endif (NOT CURL_FOUND)

#-------------------------------------------------------------------
# QUAZIP library

#-------------------------------------------------------------------
add_subdirectory(quazip)

#-------------------------------------------------------------------
# 7zip library
#-------------------------------------------------------------------
if(SEVENZIP_UNPACK_SUPPORT)
    add_subdirectory(qua7zip)
endif(SEVENZIP_UNPACK_SUPPORT)
