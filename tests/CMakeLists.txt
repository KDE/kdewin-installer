# our targets
include_directories(${CMAKE_CURRENT_BINARY_DIR})

source_group(quazip  FILES ${quazip_SOURCES}  ${quazip_HEADERS})
source_group(qua7zip FILES ${qua7zip_SOURCES} ${qua7zip_HEADERS})

set (test_mirrors_SOURCES 
	test-mirrors.cpp 
	../shared/mirrors.cpp
	../shared/settings.cpp
	../shared/downloader.cpp
	../shared/downloaderprogress.cpp
	../shared/misc.cpp
)

qt4_automoc(${test_mirrors_SOURCES})

add_executable(test-mirrors ${test_mirrors_SOURCES})
target_link_libraries(test-mirrors ${QT_QTMAIN_LIBRARIES} ${QT_QTNETWORK_LIBRARY} ${QT_QTCORE_LIBRARY} ${LIBS})


set (test-packagelist-apachemodindex_SOURCES
	test-packagelist-apachemodindex.cpp
	${installer_shared_SOURCES}
        ${quazip_HEADERS}
        ${quazip_SOURCES}
        ${qua7zip_SOURCES}
        ${qua7zip_HEADERS}
        ${md5_SOURCES}
		${md5_HEADERS}
)

qt4_automoc(${test-packagelist-apachemodindex_SOURCES})

add_executable(test-packagelist-apachemodindex ${test-packagelist-apachemodindex_SOURCES})
target_link_libraries(test-packagelist-apachemodindex ${QT_QTMAIN_LIBRARIES} ${QT_QTNETWORK_LIBRARY} ${QT_QTCORE_LIBRARY} ${LIBS})

set (test-packagelist-sourceforge_SOURCES
	test-packagelist-sourceforge.cpp
	${installer_shared_SOURCES}
        ${quazip_HEADERS}
        ${quazip_SOURCES}
        ${qua7zip_SOURCES}
        ${qua7zip_HEADERS}
        ${md5_SOURCES}
		${md5_HEADERS}
)

qt4_automoc(${test-packagelist-sourceforge_SOURCES})

add_executable(test-packagelist-sourceforge ${test-packagelist-sourceforge_SOURCES})
target_link_libraries(test-packagelist-sourceforge ${QT_QTMAIN_LIBRARIES} ${QT_QTNETWORK_LIBRARY} ${QT_QTCORE_LIBRARY} ${LIBS})

set (test-globalconfig_SOURCES
	test-globalconfig.cpp
	${installer_shared_SOURCES}
        ${quazip_HEADERS}
        ${quazip_SOURCES}
        ${qua7zip_SOURCES}
        ${qua7zip_HEADERS}
        ${md5_SOURCES}
		${md5_HEADERS}
)

qt4_automoc(${test-globalconfig_SOURCES})

add_executable(test-globalconfig ${test-globalconfig_SOURCES})
target_link_libraries(test-globalconfig ${QT_QTMAIN_LIBRARIES} ${QT_QTNETWORK_LIBRARY} ${QT_QTCORE_LIBRARY} ${LIBS})

set (test-settings_SOURCES
	test-settings.cpp
	../shared/settings.cpp
	../shared/misc.cpp
)

qt4_automoc(${test-settings_SOURCES})
add_executable(test-settings ${test-settings_SOURCES})
target_link_libraries(test-settings ${QT_QTMAIN_LIBRARIES} ${QT_QTNETWORK_LIBRARY} ${QT_QTCORE_LIBRARY} ${LIBS})

include_directories(${CMAKE_CURRENT_BINARY_DIR} )

set (test-packager_SOURCES
	test-packager.cpp
	../shared/packager.cpp
	../shared/misc.cpp
        ${quazip_HEADERS}
        ${quazip_SOURCES}
        ${qua7zip_SOURCES}
        ${qua7zip_HEADERS}
        ${md5_SOURCES}
		${md5_HEADERS}
)

qt4_automoc(${test-packager_SOURCES})
add_executable(test-packager ${test-packager_SOURCES})
target_link_libraries(test-packager ${QT_QTMAIN_LIBRARIES} ${QT_QTNETWORK_LIBRARY} ${QT_QTCORE_LIBRARY} ${LIBS})


set (test-startmenu_SOURCES
	test-startmenu.cpp
	../shared/misc.cpp
)

qt4_automoc(${test-startmenu_SOURCES})
add_executable(test-startmenu ${test-startmenu_SOURCES})
target_link_libraries(test-startmenu ${QT_QTMAIN_LIBRARIES} ${QT_QTNETWORK_LIBRARY} ${QT_QTCORE_LIBRARY} ${LIBS})

set (test-proxy_SOURCES
	test-proxy.cpp
	../shared/misc.cpp
	../shared/misc.h
	../shared/settings.cpp
	../shared/settings.h
)

qt4_automoc(${test-proxy_SOURCES})
add_executable(test-proxy ${test-proxy_SOURCES})
target_link_libraries(test-proxy ${QT_QTMAIN_LIBRARIES} ${QT_QTNETWORK_LIBRARY} ${QT_QTCORE_LIBRARY} ${LIBS})

set (TEST_NAME test-database)
set (${TEST_NAME}_SOURCES
	${TEST_NAME}.cpp
	${installer_shared_SOURCES}
    ${quazip_HEADERS}
    ${quazip_SOURCES}
    ${qua7zip_SOURCES}
    ${qua7zip_HEADERS}
        ${md5_SOURCES}
		${md5_HEADERS}
)

qt4_automoc(${${TEST_NAME}_SOURCES})
add_executable(${TEST_NAME} ${${TEST_NAME}_SOURCES})
target_link_libraries(${TEST_NAME} ${QT_QTMAIN_LIBRARIES} ${QT_QTNETWORK_LIBRARY} ${QT_QTCORE_LIBRARY} ${LIBS})
