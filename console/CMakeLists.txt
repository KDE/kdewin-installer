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

add_executable(kdewin-installer-console ${all_sources})

target_link_libraries(kdewin-installer-console ${CONSOLE_LIBS})

install_targets(/bin kdewin-installer-console )

set (TARGET kdewin-installer-console)
set (TARGET_FILENAME ${TARGET})

if (MD5SUM_EXECUTABLE)
    add_custom_command(
        TARGET ${TARGET}
        POST_BUILD
        COMMAND "${MD5SUM_EXECUTABLE} -o ${TARGET_FILENAME}${CMAKE_EXECUTABLE_SUFFIX}.md5 ${TARGET_FILENAME}${CMAKE_EXECUTABLE_SUFFIX}"
        WORKING_DIRECTORY ${EXECUTABLE_OUTPUT_PATH}
        COMMENT "creating md5sum file for ${TARGET_FILENAME}${CMAKE_EXECUTABLE_SUFFIX}"
    )
    install(FILES ${EXECUTABLE_OUTPUT_PATH}/${TARGET_FILENAME}${CMAKE_EXECUTABLE_SUFFIX}.md5 DESTINATION bin)
endif (MD5SUM_EXECUTABLE)
