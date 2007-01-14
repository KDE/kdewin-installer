set(installer_GUI_SOURCES
   complexwizard.cpp
   installwizard.cpp
#   settingspage.cpp
   main.cpp
)

set(installer_GUI_HEADERS
   complexwizard.h
   installwizard.h
#   settingspage.h
#   ui_settingspage.h
)

include_directories(${CMAKE_CURRENT_SOURCE_DIR} ${CMAKE_CURRENT_BINARY_DIR})

add_definitions(-DUSE_GUI)
if(CMAKE_COMPILER_2005)
     # to avoid a lot of deprecated warnings
     add_definitions( -D_CRT_SECURE_NO_DEPRECATE -D_CRT_NONSTDC_NO_DEPRECATE )
endif(CMAKE_COMPILER_2005)

#QT4_WRAP_UI(installer_GUI_SOURCES settingspage.ui)

set(all_sources ${installer_GUI_SOURCES}
                ${installer_GUI_HEADERS}
                ${installer_shared_SOURCES}
                ${installer_shared_HEADERS}
                ${quazip_SOURCES}
                ${quazip_HEADERS})

qt4_automoc(${all_sources})

add_executable(kdewin-installer-gui WIN32 ${all_sources})

target_link_libraries(kdewin-installer-gui ${QT_QTMAIN_LIBRARIES} ${QT_QTGUI_LIBRARY} ${QT_QTNETWORK_LIBRARY} ${QT_QTCORE_LIBRARY} ${GUI_LIBS})
