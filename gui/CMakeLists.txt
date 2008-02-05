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
   dependenciespage.cpp
   downloadpage.cpp
   downloadsettingspage.cpp
   finishpage.cpp
   installdirectorypage.cpp
   installerdialogs.cpp
   installerenginegui.cpp
   installpage.cpp
   installtypepage.cpp
   installwizard.cpp
   internetsettingspage.cpp
   mirrorsettingspage.cpp
   packageselectorpage.cpp
   packagestates.cpp
   settingspage.cpp
   titlepage.cpp
   uninstallpage.cpp
   main.cpp
)

set(installer_GUI_HEADERS
   dependenciespage.h
   downloadpage.h
   downloadsettingspage.h
   finishpage.h
   installdirectorypage.h
   installerdialogs.h
   installerenginegui.h
   installpage.h
   installtypepage.h
   installwizard.h
   internetsettingspage.h
   mirrorsettingspage.h
   packageselectorpage.h
   packagestates.h
   settingspage.h
   titlepage.h
   uninstallpage.h
)

set (UI_SOURCES 
   dependenciespage.ui
   downloadsettingspage.ui
   installdirectorypage.ui
   installtypepage.ui
   internetsettingspage.ui
   mirrorsettingspage.ui
   settingspage.ui
   titlepage.ui
)

include_directories(${CMAKE_CURRENT_SOURCE_DIR} ${CMAKE_CURRENT_BINARY_DIR})

add_definitions(-DUSE_GUI)
if(CMAKE_COMPILER_2005)
     # to avoid a lot of deprecated warnings
     add_definitions( -D_CRT_SECURE_NO_DEPRECATE -D_CRT_NONSTDC_NO_DEPRECATE )
endif(CMAKE_COMPILER_2005)

QT4_WRAP_UI(installer_GUI_SOURCES ${UI_SOURCES})

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
                ${filters_SOURCES}
                ${filters_HEADERS}
                ${bzip2_SOURCES}
                ${bzip2_HEADERS}
        )

source_group(bzip2   FILES ${bzip2_SOURCES}   ${bzip2_HEADERS})
source_group(filters FILES ${filters_SOURCES} ${filters_HEADERS})
source_group(quazip  FILES ${quazip_SOURCES}  ${quazip_HEADERS})
source_group(qua7zip FILES ${qua7zip_SOURCES} ${qua7zip_HEADERS})

qt4_automoc(${all_sources})
add_executable(kdewin-installer-gui-${VERSION} WIN32 ${all_sources})

target_link_libraries(kdewin-installer-gui-${VERSION}
                      ${QT_QTCORE_LIBRARY} ${QT_QTMAIN_LIBRARY}  ${QT_QTGUI_LIBRARY}
                      ${GUI_LIBS}
)

write_file(${EXECUTABLE_OUTPUT_PATH}/kdewin-installer-gui.bat "start kdewin-installer-gui-${VERSION}.exe")

install_targets(/bin kdewin-installer-gui-${VERSION} )
install(FILES ${EXECUTABLE_OUTPUT_PATH}/kdewin-installer-gui.bat DESTINATION bin)

if (PACK_EXECUTABLE)
    if(WIN32)
        add_custom_command(
            TARGET kdewin-installer-gui-${VERSION}
            POST_BUILD
            COMMAND ${UPX_EXECUTABLE} --lzma -9 "${EXECUTABLE_OUTPUT_PATH}/kdewin-installer-gui-${VERSION}.exe"
        )
    else(WIN32)
        add_custom_command(
            TARGET kdewin-installer-gui-${VERSION}
            POST_BUILD
            COMMAND ${UPX_EXECUTABLE} --lzma -9 "${EXECUTABLE_OUTPUT_PATH}/kdewin-installer-gui-${VERSION}"
        )
    endif(WIN32)
endif (PACK_EXECUTABLE)
