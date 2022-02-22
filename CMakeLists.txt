cmake_minimum_required(VERSION 3.1)
project(tile_puzzles)

set(CMAKE_CXX_STANDARD 20)
option(USE_CLANG "build application with clang" ON) # OFF is the default
set(CMAKE_VERBOSE_MAKEFILE off)
include_directories(${SDL2_INCLUDE_DIRS} ${CMAKE_SOURCE_DIR}/tilePuzzlesLib)
set(SRC main.cpp)
add_executable(tile_puzzles ${SRC})

add_subdirectory(tilePuzzlesLib)
target_link_libraries(tile_puzzles PUBLIC tilePuzzlesLib)
target_include_directories(tile_puzzles PUBLIC
                          "${PROJECT_BINARY_DIR}"
                          "${PROJECT_SOURCE_DIR}/tilePuzzlesLib"
                          )
