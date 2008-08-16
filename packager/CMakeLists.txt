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
add_executable(kdewin-packager ${packager_SOURCES} ${packager_HEADERS})
target_link_libraries(kdewin-packager ${CONSOLE_LIBS})

install_targets(/bin kdewin-packager )

if (PACK_EXECUTABLE)
    get_target_property( packager_filename kdewin-packager LOCATION )
    add_custom_command(
        TARGET kdewin-packager
        POST_BUILD
        COMMAND ${UPX_EXECUTABLE} --lzma -9 "${packager_filename}"
    )
endif (PACK_EXECUTABLE)
