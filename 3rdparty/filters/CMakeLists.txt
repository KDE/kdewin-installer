
add_definitions(
    ${QT_DEFINITIONS}
)

include_directories(
    ${QT_INCLUDES}
)


set(FILTERS_INCLUDE_DIR ${CMAKE_SOURCE_DIR}/3rdparty/filters)

set(filters_SOURCES
    ${CMAKE_SOURCE_DIR}/3rdparty/filters/tarfilter.cpp
    ${CMAKE_SOURCE_DIR}/3rdparty/filters/bzip2iodevice.cpp
)
set(filters_HEADERS
    ${CMAKE_SOURCE_DIR}/3rdparty/filters/tarfilter.h
    ${CMAKE_SOURCE_DIR}/3rdparty/filters/bzip2iodevice.h
)

add_library(filters STATIC ${filters_SOURCES} ${filters_HEADERS})
target_link_libraries(filters PRIVATE ${QT_CONSOLE_LIBRARIES} ${BZIP2_LIBRARIES})
get_target_property(FILTERS_LIBRARY filters LOCATION)

# public interface
set(FILTERS_DEFINITIONS ""  CACHE STRING "definitions" FORCE)
set(FILTERS_INCLUDE_DIR ${CMAKE_SOURCE_DIR}/3rdparty/filters CACHE FILEPATH "include directories" FORCE)
set(FILTERS_LIBRARIES ${FILTERS_LIBRARY} ${BZIP2_LIBRARIES} CACHE FILEPATH "libraries" FORCE)
set(FILTERS_FOUND TRUE CACHE BOOL "state" FORCE)
