# this file is taken over directly from packager and customized to the menubuilder needs
set (VER_MAJ 0)
set (VER_MIN 1)
set (VER_PATCH 0)

set (menubuilder_SOURCES
  main.cpp
  ../shared/settings.cpp
  ../shared/misc.cpp
)

set (menubuilder_HEADERS
  ../shared/misc.h
  ../shared/settings.h
)

include_directories(
  ${CMAKE_CURRENT_SOURCE_DIR}
  ${CMAKE_CURRENT_BINARY_DIR}
)

add_definitions(-DMENUBUILDER_VERSION=${VER_MAJ}.${VER_MIN}.${VER_PATCH})

qt4_automoc(${menubuilder_SOURCES})
add_executable(kdewin-menubuilder ${menubuilder_SOURCES} ${menubuilder_HEADERS})
target_link_libraries(kdewin-menubuilder ${CONSOLE_LIBS})

install_targets(/bin kdewin-menubuilder )

if (PACK_EXECUTABLE)
    get_target_property( menubuilder_filename kdewin-menubuilder LOCATION )
    add_custom_command(
        TARGET kdewin-menubuilder
        POST_BUILD
        COMMAND ${UPX_EXECUTABLE} --lzma -9 "${menubuilder_filename}"
    )
endif (PACK_EXECUTABLE)
