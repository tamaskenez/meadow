file(GLOB sources *.h *.cpp)
source_group(TREE ${CMAKE_CURRENT_SOURCE_DIR} FILES ${sources})
add_library(meadow STATIC ${sources})
target_precompile_headers(meadow
    PRIVATE pch.h
)
target_include_directories(meadow
    PUBLIC $<BUILD_INTERFACE:meadow>
)
