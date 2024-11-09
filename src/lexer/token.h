#ifndef __ME_LEXER_TOKEN_H__
#define __ME_LEXER_TOKEN_H__

#include "menos.h"
#include "util/fixed_buf.h"
#include "util/flex_buf.h"

/* Token tag, the type of token. */
typedef enum _TokTag {

    /* Keywords. */
    TokTag_Let,

    /* Symbols. */
    TokTag_Semicolon,

    TokTag_Name,
    TokTag_NumLit,
    TokTag_StrLit,

    TokTag_Eof,
} TokTag;

const char *
TokTag_ToStr(
    TokTag tag
);

/* Token. */
typedef struct _Token {

    /* Token tag. */
    TokTag tag;

    /* Line no. */
    usize line_no;

    /* Lexeme offset within the line. */
    usize lex_off;

    /* Lexeme length. */
    usize lex_len;

    /* Extra token attributes. */
    union {

        /* Name token. */
        struct {

            /* Name string. */
            FixedBuf * str;
        } name;

        /* Number literal token. */
        struct {

            /* Number value. */
            usize val;
        } num_lit;

        /* String literal token. */
        struct {

            /* String. */
            FixedBuf * str;
        } str_lit;
    } ext;
} Token;

void
Token_Init(
    Token * tok
);

bool
Token_PushAsStr(
    Token * tok,
    FlexBuf * buf
);

/* Token sequence, the list of tokens. */
typedef struct _TokSeq TokSeq;

TokSeq *
TokSeq_New(void);

bool
TokSeq_Push(
    TokSeq * seq,
    Token * tok
);

Token *
TokSeq_Data(
    TokSeq * seq
);

usize
TokSeq_Count(
    TokSeq * seq
);

bool
TokSeq_Compact(
    TokSeq * seq
);

bool
TokSeq_PushAsStr(
    TokSeq * seq,
    FlexBuf * buf,
    ssize ind
);

void
TokSeq_Free(
    TokSeq * seq
);

#endif