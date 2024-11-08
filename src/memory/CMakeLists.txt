add_library(memory STATIC
    allocate.c allocate.h
)
target_link_libraries(memory PUBLIC menos)