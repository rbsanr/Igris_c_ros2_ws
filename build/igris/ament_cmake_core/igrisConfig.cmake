# generated from ament/cmake/core/templates/nameConfig.cmake.in

# prevent multiple inclusion
if(_igris_CONFIG_INCLUDED)
  # ensure to keep the found flag the same
  if(NOT DEFINED igris_FOUND)
    # explicitly set it to FALSE, otherwise CMake will set it to TRUE
    set(igris_FOUND FALSE)
  elseif(NOT igris_FOUND)
    # use separate condition to avoid uninitialized variable warning
    set(igris_FOUND FALSE)
  endif()
  return()
endif()
set(_igris_CONFIG_INCLUDED TRUE)

# output package information
if(NOT igris_FIND_QUIETLY)
  message(STATUS "Found igris: 0.0.1 (${igris_DIR})")
endif()

# warn when using a deprecated package
if(NOT "" STREQUAL "")
  set(_msg "Package 'igris' is deprecated")
  # append custom deprecation text if available
  if(NOT "" STREQUAL "TRUE")
    set(_msg "${_msg} ()")
  endif()
  # optionally quiet the deprecation message
  if(NOT igris_DEPRECATED_QUIET)
    message(DEPRECATION "${_msg}")
  endif()
endif()

# flag package as ament-based to distinguish it after being find_package()-ed
set(igris_FOUND_AMENT_PACKAGE TRUE)

# include all config extra files
set(_extras "")
foreach(_extra ${_extras})
  include("${igris_DIR}/${_extra}")
endforeach()
