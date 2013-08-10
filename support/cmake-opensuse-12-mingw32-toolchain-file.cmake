#
# cmake toolchain file for opensuse 12.x mingw32
# 
set (_target i686-w64-mingw32)
set (cross_root /usr/${_target}/sys-root/mingw)

set (CMAKE_SYSTEM_NAME "Windows")

set (CMAKE_C_COMPILER /usr/bin/${_target}-gcc)
set (CMAKE_CXX_COMPILER /usr/bin/${_target}-g++)
set (CMAKE_C_FLAGS "-pipe -Wall -fexceptions -fno-omit-frame-pointer -fno-optimize-sibling-calls --param=ssp-buffer-size=4 -mms-bitfields")
set (CMAKE_CXX_FLAGS "-pipe -Wall -fexceptions -fno-omit-frame-pointer -fno-optimize-sibling-calls --param=ssp-buffer-size=4 -mms-bitfields")

set (INCLUDE_INSTALL_DIR ${cross_root}/include )
set (LIB_INSTALL_DIR ${cross_root}/lib)
set (CMAKE_FIND_ROOT_PATH ${cross_root})
set (CMAKE_FIND_ROOT_PATH_MODE_LIBRARY ONLY)
set (CMAKE_FIND_ROOT_PATH_MODE_INCLUDE=ONLY)
set (CMAKE_FIND_ROOT_PATH_MODE_PROGRAM=NEVER)
