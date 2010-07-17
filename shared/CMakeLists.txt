
include_directories(
    ${BZIP2_INCLUDE_DIR}
    ${FILTERS_INCLUDE_DIR}
    ${CURL_INCLUDE_DIR}
    ${QUAZIP_INCLUDE_DIR}
    ${QUA7ZIP_INCLUDE_DIR}
)

set(installer_shared_HEADERS
    database.h
    debug.h
    downloader.h
    downloader_p.h
    downloaderprogress.h
    externalinstallercontrol.h
    globalconfig.h
    hash.h
    hashfile.h
    hintfile.h
    installer.h
    installerengine.h
    installerprogress.h
    installerupdate.h
    package.h
    packagecategorycache.h
    packagelist.h
    qio.h
    site.h
    mirrors.h
    misc.h
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
    hintfile.cpp
    installer.cpp
    installerengine.cpp
    installerprogress.cpp
    installerupdate.cpp
    mirrors.cpp
    misc.cpp
    package.cpp
    packagecategorycache.cpp
    packagelist.cpp
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

qt4_wrap_cpp(shared_sources_console ${installer_shared_HEADERS})
qt4_wrap_cpp(shared_sources_gui ${installer_shared_HEADERS})

if(NOT BUILD_INSTALLER_THEME)
add_library(shared-console STATIC ${shared_sources_console})
set_target_properties(shared-console  PROPERTIES COMPILE_FLAGS "${QT_DEFINITIONS} ${BZIP2_DEFINITIONS} ${CURL_DEFINITIONS} ${FILTERS_DEFINITIONS} ${QUAZIP_DEFINITIONS} ${QUA7ZIP_DEFINITIONS}")
target_link_libraries(shared-console ${QT_LIBRARIES} ${BZIP2_LIBRARIES} ${CURL_LIBRARIES} ${FILTERS_LIBRARIES} ${QUAZIP_LIBRARIES} ${QUA7ZIP_LIBRARIES} kernel32 user32 shell32 uuid ole32 advapi32 ws2_32 shlwapi psapi)
endif(NOT BUILD_INSTALLER_THEME)

add_library(shared-gui STATIC ${shared_sources_gui})
set_target_properties(shared-gui PROPERTIES COMPILE_FLAGS "-DUSE_GUI ${QT_DEFINITIONS} ${BZIP2_DEFINITIONS} ${CURL_DEFINITIONS} ${FILTERS_DEFINITIONS} ${QUAZIP_DEFINITIONS} ${QUA7ZIP_DEFINITIONS}")
target_link_libraries(shared-gui ${QT_LIBRARIES} ${BZIP2_LIBRARIES} ${CURL_LIBRARIES} ${FILTERS_LIBRARIES} ${QUAZIP_LIBRARIES} ${QUA7ZIP_LIBRARIES} kernel32 user32 shell32 uuid ole32 advapi32 ws2_32 shlwapi psapi)
