#ifndef __ME_LEXER_TOKEN_H__
#define __ME_LEXER_TOKEN_H__

#include "menos.h"
#include "util/fixed_buf.h"
#include "util/flex_buf.h"

/* Token tag, the type of token. */
typedef enum _TokTag {

    /* Keywords. */
    TokTag_Let,
    TokTag_If,
    TokTag_Else,
    TokTag_False,
    TokTag_True,
    TokTag_Match,
    TokTag_While,
    TokTag_For,
    TokTag_Break,
    TokTag_Continue,
    TokTag_Return,

    /* Symbols. */
    TokTag_Assign,
    TokTag_Plus,
    TokTag_Minus,
    TokTag_Asterisk,
    TokTag_ForwardSlash,
    TokTag_Equ,
    TokTag_Neq,
    TokTag_GreaterThan,
    TokTag_Gte,
    TokTag_LessThan,
    TokTag_Lte,
    TokTag_LeftParen,
    TokTag_RightParen,
    TokTag_LeftBracket,
    TokTag_RightBracket,
    TokTag_RightBrace,
    TokTag_LeftBrace,
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

void
Token_InitWithTag(
    Token * tok,
    TokTag tag
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

Token *
TokSeq_At(
    TokSeq * seq,
    usize idx
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