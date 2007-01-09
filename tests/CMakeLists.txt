# our targets
include_directories(${CMAKE_CURRENT_BINARY_DIR})
 
set (test_mirrors_SOURCES 
	test-mirrors.cpp 
	../shared/mirrors.cpp
	../shared/downloader.cpp
	../shared/downloaderprogress.cpp
)

qt4_automoc(${test_mirrors_SOURCES})

add_executable(test-mirrors ${test_mirrors_SOURCES})
target_link_libraries(test-mirrors ${QT_QTNETWORK_LIBRARY} ${QT_QTCORE_LIBRARY} ${LIBS})


set (test-packagelist-apachemodindex_SOURCES
	test-packagelist-apachemodindex.cpp
	${installer_shared_SOURCES}
	${quazip_SOURCES}
)

qt4_automoc(${test-packagelist-apachemodindex_SOURCES})

add_executable(test-packagelist-apachemodindex ${test-packagelist-apachemodindex_SOURCES})
target_link_libraries(test-packagelist-apachemodindex ${QT_QTNETWORK_LIBRARY} ${QT_QTCORE_LIBRARY} ${LIBS})

set (test-packagelist-sourceforge_SOURCES
	test-packagelist-sourceforge.cpp
	${installer_shared_SOURCES}
	${quazip_SOURCES}
)

qt4_automoc(${test-packagelist-sourceforge_SOURCES})

add_executable(test-packagelist-sourceforge ${test-packagelist-sourceforge_SOURCES})
target_link_libraries(test-packagelist-sourceforge ${QT_QTNETWORK_LIBRARY} ${QT_QTCORE_LIBRARY} ${LIBS})

set (test-configparser_SOURCES
	test-configparser.cpp
	../shared/downloader.cpp
	../shared/downloaderprogress.cpp
	../shared/configparser.cpp
)

qt4_automoc(${test-configparser_SOURCES})

add_executable(test-configparser ${test-configparser_SOURCES})
target_link_libraries(test-configparser ${QT_NETWORK_LIBRARY} ${QT_QTCORE_LIBRARY} ${LIBS})

set (test-settings_SOURCES
	test-settings.cpp
	../shared/settings.cpp
)

qt4_automoc(${test-settings_SOURCES})

add_executable(test-settings ${test-settings_SOURCES})
target_link_libraries(test-settings ${QT_QTNETWORK_LIBRARY} ${QT_QTCORE_LIBRARY} ${LIBS})
