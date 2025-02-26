cmake_minimum_required(VERSION 3.5.0)

project(
    wombat 
    VERSION 0.1.0 
    LANGUAGES C CXX
)

add_executable(
    wombat 
    main.cpp
    ${PROJECT_SOURCE_DIR}/src/wombat_cli/interface.cpp
    ${PROJECT_SOURCE_DIR}/src/wombat_cli/args.cpp
    ${PROJECT_SOURCE_DIR}/src/wombat_errors/diagnostic.cpp
    ${PROJECT_SOURCE_DIR}/src/wombat_callback/callback.cpp
    ${PROJECT_SOURCE_DIR}/src/wombat_files/io_file.cpp
    ${PROJECT_SOURCE_DIR}/src/wombat_compiler/session.cpp
    ${PROJECT_SOURCE_DIR}/src/wombat_compiler/wombat_lazy_lexer/lex.cpp
    ${PROJECT_SOURCE_DIR}/src/wombat_compiler/wombat_lazy_lexer/token.cpp
)

set(CMAKE_BUILD_TYPE Debug)
set_property(TARGET wombat PROPERTY CXX_STANDARD 23)
set_property(TARGET wombat PROPERTY CXX_STANDARD_REQUIRED ON)

set(CMAKE_RUNTIME_OUTPUT_DIRECTORY ${CMAKE_BINARY_DIR}/bin)

target_include_directories(
    wombat 
    PRIVATE ${PROJECT_SOURCE_DIR}/src
    PRIVATE ${PROJECT_SOURCE_DIR}/src/wombat_cli
    PRIVATE ${PROJECT_SOURCE_DIR}/src/wombat_callback
    PRIVATE ${PROJECT_SOURCE_DIR}/src/wombat_errors
    PRIVATE ${PROJECT_SOURCE_DIR}/src/wombat_compiler
    PRIVATE ${PROJECT_SOURCE_DIR}/src/wombat_lexer
    PRIVATE ${PROJECT_SOURCE_DIR}/src/wombat_files
    PRIVATE ${PROJECT_SOURCE_DIR}/src/wombat_compiler/wombat_lazy_lexer
)