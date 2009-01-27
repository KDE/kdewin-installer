# WINSCP tool support
# 
# Note: This package is in prelimary state and not usable generic
# 
# Copyright (c) 2006-2007, Ralf Habacker
#
# Redistribution and use is allowed according to the terms of the BSD license.
#

if (WIN32)
    if(NOT WINSCP_DIR)
        # check for enviroment variable
    	file(TO_CMAKE_PATH "$ENV{WINSCP_DIR}" WINSCP_DIR)
    	if(NOT WINSCP_DIR)
    		# search in the default program install folder
    		file(TO_CMAKE_PATH "$ENV{PROGRAMFILES}" _progFiles)
    		find_file(WINSCP_DIR_tmp winscp3 winscp
       			PATHS
       			"${_progFiles}"
    		)
    		set(WINSCP_DIR ${WINSCP_DIR_tmp})
    	endif(NOT WINSCP_DIR)
    endif(NOT WINSCP_DIR)
        
    if (WINSCP_DIR)
    
        find_program(WINSCP_EXECUTABLE winSCP.exe winscp3.com 
            PATHS
                ${WINSCP_DIR}
        )
        
       set(WINSCP_FOUND TRUE)
    else (WINSCP_DIR)
       set(WINSCP_FOUND)
    endif (WINSCP_DIR)
    
    if (WINSCP_FOUND)
      if (NOT WINSCP_FIND_QUIETLY)
        message(STATUS "Found WinSCP: ${WINSCP_DIR}")
      endif (NOT WINSCP_FIND_QUIETLY)
    else (WINSCP_FOUND)
      if (WINSCP_FIND_REQUIRED)
        message(FATAL_ERROR "Could NOT find WINSCP")
      endif (WINSCP_FIND_REQUIRED)
    endif (WINSCP_FOUND)

endif (WIN32)

MACRO (UPLOAD)
    if (WINSCP_FOUND)
set (CONFIG_FILE_TEXT "
# Automatically answer all prompts negatively not to stall
# the script on errors
option batch on
# Disable overwrite confirmations that conflict with the previous
option confirm off
# Connect
open ralf@www.winkde.org
# Change remote directory
cd /pub/kde/ports/win32/installer/
# Force binary mode transfer
option transfer binary
# upload files 
put kdewin-installer*.zip 
cd /home/kde-windows/kde-windows/installer
# Force binary mode transfer
option transfer binary
# upload files 
lcd bin
put kdewin-installer-gui-*.exe
# Disconnect
close
# Exit WinSCP
exit
") 
        write_file("winscp-config.txt" ${CONFIG_FILE_TEXT})
        add_custom_target(upload
            # using DEPENDS does not work it results in an nmake error:
            #   NMAKE : fatal error U1073: don't know how to make 'kdewin_package_without_compiler'
            # this is because for the upload target a separate makefile is created 
            # which does not know anything about other targets
            COMMAND ${CMAKE_MAKE_PROGRAM} 
                kdewin_package_without_compiler
            COMMAND ${WINSCP_EXECUTABLE} 
                /console 
                /script=winscp-config.txt
#           DEPENDS kdewin_package_without_compiler
        )
        endif (WINSCP_FOUND)
ENDMACRO (UPLOAD)

