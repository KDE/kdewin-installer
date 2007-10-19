
set (configer_SOURCES
	main.cpp
	../shared/package.cpp
	../shared/misc.cpp
)

set (configer_HEADERS
	../shared/package.h
	../shared/misc.h
)

include_directories(
	${CMAKE_CURRENT_SOURCE_DIR}
	${CMAKE_CURRENT_BINARY_DIR}
)

add_definitions(-DPACKAGE_SMALL_VERSION -DMISC_SMALL_VERSION)

qt4_automoc(${configer_SOURCES})
add_executable(kdewin-configer ${configer_SOURCES} ${configer_HEADERS})
target_link_libraries(kdewin-configer ${QT_QTMAIN_LIBRARY} ${QT_QTNETWORK_LIBRARY} ${QT_QTCORE_LIBRARY} ${LIBS})

if (CMAKE_PRODUCTION_BUILD)
    install_targets(/bin kdewin-configer )
endif (CMAKE_PRODUCTION_BUILD)

if (UPX_EXECUTABLE AND CMAKE_PRODUCTION_BUILD)
    add_custom_command(
        TARGET kdewin-configer
        POST_BUILD 
        COMMAND ${UPX_EXECUTABLE} --lzma -9 "${EXECUTABLE_OUTPUT_PATH}/kdewin-configer.exe" 
    )
endif (UPX_EXECUTABLE AND CMAKE_PRODUCTION_BUILD)
