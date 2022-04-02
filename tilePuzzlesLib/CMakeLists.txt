cmake_minimum_required(VERSION 3.1)
project(tile_puzzles_lib)

set(CMAKE_CXX_STANDARD 20)
option(USE_CLANG "build application with clang" ON) # OFF is the default
set(CMAKE_VERBOSE_MAKEFILE off)

set(SRC 
libMain.cpp
)

set(TESTS
test/test_config.cpp
test/test_entities.cpp
test/test_logger.cpp
test/test_geometry.cpp
test/test_utils.cpp
)

set(LIB_PATH ${CMAKE_SOURCE_DIR}/lib/x86_64)
file(GLOB FILAMENT_LIBS "${LIB_PATH}/*.a") 


find_package(Catch2 3 REQUIRED)
find_package(SDL2 REQUIRED)
find_package (Threads)

include_directories(${SDL2_INCLUDE_DIRS} ${CMAKE_SOURCE_DIR}/tilePuzzlesLib/include)

set(LIBS 
filament 
backend 
shaders 
vkshaders 
ibl 
bluegl 
bluevk 
filabridge 
filaflat 
filamat 
utils 
smol-v 
filamentapp 
filamentapp-resources 
filament-iblprefilter 
image 
imageio 
camutils 
filagui 
imgui 
)

add_definitions(-DUSE_SDL)

add_library(tilePuzzlesLib ${SRC})
target_compile_options(tilePuzzlesLib PUBLIC -fno-builtin)

foreach(lib ${LIBS})
        add_library(${lib} STATIC IMPORTED)
        set_target_properties(${lib} PROPERTIES IMPORTED_LOCATION ${CMAKE_SOURCE_DIR}/tilePuzzlesLib/lib/x86_64/lib${lib}.a)
        target_link_libraries(tilePuzzlesLib ${lib})
endforeach()

add_library(glog STATIC IMPORTED)
set_target_properties(glog PROPERTIES IMPORTED_LOCATION ${CMAKE_SOURCE_DIR}/tilePuzzlesLib/lib/libglog.so)

target_link_libraries(tilePuzzlesLib pthread c++ dl SDL2 glog)
target_include_directories(tilePuzzlesLib PUBLIC "${CMAKE_SOURCE_DIR}/tilePuzzlesLib/include")

##########################################################
# TESTS
add_executable(tests ${TESTS})
set_target_properties(tests PROPERTIES RUNTIME_OUTPUT_DIRECTORY_DEBUG test)
target_link_libraries(tests PUBLIC tilePuzzlesLib)
target_include_directories(tests PUBLIC
                          "${PROJECT_BINARY_DIR}"
                          "${PROJECT_SOURCE_DIR}/tilePuzzlesLib"
                          )
target_link_libraries(tests PRIVATE Catch2::Catch2WithMain)                          
##########################################################


# file(COPY ../third_party/textures DESTINATION ${PROJECT_BINARY_DIR})

add_custom_command(
        TARGET tilePuzzlesLib
        COMMAND ${CMAKE_COMMAND} -E make_directory ${CMAKE_BINARY_DIR}/textures
        COMMAND ${CMAKE_COMMAND} -E make_directory ${CMAKE_BINARY_DIR}/materials        
        COMMAND ${CMAKE_COMMAND} -E copy ${CMAKE_SOURCE_DIR}/tilePuzzlesLib/textures/** ${CMAKE_BINARY_DIR}/textures
        COMMAND ${CMAKE_COMMAND} -E copy ${CMAKE_SOURCE_DIR}/tile_material/** ${CMAKE_BINARY_DIR}/materials)                

