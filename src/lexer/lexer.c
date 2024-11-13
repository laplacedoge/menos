#include <string.h>

#include "lexer.h"
#include "token.h"
#include "memory/allocate.h"
#include "util/fixed_buf.h"
#include "util/flex_buf.h"

/* FSM state. */
typedef enum _FsmStat {
    FsmStat_Idle,
    FsmStat_Name,
    FsmStat_NumLit,
    FsmStat_StrLit,
    FsmStat_AssignOrEqu,
    FsmStat_Neq,
    FsmStat_RightBraceOrGte,
    FsmStat_LeftBraceOrLte,
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
    usize num;
    TokSeq * seq;
} Lexer;

/* Keyword token entry. */
typedef struct _KwTokEnt {
    const char * str;
    TokTag tag;
} KwTokEnt;

/* Keyword token map. */
static
KwTokEnt *
kw_tok_map[] = {
    (KwTokEnt[]) { { NULL, 0 } },
    (KwTokEnt[]) { { NULL, 0 } },
    (KwTokEnt[]) {
        { "if", TokTag_If },
        { NULL, 0 },
    },
    (KwTokEnt[]) {
        { "for", TokTag_For },
        { "let", TokTag_Let },
        { NULL, 0 },
    },
    (KwTokEnt[]) {
        { "else", TokTag_Else },
        { "true", TokTag_True },
        { NULL, 0 },
    },
    (KwTokEnt[]) {
        { "break", TokTag_Break },
        { "false", TokTag_False },
        { "match", TokTag_Match },
        { "while", TokTag_While },
        { NULL, 0 },
    },
    (KwTokEnt[]) {
        { "return", TokTag_Return },
        { NULL, 0 },
    },
    (KwTokEnt[]) {
        { "continue", TokTag_Continue },
        { NULL, 0 },
    },
};

/* The number of keyword token entires. */
static
const usize
num_kw_tok_ents = sizeof(kw_tok_map) / sizeof(kw_tok_map[0]);

/* The maximum length of the keyword token. */
static
const usize
max_kw_tok_len = num_kw_tok_ents - 1;

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
    lex->num = 0;
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
NameToKeyword(
    const u8 * buf,
    usize len,
    TokTag * tag
) {
    if (len == 0 ||
        len > max_kw_tok_len) {

        return false;
    }

    KwTokEnt * ent = kw_tok_map[len];
    while (ent->str != NULL) {
        if (memcmp(buf, ent->str, len) == 0) {
            *tag = ent->tag;
            return true;
        }

        ent += 1;
    }

    return false;
}

static
bool
PushNameToken(
    TokSeq * seq,
    FlexBuf * name
) {
    u8 * name_buf = FlexBuf_Data(name);
    usize name_len = FlexBuf_Size(name);

    Token tok;
    Token_Init(&tok);

    FixedBuf * str = NULL;

    if (NameToKeyword(name_buf, name_len, &tok.tag) == false) {
        tok.tag = TokTag_Name;

        str = FlexBuf_ToFixedBuf(name);
        if (str == NULL) {
            goto Exit;
        }

        tok.ext.name.str = str;
    }

    if (TokSeq_Push(seq, &tok) == false) {
        goto Exit;
    }

    return true;

Exit:
    if (str != NULL) {
        FixedBuf_Free(str);
    }

    return false;
}

static
bool
PushNumberToken(
    TokSeq * seq,
    usize num
) {
    Token tok;
    Token_InitWithTag(&tok, TokTag_NumLit);
    tok.ext.num_lit.val = num;

    return TokSeq_Push(seq, &tok);
}

