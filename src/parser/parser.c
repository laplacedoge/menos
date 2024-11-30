#include "parser.h"
#include "memory/allocate.h"
#include "rule.h"

const char *
ParErr_ToStr(
    ParErr err
) {
    switch (err) {
    case ParErr_Ok: return "Ok";
    case ParErr_NoEnoughMemory: return "No enough memory";
    case ParErr_UnexpectedToken: return "Unexpected token";
    }
}

typedef struct _Parser {

    /* Pointer references. */
    LexOut * lo;
    TokSeq * seq;
    FixedBuf * src;

    usize num;
    usize off;

    struct {
        ParErr type;
        FlexBuf * msg;
        usize line_no;
        usize col_no;
    } err;
} Parser;

Parser *
Parser_New(void) {
    FlexBuf * err_msg = FlexBuf_New();
    if (err_msg == NULL) {
        goto Exit;
    }

    Parser * par = (Parser *)MeMem_Malloc(sizeof(Parser));
    if (par == NULL) {
        goto FreeErrMsg;
    }

    par->seq = NULL;
    par->num = 0;
    par->off = 0;

    par->err.type = ParErr_Ok;
    par->err.msg = err_msg;
    par->err.line_no = 0;
    par->err.col_no = 0;

    return par;

FreeErrMsg:
    FlexBuf_Free(err_msg);

Exit:
    return NULL;
}

void
Parser_Link(
    Parser * par,
    LexOut * lo
) {
    par->lo = lo;
    par->seq = LexOut_Tokens(lo);
    par->src = LexOut_Source(lo);
    par->num = TokSeq_Count(par->seq);
    par->off = 0;
}

Token *
Parser_Peek(
    Parser * par
) {
    return TokSeq_At(par->seq, par->off);
}

bool
Parser_Check(
    Parser * par,
    TokTag tag
) {
    Token * tok = Parser_Peek(par);
    if (tok == NULL ||
        tok->tag != tag) {

        return false;
    }

    return true;
}

Token *
Parser_Expect(
    Parser * par,
    TokTag tag
) {
    Token * tok = Parser_Peek(par);
    if (tok == NULL ||
        tok->tag != tag) {

        return NULL;
    }

    par->off++;

    return tok;
}

Token *
Parser_ExpectAny(
    Parser * par,
    const TokTag * buf_tags,
    usize num_tags
) {
    bool found = false;
    Token * tok = Parser_Peek(par);
    for (usize i = 0; i < num_tags; i++) {
        if (tok->tag == buf_tags[i]) {
            found = true;
            break;
        }
    }

    if (found == false) {
        return NULL;
    }

    par->off++;

    return tok;
}

bool
Parser_Consume(
    Parser * par
) {
    if (par->off == TokSeq_Count(par->seq)) {
        return false;
    }

    par->off++;

    return true;
}

static
void
Parser_SetErrorInfo(
    Parser * par
) {
    const char * PREFIX = "Parser error";
    FlexBuf * msg = par->err.msg;
    const ParErr err = par->err.type;
    const char * const err_msg = ParErr_ToStr(err);

    Token * tok = TokSeq_At(par->seq, par->off);
    usize row_no = tok->row + 1;
    usize col_no = tok->col + 1;

    switch (err) {
    case ParErr_Ok:
        break;

    case ParErr_NoEnoughMemory:
        FlexBuf_PushFmt(msg, "%s: %s",
            PREFIX, err_msg);
        break;

    case ParErr_UnexpectedToken:
        FlexBuf_PushFmt(msg, "%.*s:%zu:%zu: %s: %s %s",
            (int)FixedBuf_Size(par->src),
            (char *)FixedBuf_Data(par->src),
            row_no, col_no, PREFIX, err_msg, TokTag_ToStr(tok->tag));
        break;
    }

    par->err.line_no = row_no;
    par->err.col_no = col_no;
}

bool
Parser_Parse(
    Parser * par,
    AstNode ** tree
) {
    if (par->seq == NULL) {
        return false;
    }

    if (*tree = ParRule_Prog(par), par->err.type != ParErr_Ok) {
        Parser_SetErrorInfo(par);
        return false;
    }

    return true;
}

void
Parser_SetNoEnoughMemoryError(
    Parser * par
) {
    par->err.type = ParErr_NoEnoughMemory;
}

void
Parser_SetUnexpectedTokenError(
    Parser * par
) {
    par->err.type = ParErr_UnexpectedToken;
}

bool
Parser_Failed(
    Parser * par
) {
    return par->err.type != ParErr_Ok;
}

ParErr
Parser_ErrorType(
    Parser * par
) {
    return par->err.type;
}

FlexBuf *
Parser_ErrorMessage(
    Parser * par
) {
    return par->err.msg;
}

void
Parser_Reset(
    Parser * par
) {
    par->seq = NULL;
    par->num = 0;
    par->off = 0;

    par->err.type = ParErr_Ok;
    FlexBuf_Clear(par->err.msg);
    par->err.line_no = 0;
    par->err.col_no = 0;
}

void
Parser_Free(
    Parser * par
) {
    FlexBuf_Free(par->err.msg);
    MeMem_Free(par);
}