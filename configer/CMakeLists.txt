
set (configer_SOURCES
    main.cpp
)

set (configer_HEADERS
)

include_directories(
    ${CMAKE_CURRENT_SOURCE_DIR}
    ${CMAKE_CURRENT_BINARY_DIR}
)

add_definitions(-DPACKAGE_SMALL_VERSION -DMISC_SMALL_VERSION)

qt4_automoc(${configer_SOURCES})

SET (TARGET kdewin-configer)

add_executable(${TARGET} ${configer_SOURCES} ${configer_HEADERS})
target_link_libraries(${TARGET} ${CONSOLE_LIBS})

install_targets(/bin ${TARGET} )

pack_target(${TARGET})
create_checksum_file(${TARGET})