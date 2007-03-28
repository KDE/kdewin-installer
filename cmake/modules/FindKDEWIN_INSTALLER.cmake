#
# find KDEWIN installer. 
# 
# This scripts supports the following options 
# 
#  -  install KDEWIN installer in <default application dir>/kdewin-installer
#  -  set the environment variable KDEWIN_INSTALLER_DIR=<path>
#  -  add -DKDEWIN_INSTALLER_DIR=<path> to the cmake command line 
#
# Copyright (c) 2006, Ralf Habacker
#
# Redistribution and use is allowed according to the terms of the BSD license.
#

if (WIN32)
    if(NOT KDEWIN_INSTALLER_DIR)
        # check for enviroment variable
    	file(TO_CMAKE_PATH "$ENV{KDEWIN_INSTALLER_DIR}" KDEWIN_INSTALLER_DIR)
    	if(NOT KDEWIN_INSTALLER_DIR)
    		# search in the default program install folder
    		file(TO_CMAKE_PATH "$ENV{PROGRAMFILES}" _progFiles)
    		find_file(KDEWIN_INSTALLER_DIR_tmp kdewin-installer
       			PATHS
       			"${_progFiles}"
    		)
    		set(KDEWIN_INSTALLER_DIR ${KDEWIN_INSTALLER_DIR_tmp})
    	endif(NOT KDEWIN_INSTALLER_DIR)
    endif(NOT KDEWIN_INSTALLER_DIR)
        
    if (KDEWIN_INSTALLER_DIR)
    
        find_program(KDEWIN_PACKAGER kdewin-packager
            PATHS
                ${KDEWIN_INSTALLER_DIR}/bin
        )
        
       set(KDEWIN_INSTALLER_FOUND TRUE)
    else (KDEWIN_INSTALLER_DIR)
       set(KDEWIN_INSTALLER_FOUND)
    endif (KDEWIN_INSTALLER_DIR)
    
    if (KDEWIN_INSTALLER_FOUND)
      if (NOT KDEWIN_INSTALLER_FIND_QUIETLY)
        message(STATUS "Found KDEWIN Installer: ${KDEWIN_INSTALLER_DIR}")
      endif (NOT KDEWIN_INSTALLER_FIND_QUIETLY)
    else (KDEWIN_INSTALLER_FOUND)
      if (KDEWIN_INSTALLER_FIND_REQUIRED)
        message(FATAL_ERROR "Could NOT find KDEWIN Installer")
      endif (KDEWIN_INSTALLER_FIND_REQUIRED)
    endif (KDEWIN_INSTALLER_FOUND)

endif (WIN32)

