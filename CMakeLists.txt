cmake_minimum_required(VERSION 3.5.0)

project(
    wombat_lang
    VERSION 0.1.0 
    LANGUAGES C CXX
)

add_executable(wombat)

target_sources(
    wombat
    PRIVATE main.cpp
    PRIVATE ${PROJECT_SOURCE_DIR}/src/den/den.cpp
    PRIVATE ${PROJECT_SOURCE_DIR}/src/utils/strch.cpp
)

set(CMAKE_BUILD_TYPE Debug)
set_property(TARGET wombat PROPERTY CXX_STANDARD 23)
set_property(TARGET wombat PROPERTY CXX_STANDARD_REQUIRED ON)
set_property(TARGET wombat PROPERTY OUTPUT_NAME "den")

set(CMAKE_RUNTIME_OUTPUT_DIRECTORY ${CMAKE_BINARY_DIR}/bin)

target_include_directories(
    wombat 
    PRIVATE ${PROJECT_SOURCE_DIR}/src/den
    PRIVATE ${PROJECT_SOURCE_DIR}/src/utils
    PRIVATE ${PROJECT_SOURCE_DIR}/src/compiler
    PRIVATE ${PROJECT_SOURCE_DIR}/src/compiler/diagnostics
    PRIVATE ${PROJECT_SOURCE_DIR}/src/compiler/lazy_lexer
    PRIVATE ${PROJECT_SOURCE_DIR}/src/compiler/parser
)