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
target_link_libraries(test-mirrors ${QT_QTCORE_LIBRARY} ${QT_QTNETWORK_LIBRARY})


set (test-packagelist-apachemodindex_SOURCES
	test-packagelist-apachemodindex.cpp
	../shared/mirrors.cpp
	../shared/downloader.cpp
	../shared/downloaderprogress.cpp
	../shared/packagelist.cpp
	../shared/package.cpp
)

qt4_automoc(${test-packagelist-apachemodindex_SOURCES})

add_executable(test-packagelist-apachemodindex ${test-packagelist-apachemodindex_SOURCES})
target_link_libraries(test-packagelist-apachemodindex ${QT_QTCORE_LIBRARY} ${QT_QTNETWORK_LIBRARY})

set (test-packagelist-sourceforge_SOURCES
	test-packagelist-sourceforge.cpp
	../shared/mirrors.cpp
	../shared/downloader.cpp
	../shared/downloaderprogress.cpp
	../shared/packagelist.cpp
	../shared/package.cpp
)

qt4_automoc(${test-packagelist-sourceforge_SOURCES})

add_executable(test-packagelist-sourceforge ${test-packagelist-sourceforge_SOURCES})
target_link_libraries(test-packagelist-sourceforge ${QT_QTCORE_LIBRARY} ${QT_QTNETWORK_LIBRARY})
