cmake_minimum_required(VERSION 3.5.0)

find_package(GTest REQUIRED)
include(GoogleTest)

add_executable(expr_tests)

target_sources(
    expr_tests 
    PRIVATE ${PROJECT_SOURCE_DIR}/tests/expr_test.cpp
    PRIVATE ${PROJECT_SOURCE_DIR}/src/wombat_compiler/wombat_lazy_lexer/token.cpp
)

target_link_libraries(
    expr_tests 
    GTest::gtest
    GTest::gtest_main pthread
)

target_include_directories(
    expr_tests
    PRIVATE ${PROJECT_SOURCE_DIR}/src/wombat_compiler/wombat_lazy_lexer
    PRIVATE ${PROJECT_SOURCE_DIR}/src/wombat_compiler/wombat_parser
)

gtest_discover_tests(expr_tests)
