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
target_include_directories(filters PUBLIC ${CMAKE_SOURCE_DIR}/3rdparty/filters)
# public interface
set(FILTERS_LIBRARIES filters CACHE FILEPATH "libraries" FORCE)
