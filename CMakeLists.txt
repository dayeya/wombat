cmake_minimum_required(VERSION 3.5.0)

project(
    wombat 
    VERSION 0.1.0 
    LANGUAGES C CXX
)

add_executable(
    wombat 
    main.cpp
    ${PROJECT_SOURCE_DIR}/src/compiler/compiler.cpp
    ${PROJECT_SOURCE_DIR}/src/lexer/lexer.cpp
    ${PROJECT_SOURCE_DIR}/src/warg_parser/warg_parser.cpp
)

set(CMAKE_BUILD_TYPE Debug)
set_property(TARGET wombat PROPERTY CXX_STANDARD 23)
set_property(TARGET wombat PROPERTY CXX_STANDARD_REQUIRED ON)

set(CMAKE_RUNTIME_OUTPUT_DIRECTORY ${CMAKE_BINARY_DIR}/bin)

target_include_directories(
    wombat PRIVATE ${PROJECT_SOURCE_DIR}/src
    PRIVATE ${PROJECT_SOURCE_DIR}/src/compiler
    PRIVATE ${PROJECT_SOURCE_DIR}/src/lexer
)