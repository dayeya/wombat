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
    PRIVATE ${PROJECT_SOURCE_DIR}/src/utils/str.cpp
    PRIVATE ${PROJECT_SOURCE_DIR}/src/utils/file.cpp
    PRIVATE ${PROJECT_SOURCE_DIR}/src/build/builder.cpp
    PRIVATE ${PROJECT_SOURCE_DIR}/src/compiler/core/compiler.cpp
    PRIVATE ${PROJECT_SOURCE_DIR}/src/compiler/lazy_lexer/lex.cpp
    PRIVATE ${PROJECT_SOURCE_DIR}/src/compiler/lazy_lexer/token.cpp
    PRIVATE ${PROJECT_SOURCE_DIR}/src/compiler/parser/parser.cpp
    PRIVATE ${PROJECT_SOURCE_DIR}/src/compiler/parser/expr.cpp
    PRIVATE ${PROJECT_SOURCE_DIR}/src/compiler/parser/stmt.cpp
    PRIVATE ${PROJECT_SOURCE_DIR}/src/compiler/parser/decl.cpp
    PRIVATE ${PROJECT_SOURCE_DIR}/src/compiler/ast/pp_visitor.cpp
    PRIVATE ${PROJECT_SOURCE_DIR}/src/compiler/errors/diag.cpp
)

set(CMAKE_BUILD_TYPE Debug)
set_property(TARGET wombat PROPERTY CXX_STANDARD 23)
set_property(TARGET wombat PROPERTY CXX_STANDARD_REQUIRED ON)

set(CMAKE_RUNTIME_OUTPUT_DIRECTORY ${CMAKE_BINARY_DIR}/bin)

target_include_directories(
    wombat 
    PRIVATE ${PROJECT_SOURCE_DIR}/src/build
    PRIVATE ${PROJECT_SOURCE_DIR}/src/utils
    PRIVATE ${PROJECT_SOURCE_DIR}/src/compiler/core
    PRIVATE ${PROJECT_SOURCE_DIR}/src/compiler/errors
    PRIVATE ${PROJECT_SOURCE_DIR}/src/compiler/lazy_lexer
    PRIVATE ${PROJECT_SOURCE_DIR}/src/compiler/parser
    PRIVATE ${PROJECT_SOURCE_DIR}/src/compiler/ast
    PRIVATE ${PROJECT_SOURCE_DIR}/src/compiler/codegen
    PRIVATE ${PROJECT_SOURCE_DIR}/src/compiler/sema_analysis
)