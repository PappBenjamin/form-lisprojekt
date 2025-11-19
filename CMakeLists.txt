cmake_minimum_required(VERSION 3.30)
project(form_lisprojekt)

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

# Set output directories for IDE convenience
set(CMAKE_RUNTIME_OUTPUT_DIRECTORY ${CMAKE_BINARY_DIR}/bin)

# Include FetchContent for downloading dependencies
include(FetchContent)

# Fetch nlohmann-json (header-only library)
FetchContent_Declare(json
        URL https://github.com/nlohmann/json/releases/download/v3.11.2/include.zip
)

FetchContent_MakeAvailable(json)

# Project sources
add_executable(form_lisprojekt
        src/main.cpp
        src/lexer.cpp
        src/parser.cpp
        src/peglib_parser.cpp
        src/ast.cpp
        src/exceptions.cpp
        src/arduino_generator.cpp
)

# Include directories
target_include_directories(form_lisprojekt PRIVATE
        ${CMAKE_CURRENT_SOURCE_DIR}/include
        ${json_SOURCE_DIR}/include
)

# Compiler flags
if(MSVC)
    target_compile_options(form_lisprojekt PRIVATE /W4)
else()
    target_compile_options(form_lisprojekt PRIVATE -Wall -Wextra -Wpedantic)
endif()

# Set as startup project for IDE
set_property(DIRECTORY ${CMAKE_CURRENT_SOURCE_DIR} PROPERTY VS_STARTUP_PROJECT form_lisprojekt)
