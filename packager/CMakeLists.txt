set (VER_MAJ 0)
set (VER_MIN 1)
set (VER_PATCH 0)

set (packager_SOURCES
  main.cpp
  packager.cpp
  ../shared/misc.cpp
  ../shared/settings.cpp
  ${quazip_SOURCES}
  ${filters_SOURCES}
  ${bzip2_SOURCES}
)

set (packager_HEADERS
  packager.h
  ../shared/misc.h
  ../shared/settings.h
  ${quazip_HEADERS}
  ${filters_HEADERS}
  ${bzip2_HEADERS}
)

include_directories(
  ${CMAKE_CURRENT_SOURCE_DIR}
  ${CMAKE_CURRENT_BINARY_DIR}
)

add_definitions(-DPACKAGER_VERSION=${VER_MAJ}.${VER_MIN}.${VER_PATCH})

source_group(bzip2   FILES ${bzip2_SOURCES}   ${bzip2_HEADERS})
source_group(filters FILES ${filters_SOURCES} ${filters_HEADERS})
source_group(quazip  FILES ${quazip_SOURCES}  ${quazip_HEADERS})
source_group(qua7zip FILES ${qua7zip_SOURCES} ${qua7zip_HEADERS})

qt4_automoc(${packager_SOURCES})
add_executable(kdewin-packager ${packager_SOURCES} ${packager_HEADERS})
target_link_libraries(kdewin-packager ${QT_QTMAIN_LIBRARY} ${QT_QTNETWORK_LIBRARY} ${QT_QTCORE_LIBRARY} ${LIBS})

install_targets(/bin kdewin-packager )

if (PACK_EXECUTABLE)
    get_target_property( packager_filename kdewin-packager LOCATION )
    add_custom_command(
        TARGET kdewin-packager
        POST_BUILD
        COMMAND ${UPX_EXECUTABLE} --lzma -9 "${packager_filename}"
    )
endif (PACK_EXECUTABLE)
