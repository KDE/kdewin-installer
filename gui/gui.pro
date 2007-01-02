TARGET=kde-installer-gui
TEMPLATE=app

HEADERS       = ..\shared\package.h \
                ..\shared\packagelist.h \
                ..\shared\downloader.h \
                ..\shared\installer.h \
                complexwizard.h \
                 installwizard.h
#                 spinboxdelegate.h 
               
SOURCES       = ..\shared\package.cpp \
                ..\shared\packagelist.cpp \
                ..\shared\downloader.cpp \
                ..\shared\installer.cpp \
                complexwizard.cpp \
                installwizard.cpp \
                main.cpp
QT += network
CONFIG += debug 
DEFINES += USE_GUI
INCLUDEPATH += ..\shared
DESTDIR = ..\bin
# install

target.path = $$[QT_INSTALL_EXAMPLES]/dialogs/simplewizard
sources.files = $$SOURCES $$HEADERS *.pro
sources.path = $$[QT_INSTALL_EXAMPLES]/dialogs/simplewizard
INSTALLS += target sources
