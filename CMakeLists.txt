cmake_minimum_required(VERSION 3.15)

project(menos LANGUAGES C)

set(CMAKE_EXPORT_COMPILE_COMMANDS ON)

add_compile_options(-g)

add_subdirectory(src)
add_subdirectory(test)