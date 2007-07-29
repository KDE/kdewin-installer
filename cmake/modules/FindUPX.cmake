#
# find UPX 
# 
# Copyright (c) 2006, Ralf Habacker
#
# Redistribution and use is allowed according to the terms of the BSD license.
#

if (WIN32)
    if(NOT UPX_EXECUTABLE)
    	file(TO_CMAKE_PATH "$ENV{PROGRAMFILES}" _progFiles)
        find_program(UPX_EXECUTABLE upx
            PATHS
                ${_progFiles}/upx
                ${_progFiles}/upx/bin
        )

        if (UPX_EXECUTABLE)        
            set(UPX_FOUND TRUE)
            execute_process(
                COMMAND ${UPX_EXECUTABLE} -V
                OUTPUT_VARIABLE output
                ERROR_VARIABLE output
            )
            STRING(REGEX MATCH "upx [0-9]+\\.[0-9]+" upx_version "${output}")
            STRING(REGEX MATCH "LZMA [a-zA-Z ]+[0-9]+\\.[0-9]+"  UPX_LZMA_VERSION "${output}")
        endif (UPX_EXECUTABLE)        
    else(NOT UPX_EXECUTABLE)
       set(UPX_FOUND)
    endif(NOT UPX_EXECUTABLE)
    
    if (UPX_FOUND)
      if (NOT UPX_FIND_QUIETLY)
        message(STATUS "Found ${upx_version} ${UPX_LZMA_VERSION} ")
      endif (NOT UPX_FIND_QUIETLY)
    else (UPX_FOUND)
      if (UPX_FIND_REQUIRED)
        message(FATAL_ERROR "Could NOT find UPX program")
      endif (UPX_FIND_REQUIRED)
    endif (UPX_FOUND)
endif (WIN32)