static
bool
PushStringLiteralToken(
    TokSeq * seq,
    FlexBuf * str
) {
    FixedBuf * _str = FlexBuf_ToFixedBuf(str);
    if (_str == NULL) {
        goto Exit;
    }

    Token tok;
    Token_InitWithTag(&tok, TokTag_StrLit);
    tok.ext.str_lit.str = _str;

    if (TokSeq_Push(seq, &tok) == false) {
        goto FreeStr;
    }

    return true;

FreeStr:
    FixedBuf_Free(_str);

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

    /* If this is the first character of a number. */
    if (byte >= '0' && byte <= '9') {
        lex->num = byte - '0';

        lex->stat = FsmStat_NumLit;

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

    /* If this is the opening double quote of a string literal. */
    if (byte == '"') {

        /* Clear the string buffer. */
        FlexBuf_Clear(lex->str);

        lex->stat = FsmStat_StrLit;

        return FsmRes_Ok;
    }

    /* Comparison operators. */
    do {
        if (byte == '=') {
            lex->stat = FsmStat_AssignOrEqu;
        } else if (byte == '!') {
            lex->stat = FsmStat_Neq;
        } else if (byte == '>') {
            lex->stat = FsmStat_RightBraceOrGte;
        } else if (byte == '<') {
            lex->stat = FsmStat_LeftBraceOrLte;
        } else {
            break;
        }

        return FsmRes_Ok;

    } while (false);

    do {
        TokTag tag;
        bool found = true;

        switch (byte) {
        case '(': tag = TokTag_LeftParen; break;
        case ')': tag = TokTag_RightParen; break;
        case '[': tag = TokTag_LeftBracket; break;
        case ']': tag = TokTag_RightBracket; break;
        case '{': tag = TokTag_LeftBrace; break;
        case '}': tag = TokTag_RightBrace; break;
        case ';': tag = TokTag_Semicolon; break;
        default: found = false; break;
        }

        if (found == false) {
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
inline
FsmRes
Lexer_FeedByte_Num(
    Lexer * lex,
    u8 byte
) {

    /* If this is the remaining character of a number. */
    if (byte >= '0' && byte <= '9') {
        lex->num *= 10;
        lex->num += byte - '0';

        return FsmRes_Ok;
    }

    /* This number is finished, push it to the token sequence. */
    if (PushNumberToken(lex->seq, lex->num) == false) {
        return FsmRes_NoMemory;
    }

    lex->stat = FsmStat_Idle;

    return FsmRes_Again;
}

static
inline
FsmRes
Lexer_FeedByte_StrLit(
    Lexer * lex,
    u8 byte
) {

    /* Line break characters are invalid. */
    if (byte == '\r' ||
        byte == '\n') {

        return FsmRes_UnexpectedByte;
    }

    /* If this string literal is finished, push it to the token sequence. */
    if (byte == '"') {
        if (PushStringLiteralToken(lex->seq, lex->str) == false) {
            return FsmRes_NoMemory;
        }

        lex->stat = FsmStat_Idle;

        return FsmRes_Ok;
    }

    /* Push this character to the string buffer. */
    if (FlexBuf_PushByte(lex->str, byte) == false) {
        return FsmRes_NoMemory;
    }

    return FsmRes_Ok;
}

static
inline
FsmRes
Lexer_FeedByte_AsgOrEqu(
    Lexer * lex,
    u8 byte
) {
    TokTag tag;
    FsmRes res;

    if (byte == '=') {
        tag = TokTag_Equ;
        res = FsmRes_Ok;
    } else {
        tag = TokTag_Assign;
        res = FsmRes_Again;
    }

    if (PushNormalToken(lex->seq, tag) == false) {
        return FsmRes_NoMemory;
    }

    lex->stat = FsmStat_Idle;

    return res;
}

static
inline
FsmRes
Lexer_FeedByte_Neq(
    Lexer * lex,
    u8 byte
) {
    if (byte == '=') {
        if (PushNormalToken(lex->seq, TokTag_Neq) == false) {
            return FsmRes_NoMemory;
        }

        lex->stat = FsmStat_Idle;

        return FsmRes_Ok;
    }

    return FsmRes_UnexpectedByte;
}

static
inline
FsmRes
Lexer_FeedByte_GtOrGte(
    Lexer * lex,
    u8 byte
) {
    TokTag tag;
    FsmRes res;

    if (byte == '=') {
        tag = TokTag_Gte;
        res = FsmRes_Ok;
    } else {
        tag = TokTag_GreaterThan;
        res = FsmRes_Again;
    }

    if (PushNormalToken(lex->seq, tag) == false) {
        return FsmRes_NoMemory;
    }

    lex->stat = FsmStat_Idle;

    return res;
}

static
inline
FsmRes
Lexer_FeedByte_LtOrLte(
    Lexer * lex,
    u8 byte
) {
    TokTag tag;
    FsmRes res;

    if (byte == '=') {
        tag = TokTag_Lte;
        res = FsmRes_Ok;
    } else {
        tag = TokTag_LessThan;
        res = FsmRes_Again;
    }

    if (PushNormalToken(lex->seq, tag) == false) {
        return FsmRes_NoMemory;
    }

    lex->stat = FsmStat_Idle;

    return res;
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

    case FsmStat_NumLit:
        res = Lexer_FeedByte_Num(lex, byte);
        break;

    case FsmStat_StrLit:
        res = Lexer_FeedByte_StrLit(lex, byte);
        break;

    case FsmStat_AssignOrEqu:
        res = Lexer_FeedByte_AsgOrEqu(lex, byte);
        break;

    case FsmStat_Neq:
        res = Lexer_FeedByte_Neq(lex, byte);
        break;

    case FsmStat_RightBraceOrGte:
        res = Lexer_FeedByte_GtOrGte(lex, byte);
        break;

    case FsmStat_LeftBraceOrLte:
        res = Lexer_FeedByte_LtOrLte(lex, byte);
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
inline
FsmRes
Lexer_FeedEol_Num(
    Lexer * lex
) {

    /* This number is finished, push it to the token sequence. */
    if (PushNumberToken(lex->seq, lex->num) == false) {
        return FsmRes_NoMemory;
    }

    lex->stat = FsmStat_Idle;

    return FsmRes_Ok;
}

static
inline
FsmRes
Lexer_FeedEol_StrLit(
    Lexer * lex
) {

    /* If this string literal is finished, push it to the token sequence. */
    if (PushStringLiteralToken(lex->seq, lex->str) == false) {
        return FsmRes_NoMemory;
    }

    lex->stat = FsmStat_Idle;

    return FsmRes_Ok;
}

static
inline
FsmRes
Lexer_FeedEol_AsgOrEqu(
    Lexer * lex
) {
    if (PushNormalToken(lex->seq, TokTag_Assign) == false) {
        return FsmRes_NoMemory;
    }

    lex->stat = FsmStat_Idle;

    return FsmRes_Ok;
}

static
inline
FsmRes
Lexer_FeedEol_Neq(
    Lexer * lex
) {
    return FsmRes_UnexpectedByte;
}

static
inline
FsmRes
Lexer_FeedEol_GtOrGte(
    Lexer * lex
) {
    if (PushNormalToken(lex->seq, TokTag_RightBrace) == false) {
        return FsmRes_NoMemory;
    }

    lex->stat = FsmStat_Idle;

    return FsmRes_Ok;
}

static
inline
FsmRes
Lexer_FeedEol_LtOrLte(
    Lexer * lex
) {
    if (PushNormalToken(lex->seq, TokTag_LeftBrace) == false) {
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

    case FsmStat_NumLit:
        res = Lexer_FeedEol_Num(lex);
        break;

    case FsmStat_StrLit:
        res = Lexer_FeedEol_StrLit(lex);
        break;

    case FsmStat_AssignOrEqu:
        res = Lexer_FeedEol_AsgOrEqu(lex);
        break;

    case FsmStat_Neq:
        res = Lexer_FeedEol_Neq(lex);
        break;

    case FsmStat_RightBraceOrGte:
        res = Lexer_FeedEol_GtOrGte(lex);
        break;

    case FsmStat_LeftBraceOrLte:
        res = Lexer_FeedEol_LtOrLte(lex);
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
    Lexer * lex,
    TokSeq ** seq
) {
    switch (Lexer_FeedEol(lex)) {
    case FsmRes_Ok:
        break;

    case FsmRes_Again:
    case FsmRes_NoMemory:
    case FsmRes_UnexpectedByte:
        return false;
    }

    if (PushNormalToken(lex->seq, TokTag_Eof) == false) {
        return false;
    }

    TokSeq * new_seq = TokSeq_New();
    if (new_seq == NULL) {
        return false;
    }

    TokSeq * res_seq = lex->seq;
    lex->seq = new_seq;
    *seq = res_seq;

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