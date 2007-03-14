find_package(Doxygen)


set (top_srcdir ${CMAKE_SOURCE_DIR}/..)
set (INPUT "${CMAKE_SOURCE_DIR}/shared ${CMAKE_SOURCE_DIR}/gui ${CMAKE_SOURCE_DIR}/console ${CMAKE_SOURCE_DIR}/tests")
configure_file(${CMAKE_SOURCE_DIR}/doc/installer.doxy.cmake ${CMAKE_BINARY_DIR}/doc/Doxyfile )
add_custom_target(doc 
    COMMAND ${DOXYGEN_EXECUTABLE} ${CMAKE_BINARY_DIR}/doc/Doxyfile
)
