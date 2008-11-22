set (APPNAME kdewin)

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

QT4_ADD_RESOURCES(all_sources gui.qrc)
if(WIN32)
 if(MINGW)
   QT4_ADD_RESOURCES2(all_sources gui.rc)
 else(MINGW)
   list(APPEND all_sources gui.rc)
 endif(MINGW)
endif(WIN32)

qt4_wrap_cpp(all_sources ${all_headers})

set (TARGET ${APPNAME}-installer-gui)
set (TARGET_FILENAME ${APPNAME}-installer-gui-${VERSION_PATCH})

add_executable(${TARGET} WIN32 ${all_sources})
set_target_properties(${TARGET} PROPERTIES COMPILE_FLAGS -DUSE_GUI)
set_target_properties(${TARGET} PROPERTIES OUTPUT_NAME ${TARGET_FILENAME})
target_link_libraries(${TARGET}
                      ${QT_QTCORE_LIBRARY} ${QT_QTMAIN_LIBRARY}  ${QT_QTGUI_LIBRARY}
                      ${GUI_LIBS}
)

write_file(${EXECUTABLE_OUTPUT_PATH}/${TARGET}.bat "start ${TARGET_FILENAME}")

install_targets(/bin ${TARGET} )
install(FILES ${EXECUTABLE_OUTPUT_PATH}/${TARGET}.bat DESTINATION bin)

pack_target(${TARGET})
create_md5sum_file(${TARGET})
