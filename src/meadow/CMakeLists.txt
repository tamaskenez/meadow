include(CheckCXXSymbolExists)
check_cxx_symbol_exists(cyl_bessel_i cmath HAVE_CYL_BESSEL_I)

file(GLOB_RECURSE sources CONFIGURE_DEPENDS *.h *.cpp)
source_group(TREE ${CMAKE_CURRENT_SOURCE_DIR} FILES ${sources})

if(NOT MEADOW_HAS_SDL)
    list(FILTER sources EXCLUDE REGEX "^sdl\\.*$")
endif()

add_library(meadow STATIC ${sources})
add_library(meadow::meadow ALIAS meadow)

target_precompile_headers(meadow
    PRIVATE pch.h
)
target_include_directories(meadow
    PUBLIC $<BUILD_INTERFACE:${CMAKE_CURRENT_SOURCE_DIR}>/include
)

if(HAVE_CYL_BESSEL_I)
    target_compile_definitions(meadow PUBLIC MEADOW_HAS_CYL_BESSEL_I=1)
else()
    target_compile_definitions(meadow PUBLIC MEADOW_HAS_CYL_BESSEL_I=0)
    if(MEADOW_HAS_BOOST)
        target_link_libraries(meadow PRIVATE
            Boost::math
        )
    endif()
endif()

if(MEADOW_HAS_EIGEN)
    target_link_libraries(meadow PRIVATE
        Eigen3::Eigen
    )
endif()

if(MEADOW_HAS_ABSL)
    target_link_libraries(meadow PUBLIC
        absl::cleanup
        absl::log
        absl::check
    )
endif()

if(MEADOW_HAS_SDL)
    target_link_libraries(meadow PUBLIC
        SDL3::SDL3
    )
    get_target_property(sdl_include_dirs SDL3::Headers INTERFACE_INCLUDE_DIRECTORIES)
    target_include_directories(meadow SYSTEM PRIVATE ${sdl_include_dirs})
endif()

if(MEADOW_HAS_SDL_MIXER)
    target_link_libraries(meadow PUBLIC
        SDL3_mixer::SDL3_mixer
    )
endif()

foreach(dependency IN LISTS dependencies)
    if(MEADOW_HAS_${dependency})
        target_compile_definitions(meadow PUBLIC MEADOW_HAS_${dependency}=1)
    else()
        target_compile_definitions(meadow PUBLIC MEADOW_HAS_${dependency}=0)
    endif()
endforeach()
