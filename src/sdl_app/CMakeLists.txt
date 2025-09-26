file(GLOB_RECURSE sources CONFIGURE_DEPENDS *.h *.cpp)
source_group(TREE ${CMAKE_CURRENT_SOURCE_DIR} FILES ${sources})

add_library(sdl_app STATIC ${sources})
add_library(meadow::sdl_app ALIAS sdl_app)

target_include_directories(sdl_app
    PUBLIC $<BUILD_INTERFACE:${CMAKE_CURRENT_SOURCE_DIR}>/include
)

target_link_libraries(sdl_app PUBLIC
    SDL3::SDL3
)

get_target_property(sdl_include_dirs SDL3::Headers INTERFACE_INCLUDE_DIRECTORIES)
target_include_directories(sdl_app SYSTEM PRIVATE ${sdl_include_dirs})
