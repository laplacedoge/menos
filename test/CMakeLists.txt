add_executable(test
    test.c greatest.h
    test_fixed_buf.c
    test_flex_buf.c
    test_lexer.c
)
target_link_libraries(test PRIVATE
    fixed_buf flex_buf lexer
)