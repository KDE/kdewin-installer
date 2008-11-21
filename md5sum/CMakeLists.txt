
add_definitions(-DMISC_SMALL_VERSION)
add_executable(md5sum md5sum.cpp ../shared/misc.cpp)
target_link_libraries(md5sum ${QT_QTCORE_LIBRARY} ${QT_QTMAIN_LIBRARY})
if (MSVC)
    target_link_libraries(md5sum ws2_32)
endif (MSVC)

