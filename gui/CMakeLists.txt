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

add_definitions(
    ${GUI_DEFINITIONS}
)

include_directories(
    ${CMAKE_CURRENT_SOURCE_DIR} 
    ${CMAKE_CURRENT_BINARY_DIR}
    ${GUI_INCLUDE_DIR}
)

if(CMAKE_COMPILER_2005)
     # to avoid a lot of deprecated warnings
     add_definitions( -D_CRT_SECURE_NO_DEPRECATE -D_CRT_NONSTDC_NO_DEPRECATE )
endif(CMAKE_COMPILER_2005)


set(installer_GUI_SOURCES
   ${CMAKE_CURRENT_SOURCE_DIR}/dependenciespage.cpp
   ${CMAKE_CURRENT_SOURCE_DIR}/downloadpage.cpp
   ${CMAKE_CURRENT_SOURCE_DIR}/downloadsettingspage.cpp
   ${CMAKE_CURRENT_SOURCE_DIR}/finishpage.cpp
   ${CMAKE_CURRENT_SOURCE_DIR}/installdirectorypage.cpp
   ${CMAKE_CURRENT_SOURCE_DIR}/installerdialogs.cpp
   ${CMAKE_CURRENT_SOURCE_DIR}/installerenginegui.cpp
   ${CMAKE_CURRENT_SOURCE_DIR}/installpage.cpp
   ${CMAKE_CURRENT_SOURCE_DIR}/usercompilermodepage.cpp
   ${CMAKE_CURRENT_SOURCE_DIR}/enduserinstallmodepage.cpp
   ${CMAKE_CURRENT_SOURCE_DIR}/enduserpackageselectorpage.cpp
   ${CMAKE_CURRENT_SOURCE_DIR}/installwizard.cpp
   ${CMAKE_CURRENT_SOURCE_DIR}/installwizardpage.cpp
   ${CMAKE_CURRENT_SOURCE_DIR}/internetsettingspage.cpp
   ${CMAKE_CURRENT_SOURCE_DIR}/mirrorsettingspage.cpp
   ${CMAKE_CURRENT_SOURCE_DIR}/packageselectorpage.cpp
   ${CMAKE_CURRENT_SOURCE_DIR}/postprocesspage.cpp
   ${CMAKE_CURRENT_SOURCE_DIR}/releaseselectionpage.cpp
   ${CMAKE_CURRENT_SOURCE_DIR}/settingspage.cpp
   ${CMAKE_CURRENT_SOURCE_DIR}/titlepage.cpp
   ${CMAKE_CURRENT_SOURCE_DIR}/uninstallpage.cpp
   ${CMAKE_CURRENT_SOURCE_DIR}/main.cpp
)

set(installer_GUI_HEADERS
   ${CMAKE_CURRENT_SOURCE_DIR}/dependenciespage.h
   ${CMAKE_CURRENT_SOURCE_DIR}/downloadpage.h
   ${CMAKE_CURRENT_SOURCE_DIR}/downloadsettingspage.h
   ${CMAKE_CURRENT_SOURCE_DIR}/finishpage.h
   ${CMAKE_CURRENT_SOURCE_DIR}/installdirectorypage.h
   ${CMAKE_CURRENT_SOURCE_DIR}/installerdialogs.h
   ${CMAKE_CURRENT_SOURCE_DIR}/installerenginegui.h
   ${CMAKE_CURRENT_SOURCE_DIR}/installpage.h
   ${CMAKE_CURRENT_SOURCE_DIR}/usercompilermodepage.h
   ${CMAKE_CURRENT_SOURCE_DIR}/enduserinstallmodepage.h
   ${CMAKE_CURRENT_SOURCE_DIR}/enduserpackageselectorpage.h
   ${CMAKE_CURRENT_SOURCE_DIR}/installwizard.h
   ${CMAKE_CURRENT_SOURCE_DIR}/installwizardpage.h
   ${CMAKE_CURRENT_SOURCE_DIR}/internetsettingspage.h
   ${CMAKE_CURRENT_SOURCE_DIR}/mirrorsettingspage.h
   ${CMAKE_CURRENT_SOURCE_DIR}/packageselectorpage.h
   ${CMAKE_CURRENT_SOURCE_DIR}/postprocesspage.h
   ${CMAKE_CURRENT_SOURCE_DIR}/releaseselectionpage.h
   ${CMAKE_CURRENT_SOURCE_DIR}/settingspage.h
   ${CMAKE_CURRENT_SOURCE_DIR}/titlepage.h
   ${CMAKE_CURRENT_SOURCE_DIR}/uninstallpage.h
)

set (main_UI_SOURCES 
   ${CMAKE_CURRENT_SOURCE_DIR}/dependenciespage.ui
   ${CMAKE_CURRENT_SOURCE_DIR}/downloadsettingspage.ui
   ${CMAKE_CURRENT_SOURCE_DIR}/installdirectorypage.ui
   ${CMAKE_CURRENT_SOURCE_DIR}/usercompilermodepage.ui
   ${CMAKE_CURRENT_SOURCE_DIR}/enduserinstallmodepage.ui
   ${CMAKE_CURRENT_SOURCE_DIR}/enduserpackageselectorpage.ui
   ${CMAKE_CURRENT_SOURCE_DIR}/internetsettingspage.ui
   ${CMAKE_CURRENT_SOURCE_DIR}/mirrorsettingspage.ui
   ${CMAKE_CURRENT_SOURCE_DIR}/releaseselectionpage.ui
   ${CMAKE_CURRENT_SOURCE_DIR}/postprocesspage.ui
   ${CMAKE_CURRENT_SOURCE_DIR}/settingspage.ui
)

set(main_all_sources 
    ${installer_GUI_SOURCES}
    ${installer_shared_SOURCES}
    ${md5_SOURCES}
)
set(main_all_headers
    ${installer_shared_HEADERS}
    ${installer_GUI_HEADERS}
    ${md5_HEADERS}
)

if(BUILD_INSTALLER_THEME)
  add_subdirectory(themes/${BUILD_INSTALLER_THEME})
else(BUILD_INSTALLER_THEME)
  add_subdirectory(themes/main)
endif(BUILD_INSTALLER_THEME)
