add_definitions(
    -DMISC_SMALL_VERSION
    ${QT_DEFINITIONS}
)

include_directories(
    ${QT_INCLUDES}
    ../shared
)

add_executable(sha1sum 
    sha1sum.cpp 
    ../shared/hash.cpp 
    ../shared/debug.cpp
)
target_link_libraries(sha1sum ${QT_QTCORE_LIBRARY} ${QT_QTMAIN_LIBRARY})
if (MSVC)
    target_link_libraries(sha1sum ws2_32)
endif (MSVC)

get_target_property(SHA1SUM_EXECUTABLE sha1sum LOCATION)
if (CMAKE_CROSSCOMPILING)
    set(SHA1SUM_EXECUTABLE wine z:${SHA1SUM_EXECUTABLE} CACHE FILEPATH "executable" FORCE)
else ()
    set(SHA1SUM_EXECUTABLE ${SHA1SUM_EXECUTABLE} CACHE FILEPATH "executable" FORCE)
endif ()
