file(GLOB_RECURSE sources CONFIGURE_DEPENDS *.h *.cpp)
source_group(TREE ${CMAKE_CURRENT_SOURCE_DIR} FILES ${sources})
add_library(meadow STATIC ${sources})
target_precompile_headers(meadow
    PRIVATE pch.h
)
target_include_directories(meadow
    PUBLIC $<BUILD_INTERFACE:${CMAKE_CURRENT_SOURCE_DIR}>
)

if(TARGET Eigen3::Eigen)
    target_link_libraries(meadow PRIVATE
        Eigen3::Eigen
    )
    target_compile_definitions(meadow PUBLIC MEADOW_HAS_EIGEN=1)
else()
    target_compile_definitions(meadow PUBLIC MEADOW_HAS_EIGEN=0)
endif()

target_link_libraries(meadow PUBLIC
    absl::cleanup
    absl::log
    absl::check
)
