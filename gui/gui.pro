TARGET=kde-installer-gui
TEMPLATE=app

HEADERS       = complexwizard.h \
                installwizard.h
#               spinboxdelegate.h 
               
SOURCES       = complexwizard.cpp \
                installwizard.cpp \
                main.cpp
QT += network
CONFIG += release
DEFINES += USE_GUI
DESTDIR = ..\bin
# install

include(../shared/shared.pri)

target.path = $$[QT_INSTALL_EXAMPLES]/dialogs/simplewizard
sources.files = $$SOURCES $$HEADERS *.pro
sources.path = $$[QT_INSTALL_EXAMPLES]/dialogs/simplewizard
INSTALLS += target sources
