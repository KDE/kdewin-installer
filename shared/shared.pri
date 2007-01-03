DEFINES += USE_EXTERNAL_ZIP DEBUG

# uncomment for including doc and source packages
#DEFINES += INCLUDE_DOC_AND_SRC_PACKAGES


SOURCES += ..\shared\package.cpp \
           ..\shared\packagelist.cpp \
           ..\shared\downloader.cpp \
           ..\shared\installer.cpp \

!contains( DEFINES, USE_EXTERNAL_ZIP ) {
  SOURCES += ..\shared\unzip.c
}

HEADERS += ..\shared\package.h \
           ..\shared\packagelist.h \
           ..\shared\downloader.h \
           ..\shared\installer.h

INCLUDEPATH += ..\shared 

!contains( DEFINES, USE_EXTERNAL_ZIP ) {
  INCLUDEPATH += ..\unzip-5.51-1-src\src\unzip\5.51\unzip-5.51
  LIBS += -L..\unzip-5.51-1-src\src\unzip\5.51\unzip-5.51 -lunzip32
}
