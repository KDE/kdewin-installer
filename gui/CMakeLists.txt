# FIXME: The original QT4_ADD_RESOURCES should be extended to support this filetype too
#
# QT4_ADD_RESOURCE2(outfiles inputfile ... )
# TODO  perhaps consider adding support for compression and root options to rcc

MACRO (QT4_ADD_RESOURCES2 outfiles )

FOREACH (it ${ARGN})
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
    COMMAND windres
    ARGS -i ${infile} -o ${outfile} --include-dir=${CMAKE_CURRENT_SOURCE_DIR}
    MAIN_DEPENDENCY ${infile}
    DEPENDS ${_RC_DEPENDS})
  SET(${outfiles} ${${outfiles}} ${outfile})
ENDFOREACH (it)

ENDMACRO (QT4_ADD_RESOURCES2)



set(installer_GUI_SOURCES
   complexwizard.cpp
   installwizard.cpp
   settingspage.cpp
   main.cpp
)

set(installer_GUI_HEADERS
   complexwizard.h
   installwizard.h
   settingspage.h
)

include_directories(${CMAKE_CURRENT_SOURCE_DIR} ${CMAKE_CURRENT_BINARY_DIR})

add_definitions(-DUSE_GUI)
if(CMAKE_COMPILER_2005)
     # to avoid a lot of deprecated warnings
     add_definitions( -D_CRT_SECURE_NO_DEPRECATE -D_CRT_NONSTDC_NO_DEPRECATE )
endif(CMAKE_COMPILER_2005)

QT4_WRAP_UI(installer_GUI_SOURCES settingspage.ui)
QT4_ADD_RESOURCES(installer_GUI_SOURCES gui.qrc)
if(WIN32)
 if(MINGW)
   QT4_ADD_RESOURCES2(installer_GUI_SOURCES gui.rc)
 else(MINGW)
   set(installer_GUI_SOURCES ${installer_GUI_SOURCES} gui.rc)
 endif(MINGW)
endif(WIN32)

set(all_sources ${installer_GUI_SOURCES}
                ${installer_GUI_HEADERS}
                ${installer_shared_SOURCES}
                ${installer_shared_HEADERS}
                ${quazip_SOURCES}
                ${quazip_HEADERS}
                ${qua7zip_SOURCES}
                ${qua7zip_HEADERS}
                ${md5_SOURCES}
                ${md5_HEADERS}
        )

source_group(quazip FILES  ${quazip_SOURCES}  ${quazip_HEADERS})
source_group(qua7zip FILES ${qua7zip_SOURCES} ${qua7zip_HEADERS})

qt4_automoc(${all_sources})

add_executable(kdewin-installer-gui ${all_sources})

target_link_libraries(kdewin-installer-gui ${QT_QTMAIN_LIBRARIES} ${QT_QTGUI_LIBRARY} ${QT_QTNETWORK_LIBRARY} ${QT_QTCORE_LIBRARY} ${GUI_LIBS})

install_targets(/bin kdewin-installer-gui )

if (UPX_EXECUTABLE AND CMAKE_PRODUCTION_BUILD)
    add_custom_command(
        TARGET kdewin-installer-gui 
        POST_BUILD 
        COMMAND ${UPX_EXECUTABLE} --lzma -9 "${CMAKE_BINARY_DIR}/gui/kdewin-installer-gui.exe"
    )
endif (UPX_EXECUTABLE AND CMAKE_PRODUCTION_BUILD)
