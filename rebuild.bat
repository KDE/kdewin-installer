mingw32-make clean -j
del CMakeCache.txt
cmake -G"MinGW Makefiles" -DCMAKE_LIBRARY_PATH=C:\users\tudalex\attica\install\lib -DCMAKE_INCLUDE_PATH=C:\users\tudalex\attica\install\include
mingw32-make