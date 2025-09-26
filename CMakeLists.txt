cmake_minimum_required(VERSION 3.29)
project(meadow)

if(PROJECT_IS_TOP_LEVEL)
    if(NOT CMAKE_PREFIX_PATH)
        set(CMAKE_PREFIX_PATH ${CMAKE_CURRENT_SOURCE_DIR}/id)
    endif()

    set(CMAKE_FIND_PACKAGE_PREFER_CONFIG ON)
    set(CMAKE_CXX_STANDARD 23)
    set(CMAKE_CXX_STANDARD_REQUIRED ON)

    option(BUILD_TESTING "Build the testing tree." ON)
    if (BUILD_TESTING)
        enable_testing()
        find_package(GTest REQUIRED CONFIG)
    endif()

    set(CMAKE_COMPILE_WARNING_AS_ERROR ON)

    include(${CMAKE_SOURCE_DIR}/cmake/warnings_clang.cmake)
    include(${CMAKE_SOURCE_DIR}/cmake/warnings_gcc.cmake)
    include(${CMAKE_SOURCE_DIR}/cmake/warnings_msvc.cmake)

    set(dependency_default_requirement ON)
else()
    set(dependency_default_requirement AUTO)
endif()

set(dependencies
    ABSL
    EIGEN
    SDL
    SDL_MIXER
)

set(ABSL_PACKAGE_NAME absl)
set(EIGEN_PACKAGE_NAME Eigen3)
set(SDL_PACKAGE_NAME SDL3)
set(SDL_MIXER_PACKAGE_NAME SDL3_mixer)

foreach(dependency IN LISTS dependencies)
    set(MEADOW_WITH_${dependency} ${dependency_default_requirement} CACHE STRING "Build with ${dependency}")
    if(MEADOW_WITH_${dependency} STREQUAL "AUTO")
        find_package(${${dependency}_PACKAGE_NAME} CONFIG)
        if(${${dependency}_PACKAGE_NAME}_FOUND)
            message(STATUS "Dependency ${dependency} is set to AUTO and it is FOUND")
            set(MEADOW_HAS_${dependency} 1)
        else()
            message(STATUS "Dependency ${dependency} is set to AUTO and it is NOT FOUND")
            set(MEADOW_HAS_${dependency} 0)
        endif()
    elseif(MEADOW_WITH_${dependency})
        message(STATUS "Dependency ${dependency} is ON")
        find_package(${${dependency}_PACKAGE_NAME} REQUIRED CONFIG)
        set(MEADOW_HAS_${dependency} 1)
    else()
        message(STATUS "Dependency ${dependency} is OFF")
        set(MEADOW_HAS_${dependency} 0)
    endif()
endforeach()

if(MEADOW_HAS_SDL_MIXER AND NOT MEADOW_HAS_SDL)
    message(FATAL_ERROR "Can't build with SDL_MIXER but without SDL")
endif()

add_subdirectory(src)
if(BUILD_TESTING)
    find_package(magic_enum REQUIRED CONFIG)
    add_subdirectory(testing)
endif()
