project(kdewin-social-installer)
#set(QT_USE_QTWEBKIT TRUE)
set(CMAKE_INCLUDE_CURRENT_DIR ON)
set(TARGET social-installer)
SET(CMAKE_BUILD_TYPE "Debug")
cmake_minimum_required(VERSION 2.6)
#add_definitions( -DQT_NO_CAST_FROM_ASCII )
#add_definitions( -DQT_NO_CAST_TO_ASCII )
#add_definitions(-DQT_USE_FAST_CONCATENATION -DQT_USE_FAST_OPERATOR_PLUS)
#add_definitions(-DCMAKE_BUILD_TYPE=Debug)

set(testUI installerprogresssocial.ui
           softwaredetails.ui
           commentadd.ui
           comment.ui
           settingspage.ui)

set(testSRCS main.cpp
             mainwindow.cpp
             categorybutton.cpp
             softwareitem.cpp
             installerenginesocial.cpp
             installerprogresssocial.cpp
             softwaredetails.cpp
             imagedownloader.cpp
             commentadd.cpp
             comment.cpp
             descriptionbrowser.cpp
             settingspage.cpp)

set(testHDRS mainwindow.h
             categorybutton.h
             softwareitem.h
             installerenginesocial.h
             installerprogresssocial.h
             softwaredetails.h
             imagedownloader.h
             commentadd.h
             comment.h
             descriptionbrowser.h
             settingspage.h)

configure_file(images.qrc.cmake ${CMAKE_CURRENT_BINARY_DIR}/images.qrc)

qt4_wrap_ui(test_UI_SRCS ${testUI})
qt4_wrap_cpp(test_MOC_SRCS ${testHDRS})
qt4_add_resources(test_RC images.qrc)
find_package(Qt4 REQUIRED)
include_directories(
    ${CMAKE_CURRENT_SOURCE_DIR}
    ${CMAKE_CURRENT_BINARY_DIR}
    ${GUI_INCLUDE_DIR}
)
add_definitions(
    ${GUI_DEFINITIONS}
)
INCLUDE(${QT_USE_FILE} ${QT_USE_QTNETWORK})


add_executable(${TARGET} WIN32 ${testSRCS} ${test_MOC_SRCS} ${testUI} ${test_RC})
target_link_libraries(${TARGET}  ${GUI_LIBS}  ${LIBATTICA_LIBRARIES}  ws2_32 ${QT_QTNETWORK_LIBRARY})
install_targets(/bin ${TARGET})

