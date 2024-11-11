add_library(fixed_buf STATIC
    fixed_buf.c fixed_buf.h
)
target_link_libraries(fixed_buf PRIVATE memory flex_buf)
target_link_libraries(fixed_buf PUBLIC menos)

add_library(flex_buf STATIC
    flex_buf.c flex_buf.h
)
target_link_libraries(flex_buf PRIVATE memory)
target_link_libraries(flex_buf PUBLIC menos)