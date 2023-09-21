cmake_minimum_required(VERSION 3.21)

project(chip8)

set(CMAKE_EXPORT_COMPILE_COMMANDS ON)
set(CMAKE_CXX_STANDARD 20)

# TODO
add_compile_options(-Wno-address-of-temporary)
add_compile_options(-Wno-c99-designator)

include(FetchContent)

# Main
add_executable(${PROJECT_NAME} main.cpp sokol_impl.cpp)

add_executable(print_inst print_inst.cpp)

# sokol
FetchContent_Declare(
  sokol
  GIT_REPOSITORY https://github.com/floooh/sokol
)

FetchContent_GetProperties(sokol)
FetchContent_Populate(sokol)
target_include_directories(chip8 PUBLIC ${sokol_SOURCE_DIR})