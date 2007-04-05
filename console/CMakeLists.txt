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

set(all_sources ${installer_CONSOLE_SOURCES}
                ${installer_CONSOLE_HEADERS}
                ${installer_shared_SOURCES}
                ${installer_shared_HEADERS}
                ${quazip_HEADERS}
                ${quazip_SOURCES}
                ${qua7zip_SOURCES}
                ${qua7zip_HEADERS}
		        ${md5_SOURCES}
				${md5_HEADERS}
        )

source_group(quazip FILES ${quazip_SOURCES} ${quazip_HEADERS})
source_group(qua7zip FILES ${qua7zip_SOURCES} ${qua7zip_HEADERS})

qt4_automoc(${all_sources})

add_executable(kdewin-installer-console ${all_sources})

target_link_libraries(kdewin-installer-console ${QT_QTMAIN_LIBRARIES} ${QT_QTNETWORK_LIBRARY} ${QT_QTCORE_LIBRARY} ${LIBS})

if (CMAKE_PRODUCTION_BUILD)
    install_targets(/bin kdewin-installer-console )
endif (CMAKE_PRODUCTION_BUILD)
