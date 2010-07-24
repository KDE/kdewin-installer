set (VER_MAJ 0)
set (VER_MIN 1)
set (VER_PATCH 0)

add_definitions(
    -DPACKAGER_VERSION=${VER_MAJ}.${VER_MIN}.${VER_PATCH}
    ${CONSOLE_DEFINITIONS}
)

include_directories(
  ${CMAKE_CURRENT_SOURCE_DIR}
  ${CMAKE_CURRENT_BINARY_DIR}
  ${CONSOLE_INCLUDE_DIR}
)

set (packager_SOURCES
  main.cpp
  packager.cpp
  xmltemplatepackager.cpp
  ../shared/debug.cpp
  ../shared/misc.cpp
)

set (packager_HEADERS
  packager.h
  xmltemplatepackager.h
)




qt4_automoc(${packager_SOURCES})

QT4_ADD_RESOURCES(packager_SOURCES ${CMAKE_CURRENT_SOURCE_DIR}/packager.qrc)

set (TARGET kdewin-packager)

add_executable(${TARGET} ${packager_SOURCES} ${packager_HEADERS})
target_link_libraries(${TARGET} ${CONSOLE_LIBS} ${QT_QTXML_LIBRARIES})

install_targets(/bin ${TARGET} )

pack_target(${TARGET} )
create_checksum_file(${TARGET} )
