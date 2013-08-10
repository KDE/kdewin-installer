
add_definitions(
    ${BZIP2_DEFINITIONS}
    ${QT_DEFINITIONS}
)

include_directories(
    ${BZIP2_INCLUDE_DIR}
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
target_link_libraries(filters)
get_target_property(FILTERS_LIBRARY filters LOCATION)

# public interface
set(FILTERS_DEFINITIONS ""  CACHE STRING "definitions" FORCE)
set(FILTERS_INCLUDE_DIR ${CMAKE_SOURCE_DIR}/3rdparty/filters CACHE FILEPATH "include directories" FORCE)
set(FILTERS_LIBRARIES ${FILTERS_LIBRARY} ${BZIP2_LIBRARIES} CACHE FILEPATH "libraries" FORCE)
set(FILTERS_FOUND TRUE CACHE BOOL "state" FORCE)
