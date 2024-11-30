#ifndef __ME_PARSER_PARSER_H__
#define __ME_PARSER_PARSER_H__

#include "menos.h"
#include "lexer/token.h"
#include "lexer/lexer.h"
#include "ast.h"

typedef enum _ParErr {
    ParErr_Ok,
    ParErr_NoEnoughMemory,
    ParErr_UnexpectedToken,
} ParErr;

typedef struct _Parser Parser;

Parser *
Parser_New(void);

void
Parser_Link(
    Parser * par,
    LexOut * lo
);

Token *
Parser_Peek(
    Parser * par
);

bool
Parser_Check(
    Parser * par,
    TokTag tag
);

Token *
Parser_Expect(
    Parser * par,
    TokTag tag
);

Token *
Parser_ExpectAny(
    Parser * par,
    const TokTag * buf_tags,
    usize num_tags
);

bool
Parser_Consume(
    Parser * par
);

bool
Parser_Parse(
    Parser * par,
    AstNode ** tree
);

void
Parser_SetNoEnoughMemoryError(
    Parser * par
);

void
Parser_SetUnexpectedTokenError(
    Parser * par
);

bool
Parser_Failed(
    Parser * par
);

ParErr
Parser_ErrorType(
    Parser * par
);

FlexBuf *
Parser_ErrorMessage(
    Parser * par
);

void
Parser_Reset(
    Parser * par
);

void
Parser_Free(
    Parser * par
);

#endif