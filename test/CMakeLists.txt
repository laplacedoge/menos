add_executable(test
    test.c greatest.h
    fixed_buf.c
    flex_buf.c
    lexer.c
)
target_link_libraries(test PRIVATE
    fixed_buf flex_buf lexer
)