
set (configer_SOURCES
	main.cpp
	../shared/debug.cpp
	../shared/hintfile.cpp
	../shared/misc.cpp
	../shared/package.cpp
)

set (configer_HEADERS
	../shared/debug.h
	../shared/hintfile.h
	../shared/misc.h
	../shared/package.h
)

include_directories(
	${CMAKE_CURRENT_SOURCE_DIR}
	${CMAKE_CURRENT_BINARY_DIR}
)

add_definitions(-DPACKAGE_SMALL_VERSION -DMISC_SMALL_VERSION)

qt4_automoc(${configer_SOURCES})
add_executable(kdewin-configer ${configer_SOURCES} ${configer_HEADERS})
target_link_libraries(kdewin-configer ${QT_QTMAIN_LIBRARY} ${QT_QTNETWORK_LIBRARY} ${QT_QTCORE_LIBRARY} ${LIBS})

install_targets(/bin kdewin-configer )

if (PACK_EXECUTABLE)
    get_target_property( configer_filename kdewin-configer LOCATION )
    add_custom_command(
        TARGET kdewin-configer
        POST_BUILD 
        COMMAND ${UPX_EXECUTABLE} --lzma -9 "${configer_filename}" 
    )
endif (PACK_EXECUTABLE)
