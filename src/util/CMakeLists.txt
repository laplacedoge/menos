add_library(fixed_buf STATIC
    fixed_buf.c fixed_buf.h
)
target_link_libraries(fixed_buf PRIVATE memory)
target_link_libraries(fixed_buf PUBLIC menos)