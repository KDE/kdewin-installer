# our targets
 
set (test_mirrors_SOURCES 
	test-mirrors.cpp 
	../shared/mirrors.cpp
	../shared/downloader.cpp
	../shared/downloaderprogress.cpp
)
include_directories(${CMAKE_CURRENT_BINARY_DIR})

qt4_automoc(${test_mirrors_SOURCES})

add_executable(test-mirrors ${test_mirrors_SOURCES})
target_link_libraries(test-mirrors ${QT_QTCORE_LIBRARY} ${QT_QTNETWORK_LIBRARY})
