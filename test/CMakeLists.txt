add_executable(test
    test.c greatest.h
    fixed_buf.c
)
target_link_libraries(test PRIVATE fixed_buf)