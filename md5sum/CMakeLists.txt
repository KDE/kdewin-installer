
add_executable(md5sum md5sum.cpp ../shared/misc.cpp)
target_link_libraries(md5sum ${QT_QTCORE_LIBRARY} ${QT_QTMAIN_LIBRARY} ${CONSOLE_LIBS})
