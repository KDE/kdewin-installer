#
# find KDEWIN packager. 
# 
# This scripts tries to find the kdewin packager in the following pathes and order 
# 
#    path specified by the environment dir KDEWIN_PACKAGER_DIR
#    <ProgramFiles>/kdewin-packager 
#    <ProgramFiles>/kdewin-installer
#
# Copyright (c) 2006-2007, Ralf Habacker
#
# Redistribution and use is allowed according to the terms of the BSD license.
#

if (WIN32)
    if(NOT KDEWIN_PACKAGER_DIR)
        # check for enviroment variable
    	file(TO_CMAKE_PATH "$ENV{KDEWIN_PACKAGER_DIR}" KDEWIN_PACKAGER_DIR)
    	if(NOT KDEWIN_PACKAGER_DIR)
    		# search in the default program install folder
    		file(TO_CMAKE_PATH "$ENV{PROGRAMFILES}" _progFiles)
    		find_file(KDEWIN_PACKAGER_DIR_tmp kdewin-packager kdewin-installer
       			PATHS
       			"${_progFiles}"
    		)
    		set(KDEWIN_PACKAGER_DIR ${KDEWIN_PACKAGER_DIR_tmp})
    	endif(NOT KDEWIN_PACKAGER_DIR)
    endif(NOT KDEWIN_PACKAGER_DIR)
        
    if (KDEWIN_PACKAGER_DIR)
    
        find_program(KDEWIN_PACKAGER_EXECUTABLE kdewin-packager
            PATHS
                ${KDEWIN_PACKAGER_DIR}/bin
        )
        
       set(KDEWIN_PACKAGER_FOUND TRUE)
    else (KDEWIN_PACKAGER_DIR)
       set(KDEWIN_PACKAGER_FOUND)
    endif (KDEWIN_PACKAGER_DIR)
    
    if (KDEWIN_PACKAGER_FOUND)
      if (NOT KDEWIN_PACKAGER_FIND_QUIETLY)
        message(STATUS "Found KDEWIN Packager: ${KDEWIN_PACKAGER_DIR}")
      endif (NOT KDEWIN_PACKAGER_FIND_QUIETLY)
    else (KDEWIN_PACKAGER_FOUND)
      if (KDEWIN_PACKAGER_FIND_REQUIRED)
        message(FATAL_ERROR "Could NOT find KDEWIN PAckager")
      endif (KDEWIN_PACKAGER_FIND_REQUIRED)
    endif (KDEWIN_PACKAGER_FOUND)

endif (WIN32)

MACRO (KDEWIN_PACKAGER _name _version _notes _options)
    if (KDEWIN_PACKAGER_FOUND)
        if (MSVC)
            set (type "msvc")
        else (MSVC)
            set (type "mingw")
        endif (MSVC)
    
        add_custom_target(kdewin_package
            COMMAND ${CMAKE_COMMAND} 
                -P ${CMAKE_BINARY_DIR}/cmake_install.cmake -DCMAKE_INSTALL_PREFIX=${CMAKE_BINARY_DIR}/temp
            COMMAND ${KDEWIN_PACKAGER_EXECUTABLE} 
                -name ${_name}
                -root ${CMAKE_BINARY_DIR}/temp
                -srcroot ${CMAKE_SOURCE_DIR}
                -version ${_version} 
                -notes "${_notes}"
                -type ${type}
                ${_options}               
            # FIXME: cleanup does not work 
            #COMMAND rmdir /Q /S ${CMAKE_BINARY_DIR}\temp
        )
        add_custom_target(kdewin_package_debug_and_release
            COMMAND ${CMAKE_COMMAND} 
                -H$(CMAKE_SOURCE_DIR) 
                -B$(CMAKE_BINARY_DIR)
                -DCMAKE_BUILD_TYPE=Release
            COMMAND ${CMAKE_MAKE_PROGRAM} 
                clean
            COMMAND ${CMAKE_MAKE_PROGRAM} 
                install/local
            COMMAND ${CMAKE_COMMAND} 
                -H$(CMAKE_SOURCE_DIR) 
                -B$(CMAKE_BINARY_DIR)
                -DCMAKE_BUILD_TYPE=Debug
            COMMAND ${CMAKE_MAKE_PROGRAM} 
                clean
            COMMAND ${CMAKE_MAKE_PROGRAM} 
                install/local
            COMMAND ${KDEWIN_PACKAGER_EXECUTABLE} 
                -name ${_name}
                -root ${CMAKE_BINARY_DIR}/temp
                -srcroot ${CMAKE_SOURCE_DIR}
                -version ${_version} 
                -notes "${_notes}"
                -type ${type}
                ${_options}               
            # FIXME: cleanup does not work 
            #COMMAND rmdir /Q /S ${CMAKE_BINARY_DIR}\temp
        )
    endif (KDEWIN_PACKAGER_FOUND)
ENDMACRO (KDEWIN_PACKAGER)

