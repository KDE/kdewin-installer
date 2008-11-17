set (APPNAME amarok)

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

if (PACK_EXECUTABLE)
    add_custom_command(
        TARGET ${TARGET}
        POST_BUILD
        COMMAND ${UPX_EXECUTABLE} --lzma -9 "${TARGET_FILENAME}"
    )
endif (PACK_EXECUTABLE)

if (MD5SUM_EXECUTABLE)
    add_custom_command(
        TARGET ${TARGET}
        POST_BUILD
        COMMAND "${MD5SUM_EXECUTABLE} -o ${TARGET_FILENAME}.exe.md5 ${TARGET_FILENAME}.exe"
        WORKING_DIRECTORY ${EXECUTABLE_OUTPUT_PATH}
    )
    install(FILES ${EXECUTABLE_OUTPUT_PATH}/${TARGET_FILENAME}.exe.md5 DESTINATION bin)
endif (MD5SUM_EXECUTABLE)
