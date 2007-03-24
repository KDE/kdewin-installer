set (VER_MAJ 0)
set (VER_MIN 1)
set (VER_PATCH 0)

set (packager_SOURCES
	main.cpp
	../shared/packager.cpp
	../shared/misc.cpp
	${quazip_SOURCES}
	${md5_SOURCES}
)

set (packager_HEADERS
    ../shared/packager.h
	../shared/misc.h
	${quazip_HEADERS}
	${md5_HEADERS}
)

include_directories(
	${CMAKE_CURRENT_SOURCE_DIR}
	${CMAKE_CURRENT_BINARY_DIR}
)

add_definitions(-DPACKAGER_VERSION=${VER_MAJ}.${VER_MIN}.${VER_PATCH})

source_group(quazip FILES ${quazip_SOURCES} ${quazip_HEADERS})

qt4_automoc(${packager_SOURCES})
add_executable(kdewin-packager ${packager_SOURCES} ${packager_HEADERS})
target_link_libraries(kdewin-packager ${QT_QTMAIN_LIBRARIES} ${QT_QTNETWORK_LIBRARY} ${QT_QTCORE_LIBRARY} ${LIBS})

install_targets(/bin kdewin-packager )
