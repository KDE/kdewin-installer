# FIXME: The original QT4_ADD_RESOURCES should be extended to support this filetype too
#
# QT4_ADD_RESOURCE2(outfiles inputfile ... )
# TODO  perhaps consider adding support for compression and root options to rcc

MACRO(QT4_ADD_RESOURCES2 outfiles )

FIND_PROGRAM(WINDRES_EXECUTABLE windres)
FOREACH(it ${ARGN})
  GET_FILENAME_COMPONENT(outfilename ${it} NAME_WE)
  GET_FILENAME_COMPONENT(infile ${it} ABSOLUTE)
  GET_FILENAME_COMPONENT(rc_path ${infile} PATH)
  SET(outfile ${CMAKE_CURRENT_BINARY_DIR}/${outfilename}_res.o)
  #  parse file for dependencies
  FILE(READ "${infile}" _RC_FILE_CONTENTS)
  STRING(REGEX MATCHALL "<file>[^<]*" _RC_FILES "${_RC_FILE_CONTENTS}")
  SET(_RC_DEPENDS)
  FOREACH(_RC_FILE ${_RC_FILES})
    STRING(REGEX REPLACE "^<file>" "" _RC_FILE "${_RC_FILE}")
    SET(_RC_DEPENDS ${_RC_DEPENDS} "${rc_path}/${_RC_FILE}")
  ENDFOREACH(_RC_FILE)
  ADD_CUSTOM_COMMAND(OUTPUT ${outfile}
    COMMAND ${WINDRES_EXECUTABLE}
    ARGS -i ${infile} -o ${outfile} --include-dir=${CMAKE_CURRENT_SOURCE_DIR}
    MAIN_DEPENDENCY ${infile}
    DEPENDS ${_RC_DEPENDS})
  SET(${outfiles} ${${outfiles}} ${outfile})
ENDFOREACH(it)

ENDMACRO(QT4_ADD_RESOURCES2)

if (BUILD_TRANSLATIONS)
    # copied from cmake Modules dir because the original one does not support the _target_dir parameter
    MACRO(QT4_CREATE_TRANSLATION _qm_files  _target_dir)
      SET(_my_sources)
      SET(_my_tsfiles)
      FOREACH (_file ${ARGN})
         GET_FILENAME_COMPONENT(_ext ${_file} EXT)
         GET_FILENAME_COMPONENT(_abs_FILE ${_file} ABSOLUTE)
         IF(_ext MATCHES "ts")
           LIST(APPEND _my_tsfiles ${_abs_FILE})
         ELSE(_ext MATCHES "ts")
              LIST(APPEND _my_sources ${_abs_FILE})
         ENDIF(_ext MATCHES "ts")
      ENDFOREACH(_file)
      FOREACH(_ts_file ${_my_tsfiles})
        ADD_CUSTOM_COMMAND(OUTPUT ${_ts_file}
           COMMAND ${QT_LUPDATE_EXECUTABLE}
           ARGS ${_my_sources} -ts ${_ts_file}
           DEPENDS ${_my_sources}
           COMMENT "updating ${_ts_file}"
       )
      ENDFOREACH(_ts_file)
      QT4_ADD_TRANSLATION(${_qm_files} ${_target_dir} ${_my_tsfiles})
    ENDMACRO(QT4_CREATE_TRANSLATION)

    MACRO(QT4_ADD_TRANSLATION _qm_files _target_dir)
      IF(_target_dir STREQUAL "")
         SET(_target_dir ${CMAKE_CURRENT_BINARY_DIR})
      ENDIF(_target_dir STREQUAL "")
      FOREACH (_current_FILE ${ARGN})
         GET_FILENAME_COMPONENT(_abs_FILE ${_current_FILE} ABSOLUTE)
         GET_FILENAME_COMPONENT(qm ${_abs_FILE} NAME_WE)
         SET(qm "${_target_dir}/${qm}.qm")

         ADD_CUSTOM_COMMAND(OUTPUT ${qm}
            COMMAND ${QT_LRELEASE_EXECUTABLE}
            ARGS ${_abs_FILE} -qm ${qm}
            DEPENDS ${_abs_FILE}
           COMMENT "creating ${qm}"
         )
         SET(${_qm_files} ${${_qm_files}} ${qm})
      ENDFOREACH (_current_FILE)
    ENDMACRO(QT4_ADD_TRANSLATION)
