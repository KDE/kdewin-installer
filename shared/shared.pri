DEFINES += DEBUG
#USE_EXTERNAL_ZIP 
# uncomment for including doc and source packages
#DEFINES += INCLUDE_DOC_AND_SRC_PACKAGES


SOURCES += ..\shared\package.cpp \
           ..\shared\packagelist.cpp \
           ..\shared\downloader.cpp \
           ..\shared\installer.cpp \

HEADERS += ..\shared\package.h \
           ..\shared\packagelist.h \
           ..\shared\downloader.h \
           ..\shared\installer.h

INCLUDEPATH += ..\shared 

!contains( DEFINES, USE_EXTERNAL_ZIP ) {
  INCLUDEPATH += ..\..\3rdparty\quazip ..\..\3rdparty\zlib
  LIBS += -L..\..\3rdparty\quazip\quazip\release -lquazip
}
