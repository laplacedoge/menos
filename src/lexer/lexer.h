#ifndef __ME_LEXER_LEXER_H__
#define __ME_LEXER_LEXER_H__

#include "menos.h"
#include "lexer/token.h"

typedef struct _Lexer Lexer;

Lexer *
Lexer_New(void);

bool
Lexer_Feed(
    Lexer * lex,
    const void * buf,
    usize len
);

bool
Lexer_Finalize(
    Lexer * lex,
    TokSeq ** seq
);

void
Lexer_Free(
    Lexer * lex
);

#endif