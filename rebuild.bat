
set QT_VERSION=4.7.3
set /P QT_VERSION=Qt Version [4.7.3]: %=%
mingw32-make clean -j
del CMakeCache.txt
cmake -G"MinGW Makefiles" -DCMAKE_LIBRARY_PATH=C:\users\tudalex\attica\install\lib -DCMAKE_INCLUDE_PATH=C:\users\tudalex\attica\install\include -DQT_QMAKE_EXECUTABLE=C:\QtSDK\Desktop\Qt\%QT_VERSION%\mingw\bin\qmake.exe
mingw32-make