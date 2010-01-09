set (DESKTOP_APPNAME "KDE Installer")
set (TARGET_FILENAME kdewin-installer-gui-${VERSION_PATCH})

configure_file(gui.qrc.cmake ${CMAKE_CURRENT_BINARY_DIR}/gui.qrc)

add_themed_installer(kdewin ${CMAKE_CURRENT_BINARY_DIR}/gui.qrc)
configure_file(installer.desktop.cmake ${CMAKE_CURRENT_BINARY_DIR}/installer.desktop)

install(FILES ${CMAKE_CURRENT_BINARY_DIR}/installer.desktop DESTINATION share/applications/kde4)