endif(BUILD_TRANSLATIONS)

macro (pack_target _target)
    if (PACK_EXECUTABLE)
        get_target_property( _filename ${_target} LOCATION )
        add_custom_command(
            TARGET ${_target}
            POST_BUILD
            COMMAND ${UPX_EXECUTABLE} --lzma -9 "${_filename}"
            COMMENT "packing ${_target}"
        )
    endif (PACK_EXECUTABLE)
endmacro (pack_target)

macro (create_checksum_file _target)
    if (SHA1SUM_EXECUTABLE)
        get_target_property( _filename ${_target} LOCATION )
        get_filename_component(_name ${_filename} NAME)
        add_custom_command(
            TARGET ${_target}
            POST_BUILD
            COMMAND ${SHA1SUM_EXECUTABLE}
            ARGS -o ${_name}.sha1 ${_name}
            WORKING_DIRECTORY ${EXECUTABLE_OUTPUT_PATH}
            COMMENT "creating sha1sum file for ${_target}"
        )
        install(FILES ${_filename}.sha1 DESTINATION bin)
    endif (SHA1SUM_EXECUTABLE)
endmacro (create_checksum_file _target)

# the following macro adds a themed installer
macro (add_themed_installer _appname _resource)
  set (TARGET ${_appname}-installer-gui)
  set (TARGET_FILENAME ${_appname}-installer-gui-${VERSION_PATCH})

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

  QT4_ADD_RESOURCES(all_sources ${_resource})
  if(WIN32)
  if(MINGW)
    QT4_ADD_RESOURCES2(all_sources ${CMAKE_CURRENT_SOURCE_DIR}/gui.rc)
  else(MINGW)
    list(APPEND all_sources ${CMAKE_CURRENT_SOURCE_DIR}/gui.rc)
  endif(MINGW)
  endif(WIN32)

  qt4_wrap_cpp(all_sources ${all_headers})

#  if (BUILD_TRANSLATIONS)
#      qt4_create_translation(
#          _qm_files
#          ${CMAKE_BINARY_DIR}/bin
#          ${all_sources} ${all_headers}
#          ${CMAKE_SOURCE_DIR}/translations/installer-gui-de.ts
#          ${CMAKE_SOURCE_DIR}/translations/installer-gui-fr.ts
#      )
#
#      add_custom_target(${TARGET}-translations DEPENDS ${_qm_files} ) 
#      install(FILES ${_qm_files} DESTINATION bin)
#      list(APPEND all_sources ${_qm_files})
#  endif (BUILD_TRANSLATIONS)

  if(WIN32)
    set(_WIN32 WIN32)
  else(WIN32)
    set(_WIN32)
  endif(WIN32)
  add_executable(${TARGET} ${_WIN32} ${all_sources} ${all_headers})
  set_target_properties(${TARGET} PROPERTIES COMPILE_FLAGS -DUSE_GUI)
  set_target_properties(${TARGET} PROPERTIES OUTPUT_NAME ${TARGET_FILENAME})
  target_link_libraries(${TARGET}
                        ${QT_QTCORE_LIBRARY}
                        ${QT_QTGUI_LIBRARY}
                        ${QT_QTMAIN_LIBRARY}
                        ${GUI_LIBS}
  )
  write_file(${EXECUTABLE_OUTPUT_PATH}/${TARGET}.bat "start ${TARGET_FILENAME}")

  install_targets(/bin ${TARGET} )
  install(FILES ${EXECUTABLE_OUTPUT_PATH}/${TARGET}.bat DESTINATION bin)

  pack_target(${TARGET})
  create_checksum_file(${TARGET})
endmacro (add_themed_installer _appname _resource)
