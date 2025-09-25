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
endif()

option(MEADOW_NO_ABSL "Build without absl" OFF)
if(NOT MEADOW_NO_ABSL)
    find_package(absl REQUIRED CONFIG)
endif()

option(MEADOW_NO_EIGEN "Build without Eigen-dependent parts" OFF)
if(NOT MEADOW_NO_EIGEN)
    find_package(Eigen3 REQUIRED CONFIG)
endif()

option(MEADOW_NO_SDL "Build without SDL-dependent parts" OFF)
if(NOT MEADOW_NO_SDL)
    find_package(SDL3 REQUIRED CONFIG)
endif()

add_subdirectory(src)
if(BUILD_TESTING)
    find_package(magic_enum REQUIRED CONFIG)
    add_subdirectory(testing)
endif()
