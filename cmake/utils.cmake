function(list_targets ARG_DIRECTORY)
  if(NOT ARG_DIRECTORY)
    set(ARG_DIRECTORY ${CMAKE_CURRENT_SOURCE_DIR})
  endif()
  
  # Set property of targets in this subdir.
  get_property(targets DIRECTORY ${ARG_DIRECTORY} PROPERTY BUILDSYSTEM_TARGETS)
  foreach(target IN LISTS targets)
    message(STATUS "${target} in ${ARG_DIRECTORY}")
  endforeach()
  
  # Recurse subdirectories.
  get_property(subdirs DIRECTORY ${ARG_DIRECTORY} PROPERTY SUBDIRECTORIES)
  foreach(subdir IN LISTS subdirs)
    ListTargets(${subdir})
  endforeach()
endfunction()

function(print_target_properties target)
  # Parse optional CONFIGS argument
  cmake_parse_arguments(ARG "" "" "CONFIGS" ${ARGN})

  if(NOT TARGET ${target})
    message(FATAL_ERROR "print_target_properties: '${target}' is not a target")
    return()
  endif()

  execute_process(COMMAND ${CMAKE_COMMAND} --help-property-list
          OUTPUT_VARIABLE _properties
          OUTPUT_STRIP_TRAILING_WHITESPACE
  )
  string(REPLACE "\n" ";" _properties "${_properties}")

  message(STATUS "=== Properties of target: ${target} ===")
  list(REMOVE_DUPLICATES _properties)
  foreach(_prop IN LISTS _properties)
    if(_prop MATCHES "_<CONFIG>$")
      # Expand config-suffixed properties if configs were provided
      if(ARG_CONFIGS)
        foreach(_cfg IN LISTS ARG_CONFIGS)
          string(TOUPPER "${_cfg}" _cfg_upper)
          string(REPLACE "<CONFIG>" "${_cfg_upper}" _resolved "${_prop}")
          get_target_property(_val ${target} ${_resolved})
          if((NOT _val STREQUAL "${target}-NOTFOUND") AND (NOT _val STREQUAL "_val-NOTFOUND"))
            message(STATUS "  ${_resolved} = ${_val}")
          endif()
        endforeach()
      endif()
    elseif(_prop MATCHES "<")
      # Skip other parameterized properties
      continue()
    else()
      get_target_property(_val ${target} ${_prop})
      if((NOT _val STREQUAL "${target}-NOTFOUND") AND (NOT _val STREQUAL "_val-NOTFOUND"))
        message(STATUS "  ${_prop} = ${_val}")
      endif()
    endif()
  endforeach()
  message(STATUS "=== End of properties for: ${target} ===")
endfunction()
