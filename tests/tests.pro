TARGET=tests
TEMPLATE=app

HEADERS       =                 
SOURCES       = tests.cpp
QT += network
CONFIG += debug console
DESTDIR = ..\bin
# install

include(../shared/shared.pri)


target.path = $$[QT_INSTALL_EXAMPLES]/dialogs/simplewizard
sources.files = $$SOURCES $$HEADERS *.pro
sources.path = $$[QT_INSTALL_EXAMPLES]/dialogs/simplewizard
INSTALLS += target sources
