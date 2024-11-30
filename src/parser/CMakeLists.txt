add_library(parser STATIC
    ast.c ast.h
    rule.c rule.h
    parser.c parser.h
)
target_link_libraries(parser PUBLIC menos memory fixed_buf flex_buf)