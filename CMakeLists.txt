cmake_minimum_required(VERSION 3.5.0)

project(
    wombat 
    VERSION 0.1.0 
    LANGUAGES C CXX
)

add_executable(wombat)

target_sources(
    wombat
    PRIVATE main.cpp
    PRIVATE ${PROJECT_SOURCE_DIR}/src/wombat_cli/interface.cpp
    PRIVATE ${PROJECT_SOURCE_DIR}/src/wombat_cli/args.cpp
    PRIVATE ${PROJECT_SOURCE_DIR}/src/wombat_compiler/wombat_cutil/cutil.cpp
    PRIVATE ${PROJECT_SOURCE_DIR}/src/wombat_errors/diagnostic.cpp
    PRIVATE ${PROJECT_SOURCE_DIR}/src/wombat_callback/callback.cpp
    PRIVATE ${PROJECT_SOURCE_DIR}/src/wombat_files/io_file.cpp
    PRIVATE ${PROJECT_SOURCE_DIR}/src/wombat_compiler/session.cpp
    PRIVATE ${PROJECT_SOURCE_DIR}/src/wombat_compiler/wombat_lazy_lexer/lex.cpp
    PRIVATE ${PROJECT_SOURCE_DIR}/src/wombat_compiler/wombat_lazy_lexer/token.cpp
    PRIVATE ${PROJECT_SOURCE_DIR}/src/wombat_compiler/wombat_parser/parser.cpp
    PRIVATE ${PROJECT_SOURCE_DIR}/src/wombat_compiler/wombat_parser/expr.cpp
    PRIVATE ${PROJECT_SOURCE_DIR}/src/wombat_compiler/wombat_parser/ast.cpp
    PRIVATE ${PROJECT_SOURCE_DIR}/src/wombat_compiler/wombat_parser/readables.cpp
)

set(CMAKE_BUILD_TYPE Debug)
set_property(TARGET wombat PROPERTY CXX_STANDARD 23)
set_property(TARGET wombat PROPERTY CXX_STANDARD_REQUIRED ON)

set(CMAKE_RUNTIME_OUTPUT_DIRECTORY ${CMAKE_BINARY_DIR}/bin)

target_include_directories(
    wombat 
    PRIVATE ${PROJECT_SOURCE_DIR}/src
    PRIVATE ${PROJECT_SOURCE_DIR}/src/wombat_runtime
    PRIVATE ${PROJECT_SOURCE_DIR}/src/wombat_cli
    PRIVATE ${PROJECT_SOURCE_DIR}/src/wombat_callback
    PRIVATE ${PROJECT_SOURCE_DIR}/src/wombat_errors
    PRIVATE ${PROJECT_SOURCE_DIR}/src/wombat_compiler
    PRIVATE ${PROJECT_SOURCE_DIR}/src/wombat_files
    PRIVATE ${PROJECT_SOURCE_DIR}/src/wombat_compiler/wombat_cutil
    PRIVATE ${PROJECT_SOURCE_DIR}/src/wombat_compiler/wombat_lazy_lexer
    PRIVATE ${PROJECT_SOURCE_DIR}/src/wombat_compiler/wombat_parser
)

add_subdirectory(tests)