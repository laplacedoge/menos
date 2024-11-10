#include "lexer.h"
#include "token.h"
#include "memory/allocate.h"
#include "util/fixed_buf.h"
#include "util/flex_buf.h"

/* FSM state. */
typedef enum _FsmStat {
    FsmStat_Idle,
    FsmStat_Name,
} FsmStat;

/* FSM result. */
typedef enum _FsmRes {
    FsmRes_Ok,
    FsmRes_Again,
    FsmRes_NoMemory,
    FsmRes_UnexpectedByte,
} FsmRes;

typedef struct _Lexer {
    FsmStat stat;
    FlexBuf * str;
    TokSeq * seq;
} Lexer;

Lexer *
Lexer_New(void) {
    TokSeq * seq = TokSeq_New();
    if (seq == NULL) {
        goto Exit;
    }

    FlexBuf * str = FlexBuf_New();
    if (str == NULL) {
        goto FreeSeq;
    }

    Lexer * lex = (Lexer *)MeMem_Malloc(sizeof(Lexer));
    if (lex == NULL) {
        goto FreeStr;
    }

    lex->stat = FsmStat_Idle;
    lex->str = str;
    lex->seq = seq;

    return lex;

FreeStr:
    FlexBuf_Free(str);

FreeSeq:
    TokSeq_Free(seq);

Exit:
    return NULL;
}

static
bool
PushNormalToken(
    TokSeq * seq,
    TokTag tag
) {
    Token tok;
    Token_InitWithTag(&tok, tag);
    return TokSeq_Push(seq, &tok);
}

static
bool
PushNameToken(
    TokSeq * seq,
    FlexBuf * name
) {
    FixedBuf * str = FlexBuf_ToFixedBuf(name);
    if (str == NULL) {
        goto Exit;
    }

    Token tok;
    Token_InitWithTag(&tok, TokTag_Name);
    tok.ext.name.str = str;

    if (TokSeq_Push(seq, &tok) == false) {
        goto FreeStr;
    }

    return true;

FreeStr:
    FixedBuf_Free(str);

Exit:
    return false;
}

static
inline
FsmRes
Lexer_FeedByte_Idle(
    Lexer * lex,
    u8 byte
) {

    /* Ignore the whitespace character. */
    if (byte == ' ' ||
        byte == '\t' ||
        byte == '\r' ||
        byte == '\n') {

        return FsmRes_Ok;
    }

    /* If this is a first character of a name. */
    if ((byte >= 'A' && byte <= 'Z') ||
        (byte >= 'a' && byte <= 'z') ||
        byte == '_') {

        /* Clear the string buffer and push the first character. */
        FlexBuf_Clear(lex->str);
        if (FlexBuf_PushByte(lex->str, byte) == false) {
            return FsmRes_NoMemory;
        }

        lex->stat = FsmStat_Name;

        return FsmRes_Ok;
    }

    do {
        TokTag tag;

        if (byte == '=') {
            tag = TokTag_Assign;
        } else if (byte == ';') {
            tag = TokTag_Semicolon;
        } else {
            break;
        }

        if (PushNormalToken(lex->seq, tag) == false) {
            return FsmRes_NoMemory;
        }

        return FsmRes_Ok;

    } while (false);

    return FsmRes_UnexpectedByte;
}

static
inline
FsmRes
Lexer_FeedByte_Name(
    Lexer * lex,
    u8 byte
) {

    /* If this is a remaining character of a name. */
    if ((byte >= '0' && byte <= '9') ||
        (byte >= 'A' && byte <= 'Z') ||
        (byte >= 'a' && byte <= 'z') ||
        byte == '_') {

        /* Push this character to the string buffer. */
        if (FlexBuf_PushByte(lex->str, byte) == false) {
            return FsmRes_NoMemory;
        }

        return FsmRes_Ok;
    }

    /* This name is finished, push it to the token sequence. */
    if (PushNameToken(lex->seq, lex->str) == false) {
        return FsmRes_NoMemory;
    }

    lex->stat = FsmStat_Idle;

    return FsmRes_Again;
}

static
FsmRes
Lexer_FeedByte(
    Lexer * lex,
    u8 byte
) {
    FsmRes res;

    switch (lex->stat) {
    case FsmStat_Idle:
        res = Lexer_FeedByte_Idle(lex, byte);
        break;

    case FsmStat_Name:
        res = Lexer_FeedByte_Name(lex, byte);
        break;
    }

    return res;
}

static
inline
FsmRes
Lexer_FeedEol_Name(
    Lexer * lex
) {

    /* This name is finished, push it to the token sequence. */
    if (PushNameToken(lex->seq, lex->str) == false) {
        return FsmRes_NoMemory;
    }

    lex->stat = FsmStat_Idle;

    return FsmRes_Ok;
}

static
FsmRes
Lexer_FeedEol(
    Lexer * lex
) {
    FsmRes res;

    switch (lex->stat) {
    case FsmStat_Idle:
        res = FsmRes_Ok;
        break;

    case FsmStat_Name:
        res = Lexer_FeedEol_Name(lex);
        break;
    }

    return res;
}

bool
Lexer_Feed(
    Lexer * lex,
    const void * buf,
    usize len
) {
    for (usize i = 0; i < len; i++) {
        u8 byte = ((const u8 *)buf)[i];

        while (true) {
            FsmRes res = Lexer_FeedByte(lex, byte);
            if (res == FsmRes_Ok) {
                break;
            } else if (res == FsmRes_Again) {
                continue;
            } else if (res == FsmRes_NoMemory ||
                       res == FsmRes_UnexpectedByte) {
                return false;
            }
        }
    }

    return true;
}

bool
Lexer_Finalize(
    Lexer * lex
) {
    switch (Lexer_FeedEol(lex)) {
    case FsmRes_Ok:
        break;

    case FsmRes_Again:
    case FsmRes_NoMemory:
    case FsmRes_UnexpectedByte:
        return false;
    }

    return true;
}

void
Lexer_Free(
    Lexer * lex
) {
    FlexBuf_Free(lex->str);
    TokSeq_Free(lex->seq);
    MeMem_Free(lex);
}