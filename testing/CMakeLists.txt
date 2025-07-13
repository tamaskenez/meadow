file(GLOB_RECURSE sources CONFIGURE_DEPENDS *.h *.cpp)
source_group(TREE ${CMAKE_CURRENT_SOURCE_DIR} FILES ${sources})
add_executable(test_meadow ${sources})
target_precompile_headers(test_meadow
    PRIVATE pch.h
)
target_link_libraries(test_meadow PRIVATE
    meadow
    GTest::gtest
    GTest::gtest_main
)
add_test(NAME test_meadow COMMAND test_meadow)
