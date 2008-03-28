set (APPNAME kdewin)

include_directories(${CMAKE_CURRENT_SOURCE_DIR} ${CMAKE_CURRENT_BINARY_DIR} )

set (UI_SOURCES 
   ${main_UI_SOURCES}
   titlepage.ui
)

set(all_sources 
    ${main_all_sources}
)

QT4_WRAP_UI(all_sources ${UI_SOURCES})

QT4_ADD_RESOURCES(all_sources gui.qrc)
if(WIN32)
 if(MINGW)
   QT4_ADD_RESOURCES2(all_sources gui.rc)
 else(MINGW)
   list(APPEND all_sources gui.rc)
 endif(MINGW)
endif(WIN32)

qt4_automoc(${all_sources})

add_executable(${APPNAME}-installer-gui-${VERSION} WIN32 ${all_sources})
target_link_libraries(${APPNAME}-installer-gui-${VERSION}
                      ${QT_QTCORE_LIBRARY} ${QT_QTMAIN_LIBRARY}  ${QT_QTGUI_LIBRARY}
                      ${GUI_LIBS}
)

get_target_property( installer_filename ${APPNAME}-installer-gui-${VERSION} LOCATION )
get_filename_component( installer_filename_only ${installer_filename} NAME )
write_file(${EXECUTABLE_OUTPUT_PATH}/${APPNAME}-installer-gui.bat "start ${installer_filename_only}")

install_targets(/bin ${APPNAME}-installer-gui-${VERSION} )
install(FILES ${EXECUTABLE_OUTPUT_PATH}/${APPNAME}-installer-gui.bat DESTINATION bin)

if (PACK_EXECUTABLE)
    add_custom_command(
        TARGET ${APPNAME}-installer-gui-${VERSION}
        POST_BUILD
        COMMAND ${UPX_EXECUTABLE} --lzma -9 "${installer_filename}"
    )
endif (PACK_EXECUTABLE)

