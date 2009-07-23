set (APPNAME kdewin)
set (TARGET ${APPNAME}-installer-gui)
set (TARGET_FILENAME ${APPNAME}-installer-gui-${VERSION_PATCH})
set (DESKTOP_APPNAME "KDE Installer")

configure_file(gui.qrc.cmake ${CMAKE_CURRENT_BINARY_DIR}/gui.qrc)
configure_file(installer.desktop.cmake ${CMAKE_CURRENT_BINARY_DIR}/installer.desktop)

include_directories(${CMAKE_CURRENT_SOURCE_DIR} ${CMAKE_CURRENT_BINARY_DIR} )

set (UI_SOURCES 
   ${main_UI_SOURCES}
   titlepage.ui
)

set(all_sources 
    ${main_all_sources}
)

set(all_headers
    ${main_all_headers}
)

QT4_WRAP_UI(all_sources ${UI_SOURCES})

QT4_ADD_RESOURCES(all_sources ${CMAKE_CURRENT_BINARY_DIR}/gui.qrc)
if(WIN32)
 if(MINGW)
   QT4_ADD_RESOURCES2(all_sources gui.rc)
 else(MINGW)
   list(APPEND all_sources gui.rc)
 endif(MINGW)
endif(WIN32)

qt4_wrap_cpp(all_sources ${all_headers})

if (BUILD_TRANSLATIONS)
    qt4_create_translation(
        _qm_files
        ${CMAKE_BINARY_DIR}/bin
        ${all_sources} ${all_headers}
        ${CMAKE_SOURCE_DIR}/translations/installer-gui-de.ts
        ${CMAKE_SOURCE_DIR}/translations/installer-gui-fr.ts
    )

    add_custom_target(${TARGET}-translations DEPENDS ${_qm_files} ) 
    install(FILES ${_qm_files} DESTINATION bin)
    list(APPEND all_sources ${_qm_files})
endif (BUILD_TRANSLATIONS)

add_executable(${TARGET} WIN32 ${all_sources} ${all_headers})
set_target_properties(${TARGET} PROPERTIES COMPILE_FLAGS -DUSE_GUI)
set_target_properties(${TARGET} PROPERTIES OUTPUT_NAME ${TARGET_FILENAME})
target_link_libraries(${TARGET}
                      ${QT_QTCORE_LIBRARY} ${QT_QTMAIN_LIBRARY}  ${QT_QTGUI_LIBRARY}
                      ${GUI_LIBS}
)
write_file(${EXECUTABLE_OUTPUT_PATH}/${TARGET}.bat "start ${TARGET_FILENAME}")

install_targets(/bin ${TARGET} )
install(FILES ${EXECUTABLE_OUTPUT_PATH}/${TARGET}.bat DESTINATION bin)
install(FILES ${CMAKE_CURRENT_BINARY_DIR}/installer.desktop DESTINATION share/applications/kde4)

pack_target(${TARGET})
create_checksum_file(${TARGET})
