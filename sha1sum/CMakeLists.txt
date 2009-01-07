
add_definitions(-DMISC_SMALL_VERSION)
add_executable(sha1sum 
    sha1sum.cpp 
    ../shared/hash.cpp 
    ../shared/debug.cpp
)
target_link_libraries(sha1sum ${QT_QTCORE_LIBRARY} ${QT_QTMAIN_LIBRARY})
if (MSVC)
    target_link_libraries(sha1sum ws2_32)
endif (MSVC)

