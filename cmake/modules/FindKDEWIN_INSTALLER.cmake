# 
# Copyright (c) 2007, Ralf Habacker
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
        
        MESSAGE(${KDEWIN_PACKAGER})
       set(KDEWIN_INSTALLER_FOUND TRUE)
    else (KDEWIN_INSTALLER_DIR)
       set(KDEWIN_INSTALLER_FOUND)
    endif (KDEWIN_INSTALLER_DIR)
    
    if (KDEWIN_INSTALLER_FOUND)
      if (NOT KDEWIN_INSTALLER_FIND_QUIETLY)
        message(STATUS "Found KDEWIN_INSTALLER: ${KDEWIN_INSTALLER_DIR}")
      endif (NOT KDEWIN_INSTALLER_FIND_QUIETLY)
    else (KDEWIN_INSTALLER_FOUND)
      if (KDEWIN_INSTALLER_FIND_REQUIRED)
        message(FATAL_ERROR "Could NOT find KDEWIN_INSTALLER")
      endif (KDEWIN_INSTALLER_FIND_REQUIRED)
    endif (KDEWIN_INSTALLER_FOUND)

endif (WIN32)

