if (BUILD_TESTS)

if(QT5_BUILD)
    find_package(Qt5Test)
    list(APPEND CONSOLE_LIBS Qt5::Test)
else()
    list(APPEND CONSOLE_LIBS ${QT_QTGUI_LIBRARY} ${QT_QTTEST_LIBRARY})
endif()

#
# add_installer_test(testname [additional sources])
#

macro (add_installer_test name)
    set (TEST_NAME ${name})
    set (${TEST_NAME}_SOURCES
        ${TEST_NAME}.cpp
        ${ARGN}
    )
    if(QT4_FOUND)
        qt4_automoc(${${TEST_NAME}_SOURCES})
    endif()
    add_executable(${TEST_NAME} ${${TEST_NAME}_SOURCES})
    target_link_libraries(${TEST_NAME} ${CONSOLE_LIBS})
    add_test(${TEST_NAME} ${CMAKE_CURRENT_BINARY_DIR}/../bin/${TEST_NAME})
endmacro (add_installer_test name)

include_directories(
    ${CMAKE_CURRENT_BINARY_DIR}
    ${CMAKE_SOURCE_DIR}/packager
    ${FILTERS_INCLUDE_DIR}
)

add_installer_test(test-hashfile)
add_installer_test(test-misc)
set(CONSOLE_LIBS ${CONSOLE_LIBS} ${QT_QTXML_LIBRARY})
add_installer_test(test-packager ../packager/packager.cpp ../packager/xmltemplatepackager.cpp)

if(NOT BUILD_PACKAGER_ONLY)
add_installer_test(test-database)
add_installer_test(test-downloader ../shared/downloaderprogress.cpp)
add_installer_test(test-globalconfig)
add_installer_test(test-mirrors)
add_installer_test(test-package)
add_installer_test(test-packagelist-apachemodindex)
add_installer_test(test-packagelist-sourceforge)
add_installer_test(test-packagelist-sourceforge-mirror)
add_installer_test(test-settings)
add_installer_test(test-unpacker)
if(win32)
  add_installer_test(test-startmenu)
endif(win32)
add_installer_test(test-proxy)
endif()

endif (BUILD_TESTS)
