# QUAZIP_FOUND - system has the QUAZIP library
# QUAZIP_INCLUDE_DIR - the QUAZIP include directory
# QUAZIP_LIBRARIES - The libraries needed to use QUAZIP

if(NOT QUAZIP_INCLUDE_DIR)
  find_path(QUAZIP_INCLUDE_DIR NAMES quazip.h PATH_SUFFIXES quazip)
  find_library(QUAZIP_LIBRARIES NAMES quazip)

  include(FindPackageHandleStandardArgs)
  find_package_handle_standard_args(QUAZIP DEFAULT_MSG QUAZIP_INCLUDE_DIR QUAZIP_LIBRARIES)

  mark_as_advanced(QUAZIP_INCLUDE_DIR QUAZIP_LIBRARIES)
endif()

if (QUAZIP_FOUND)
    add_library(quazip UNKNOWN IMPORTED)
    set_target_properties(quazip PROPERTIES INTERFACE_INCLUDE_DIRECTORIES "${QUAZIP_INCLUDE_DIR}")
    set_target_properties(quazip PROPERTIES IMPORTED_LOCATION "${QUAZIP_LIBRARIES}")
endif()
