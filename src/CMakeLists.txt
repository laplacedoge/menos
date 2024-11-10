add_library(menos INTERFACE)
target_include_directories(menos INTERFACE ${CMAKE_CURRENT_SOURCE_DIR})

add_subdirectory(memory)
add_subdirectory(util)
add_subdirectory(lexer)