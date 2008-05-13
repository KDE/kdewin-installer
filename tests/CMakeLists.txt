if (BUILD_TESTS)

#
# add_test(testname [additional sources])
#
macro (add_test name)
    set (TEST_NAME ${name})
    set (${TEST_NAME}_SOURCES
        ${TEST_NAME}.cpp
        ${ARGN}
    )
    qt4_automoc(${${TEST_NAME}_SOURCES})
    add_executable(${TEST_NAME} ${${TEST_NAME}_SOURCES})
    target_link_libraries(${TEST_NAME} ${QT_QTMAIN_LIBRARY} ${QT_QTNETWORK_LIBRARY} ${QT_QTCORE_LIBRARY} ${LIBS})
endmacro (add_test)

include_directories(${CMAKE_CURRENT_BINARY_DIR} ${CMAKE_SOURCE_DIR}/packager)

source_group(quazip  FILES ${quazip_SOURCES}  ${quazip_HEADERS})
source_group(qua7zip FILES ${qua7zip_SOURCES} ${qua7zip_HEADERS})

set (SOURCES 
    ${installer_shared_SOURCES}
    ${quazip_HEADERS}
    ${quazip_SOURCES}
    ${qua7zip_SOURCES}
    ${qua7zip_HEADERS}
    ${md5_SOURCES}
    ${md5_HEADERS}
    ${bzip2_SOURCES}
    ${bzip2_HEADERS}
    ${filters_SOURCES}
    ${filters_HEADERS}
)

qt4_automoc(${SOURCES})
add_library(mylib ${SOURCES})
target_link_libraries(mylib)

list(APPEND LIBS mylib)

add_test(test-database)
add_test(test-downloader)
add_test(test-globalconfig)
add_test(test-mirrors)
add_test(test-package)
add_test(test-packagelist-apachemodindex)
add_test(test-packagelist-sourceforge)
add_test(test-packagelist-sourceforge-mirror)
add_test(test-packager ../packager/packager.cpp)
add_test(test-settings)
add_test(test-startmenu)
add_test(test-proxy)

endif (BUILD_TESTS)
