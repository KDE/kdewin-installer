set(installer_CONSOLE_SOURCES
   main.cpp
)

set(installer_CONSOLE_HEADERS
)

include_directories(${CMAKE_CURRENT_SOURCE_DIR} ${CMAKE_CURRENT_BINARY_DIR})

set(all_sources ${installer_CONSOLE_SOURCES}
                ${installer_CONSOLE_HEADERS}
                ${installer_shared_SOURCES}
                ${installer_shared_HEADERS}
                ${quazip_SOURCES}
                ${quazip_HEADERS})

qt4_automoc(${all_sources})

add_executable(kdewin-installer-console ${all_sources})

target_link_libraries(kdewin-installer-console ${QT_QTCORE_LIBRARY} ${QT_QTNETWORK_LIBRARY} ${QT_QTGUI_LIBRARY} advapi32 shell32 ws2_32 )
