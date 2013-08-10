Linux cross compiling using mingw32 packages in shared mode
===========================================================

1. install at least the following packages:

   git
   cmake >= 2.8.8
   mingw32 cross g++ (4.7 or 4.8)
   mingw32 binutils 
   mingw32 qt4 with development package
   zlib with development package
   bzip2 with development package 
   curl with development package

2. checkout kdewin-installer sources 

   cd <source-root>
   git clone kde:kdewin-installer
    
2. run 
   mkdir kdewin-installer-cross-build
   cd kdewin-installer-cross-build
   ln -s ../kdewin-installer/support/windres-i686-w64-mingw32 windres
   wine regedit /S ../kdewin-installer/support/wine-i686-w64-mingw32.reg 
   
3. on opensuse 12 run 
   PATH=$PWD:$PATH cmake -DQTDIR=/usr/i686-w64-mingw32/ -DCMAKE_TOOLCHAIN_FILE=../kdewin-installer/support/cmake-opensuse-12-mingw32-toolchain-file.cmake -DCMAKE_BUILD_TYPE=Debug ../kdewin-installer  

Note: for other distributions you need to adapt the pathes

Ralf Habacker
