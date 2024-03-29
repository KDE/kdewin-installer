#
# win32 macros 
# 
# Copyright (c) 2006-2007, Ralf Habacker <ralf.habacker@freenet.de>
#
# SPDX-License-Identifier: BSD-3-Clause
#

if (WIN32)
    #
    # addExplorerWrapper creates batch files for fast access 
    # to the build environment from the win32 explorer. 
    # 
    # For mingw and nmake projects it's opens a command shell,
    # for Visual Studio IDE's (at least tested with VS 8 2005) it
    # opens the related .sln file with paths setting specified at 
    # configure time. 
    #
    # syntax: 
    #   addExplorerWrapper()             - uses projectname set by project()    
    #   addExplorerWrapper(project-name) - create wrapper with specific project-name 
    #
    # This macro requires FindPSDK.cmake for detecting platform sdk installation
    #
    MACRO (addExplorerWrapper)
        set (_projectname ${arg1})
        if (NOT _projectname)
            set (_projectname ${CMAKE_PROJECT_NAME})
        endif (NOT _projectname)
        if (NOT _projectname)
            set (_projectname "default")
        endif (NOT _projectname)

        set (SCRIPT_NAME "${CMAKE_BINARY_DIR}/${_projectname}-shell.bat")

        # write explorer wrappers
        get_filename_component(CMAKE_BIN_PATH ${CMAKE_COMMAND} PATH)
        set (ADD_PATH "${CMAKE_BIN_PATH}")

        if (QT_QMAKE_EXECUTABLE)
            get_filename_component(QT_BIN_PATH ${QT_QMAKE_EXECUTABLE} PATH)
            set (ADD_PATH "${ADD_PATH};${QT_BIN_PATH}")
        endif (QT_QMAKE_EXECUTABLE)
        
        # add here more pathes 
        
        if (MINGW)
            get_filename_component(MINGW_BIN_PATH ${CMAKE_CXX_COMPILER} PATH)
            set (ADD_PATH "${ADD_PATH};${MINGW_BIN_PATH}")
            write_file (${SCRIPT_NAME} "set PATH=${ADD_PATH};%PATH%\ncmd.exe")
        else (MINGW)
            find_package(PSDK)
            if (PSDK_INIT_SCRIPT)
                set (PSDK_INIT_CALL "call \"${PSDK_INIT_SCRIPT}\"\n")
            endif (PSDK_INIT_SCRIPT)

            set (VS80COMNTOOLS "$ENV{VS80COMNTOOLS}")
            if (VS80COMNTOOLS)
                set (VC_INIT_CALL "call \"${VS80COMNTOOLS}/vsvars32.bat\"\n")
            else (VS80COMNTOOLS)
                set (VC_INIT_CALL "call \"${VC_BIN_PATH}/vcvars32.bat\n")
            endif (VS80COMNTOOLS)

            if (CMAKE_BUILD_TOOL STREQUAL  "nmake")
                get_filename_component(VC_BIN_PATH ${CMAKE_CXX_COMPILER} PATH)
                write_file (${SCRIPT_NAME} "set PATH=${ADD_PATH};%PATH%\n${VC_INIT_CALL}${PSDK_INIT_CALL}cmd.exe")
            else (CMAKE_BUILD_TOOL STREQUAL  "nmake")
                write_file (${SCRIPT_NAME} "set PATH=${ADD_PATH};%PATH%\n${PSDK_INIT_CALL}start ${_projectname}.sln")
            endif (CMAKE_BUILD_TOOL STREQUAL  "nmake")
        endif (MINGW)
    ENDMACRO (addExplorerWrapper)
endif(WIN32)
