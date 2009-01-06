set(installer_CONSOLE_SOURCES
   main.cpp
   installerengineconsole.cpp
)

set(installer_CONSOLE_HEADERS
   installerengineconsole.h
)

include_directories(${CMAKE_CURRENT_SOURCE_DIR} ${CMAKE_CURRENT_BINARY_DIR})

if(CMAKE_COMPILER_2005)
     # to avoid a lot of deprecated warnings
     add_definitions( -D_CRT_SECURE_NO_DEPRECATE -D_CRT_NONSTDC_NO_DEPRECATE )
endif(CMAKE_COMPILER_2005)

set(all_sources 
    ${installer_CONSOLE_SOURCES}
    ${installer_CONSOLE_HEADERS}
)

qt4_automoc(${all_sources})

set (TARGET kdewin-installer-console)

add_executable(${TARGET} ${all_sources})
target_link_libraries(${TARGET} ${CONSOLE_LIBS})

install_targets(/bin ${TARGET})

pack_target(${TARGET})
create_checksum_file(${TARGET})
