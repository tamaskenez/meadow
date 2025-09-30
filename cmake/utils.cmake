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