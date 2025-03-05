cmake_minimum_required(VERSION 3.5.0)

enable_testing() 

find_package(GTest REQUIRED)
include(GoogleTest)

add_executable(
    wtests
    ${PROJECT_SOURCE_DIR}/tests/expr_test.cpp
    ${PROJECT_SOURCE_DIR}/tests/ast_test.cpp
)

target_link_libraries(
    wtests 
    GTest::gtest 
    GTest::gtest_main 
    pthread
)

target_include_directories(
    wtests
    PRIVATE ${PROJECT_SOURCE_DIR}/src/wombat_compiler/wombat_lazy_lexer
    PRIVATE ${PROJECT_SOURCE_DIR}/src/wombat_compiler/wombat_parser
)

gtest_discover_tests(wtests)

add_test(NAME WombatUnitTests COMMAND wtests)
