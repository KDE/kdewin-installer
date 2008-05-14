if (BUILD_TESTS)

#
# add_installer_test(testname [additional sources])
#

macro (add_installer_test name)
    set (TEST_NAME ${name})
    set (${TEST_NAME}_SOURCES
        ${TEST_NAME}.cpp
        ${ARGN}
    )
    qt4_automoc(${${TEST_NAME}_SOURCES})
    add_executable(${TEST_NAME} ${${TEST_NAME}_SOURCES})
    target_link_libraries(${TEST_NAME} ${LIBS})
    add_test(${TEST_NAME} ${CMAKE_CURRENT_BINARY_DIR}/../bin/${TEST_NAME})
endmacro (add_installer_test name)

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

list(APPEND LIBS mylib ${CURL_LIBRARIES} ${QT_QTMAIN_LIBRARY} ${QT_QTNETWORK_LIBRARY} ${QT_QTCORE_LIBRARY} ${QT_QTGUI_LIBRARY} ole32 uuid)
if(MINGW)
    list(APPEND LIBS ws2_32 )
endif(MINGW)

add_installer_test(test-database)
add_installer_test(test-downloader)
add_installer_test(test-globalconfig)
add_installer_test(test-mirrors)
add_installer_test(test-package)
add_installer_test(test-packagelist-apachemodindex)
add_installer_test(test-packagelist-sourceforge)
add_installer_test(test-packagelist-sourceforge-mirror)
add_installer_test(test-packager ../packager/packager.cpp)
add_installer_test(test-settings)
add_installer_test(test-startmenu)
add_installer_test(test-proxy)

endif (BUILD_TESTS)
