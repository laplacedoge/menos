#ifndef __ME_LEXER_LEXER_H__
#define __ME_LEXER_LEXER_H__

#include "menos.h"
#include "lexer/token.h"

typedef enum _LexErr {
    LexErr_Ok,
    LexErr_NoEnoughMemory,
    LexErr_UnexpectedByte,
} LexErr;

const char *
LexErr_ToStr(
    LexErr err
);

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

LexErr
Lexer_ErrorType(
    Lexer * lex
);

FlexBuf *
Lexer_ErrorMessage(
    Lexer * lex
);

usize
Lexer_ErrorLineNo(
    Lexer * lex
);

usize
Lexer_ErrorColumnNo(
    Lexer * lex
);

void
Lexer_Reset(
    Lexer * lex
);

void
Lexer_Free(
    Lexer * lex
);

#endif