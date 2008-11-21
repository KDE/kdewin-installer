set (VER_MAJ 0)
set (VER_MIN 1)
set (VER_PATCH 0)

set (packager_SOURCES
  main.cpp
  packager.cpp
)

set (packager_HEADERS
  packager.h
)

include_directories(
  ${CMAKE_CURRENT_SOURCE_DIR}
  ${CMAKE_CURRENT_BINARY_DIR}
)

add_definitions(-DPACKAGER_VERSION=${VER_MAJ}.${VER_MIN}.${VER_PATCH})

qt4_automoc(${packager_SOURCES})

set (TARGET kdewin-packager)

add_executable(${TARGET} ${packager_SOURCES} ${packager_HEADERS})
target_link_libraries(${TARGET} ${CONSOLE_LIBS})

install_targets(/bin ${TARGET} )

pack_target(${TARGET} )
create_md5sum_file(${TARGET} )
