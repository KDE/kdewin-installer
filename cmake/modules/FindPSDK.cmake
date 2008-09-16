# Find the Platform SDK installation 
# This module defines
#  PSDK_DIR, where to find Platform SDK Installation
#  PSDK_INCLUDE_DIR, where the headers are located
#  PSDK_INIT_SCRIPT, where the environment init scripts is located 
#  PSDK_FOUND, If false, do not try to use PSDK
#
# Copyright (c) 2008, Ralf Habacker, <ralf.habacker@freenet.de>
#
# Redistribution and use is allowed according to the terms of the BSD license.
# For details see the accompanying COPYING-CMAKE-SCRIPTS file.

if(PSDK_INCLUDE_DIR)
   set(PSDK_FOUND TRUE)

else(PSDK_INCLUDE_DIR)
    FIND_PATH(PSDK_DIR include/WinBase.h
        "$ENV{ProgramFiles}/Microsoft Platform SDK for Windows Server 2003 R2"
        "$ENV{ProgramFiles}/Microsoft Platform SDK for Windows Server 2003 R1"
        "$ENV{MSSdk}"
    )
    
  set (PSDK_INCLUDE_DIR ${PSDK_DIR}/include)
  set (PSDK_INIT_SCRIPT ${PSDK_DIR}/SetEnv.cmd)
  
  if(PSDK_INCLUDE_DIR)
    set(PSDK_FOUND TRUE)
    message(STATUS "Found PSDK: ${PSDK_DIR} ")
    #INCLUDE_DIRECTORIES(${PSDK_INCLUDE_DIR})
  else(PSDK_INCLUDE_DIR)
    set(PSDK_FOUND FALSE)
    message(STATUS "PSDK not found.")
  endif(PSDK_INCLUDE_DIR)

    MARK_AS_ADVANCED(PSDK_DIR PSDK_INCLUDE_DIR)
endif(PSDK_INCLUDE_DIR)
