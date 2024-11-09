add_library(lexer STATIC
    token.c token.h
)
target_link_libraries(lexer PUBLIC menos fixed_buf flex_buf)