add_definitions(
    ${BZIP2_DEFINITIONS}
    ${CURL_DEFINITIONS}
    ${FILTERS_DEFINITIONS}
    ${QUAZIP_DEFINITIONS}
    ${QUA7ZIP_DEFINITIONS}
    ${QT_DEFINITIONS}
)

include_directories(
    ${BZIP2_INCLUDE_DIR}
    ${CURL_INCLUDE_DIR}
    ${FILTERS_INCLUDE_DIR}
    ${QUAZIP_INCLUDE_DIR}
    ${QUA7ZIP_INCLUDE_DIR}
    ${QT_INCLUDES}
)

set(installer_shared_HEADERS
    database.h
    debug.h
    downloader.h
    downloaderprogress.h
    externalinstallercontrol.h
    globalconfig.h
    hash.h
    hashfile.h
    installer.h
    installercallconfig.h
    installerengine.h
    installerprogress.h
    installerupdate.h
    package.h
    packagecategorycache.h
    packageinfo.h
    packagelist.h
    packagestates.h
    postprocessing.h
    proxysettings.h
    qio.h
    site.h
    mirrors.h
    misc.h
	typehelper.h
    registry.h
    releases.h
    selfinstaller.h
    settings.h
    uninstaller.h
    uninstaller_p.h
    unpacker.h
    unpacker_p.h
#    installerbase.h
)

set(installer_shared_SOURCES
    database.cpp
    debug.cpp
    downloader.cpp
    downloaderprogress.cpp
    externalinstallercontrol.cpp
    globalconfig.cpp
    hash.cpp
    hashfile.cpp
    installer.cpp
    installercallconfig.cpp
    installerengine.cpp
    installerprogress.cpp
    installerupdate.cpp
    mirrors.cpp
    misc.cpp
	typehelper.cpp
    package.cpp
    packagecategorycache.cpp
    packageinfo.cpp
    packagelist.cpp
    packagestates.cpp
    postprocessing.cpp
    proxysettings.cpp
    qio.cpp
    site.cpp
    selfinstaller.cpp
    registry.cpp
    releases.cpp
    settings.cpp
    uninstaller.cpp
    unpacker.cpp
#    installerbase.cpp
)

set(installer_shared_console_SOURCES
    ${installer_shared_SOURCES}
)

set(installer_shared_gui_SOURCES
    ${installer_shared_SOURCES}
)

set (shared_sources_console ${installer_shared_console_SOURCES} ${installer_shared_HEADERS})
set (shared_sources_gui ${installer_shared_gui_SOURCES} ${installer_shared_HEADERS})

if(QT5_FOUND)
  qt5_wrap_cpp(shared_sources_console ${installer_shared_HEADERS})
  qt5_wrap_cpp(shared_sources_gui ${installer_shared_HEADERS})
endif()

if(NOT BUILD_INSTALLER_THEME)
add_library(shared-console STATIC ${shared_sources_console})
target_link_libraries(shared-console ${QT_CONSOLE_LIBRARIES} ${BZIP2_LIBRARIES} ${CURL_LIBRARIES} ${FILTERS_LIBRARIES} ${QUAZIP_LIBRARIES} ${QUA7ZIP_LIBRARIES})
if(WIN32)
    target_link_libraries(shared-console kernel32 user32 shell32 uuid ole32 advapi32 ws2_32 shlwapi psapi)
endif(WIN32)
endif(NOT BUILD_INSTALLER_THEME)

if (NOT BUILD_PACKAGER_ONLY)
add_library(shared-gui STATIC ${shared_sources_gui})
set_target_properties(shared-gui PROPERTIES COMPILE_FLAGS "-DUSE_GUI")
target_link_libraries(shared-gui ${QT_GUI_LIBRARIES} ${BZIP2_LIBRARIES} ${CURL_LIBRARIES} ${FILTERS_LIBRARIES} ${QUAZIP_LIBRARIES} ${QUA7ZIP_LIBRARIES})
if(WIN32)
    target_link_libraries(shared-gui kernel32 user32 shell32 uuid ole32 advapi32 ws2_32 shlwapi psapi)
endif(WIN32)
endif()
