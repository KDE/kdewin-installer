set(installer_GUI_SOURCES
   complexwizard.cpp
   installwizard.cpp
#   licensewizard.cpp
   main.cpp
)

set(installer_GUI_HEADERS
   complexwizard.h
   installwizard.h
#   licensewizard.h
#   spinboxdelegate.h
)

include_directories(${CMAKE_CURRENT_SOURCE_DIR} ${CMAKE_CURRENT_BINARY_DIR})

add_definitions(-DUSE_GUI)

set(all_sources ${installer_GUI_SOURCES}
                ${installer_GUI_HEADERS}
                ${installer_shared_SOURCES}
                ${installer_shared_HEADERS}
                ${quazip_SOURCES}
                ${quazip_HEADERS})

qt4_automoc(${all_sources})

add_executable(kdewin-installer-gui ${all_sources})

target_link_libraries(kdewin-installer-gui ${QT_QTCORE_LIBRARY} ${QT_QTNETWORK_LIBRARY} ${QT_QTGUI_LIBRARY} advapi32 shell32 ws2_32 )
