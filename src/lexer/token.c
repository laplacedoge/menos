#include <string.h>

#include "token.h"
#include "menos.h"
#include "memory/allocate.h"
#include "util/fixed_buf.h"
#include "util/flex_buf.h"

const char *
TokTag_ToStr(
    TokTag tag
) {
    switch (tag) {
    case TokTag_Let: return "let";
    case TokTag_Assign: return "=";
    case TokTag_Equ: return "==";
    case TokTag_Neq: return "!=";
    case TokTag_GreaterThan: return ">";
    case TokTag_Gte: return ">=";
    case TokTag_LessThan: return "<";
    case TokTag_Lte: return "<=";
    case TokTag_LeftParen: return "(";
    case TokTag_RightParen: return ")";
    case TokTag_LeftBracket: return "[";
    case TokTag_RightBracket: return "]";
    case TokTag_LeftBrace: return "{";
    case TokTag_RightBrace: return "}";
    case TokTag_Semicolon: return ";";
    case TokTag_Name: return "Name";
    case TokTag_NumLit: return "NumericLiteral";
    case TokTag_StrLit: return "StringLiteral";
    case TokTag_Eof: return "EOF";
    }
}

void
Token_Init(
    Token * tok
) {
    memset(tok, 0, sizeof(Token));
}

void
Token_InitWithTag(
    Token * tok,
    TokTag tag
) {
    Token_Init(tok);
    tok->tag = tag;
}

bool
Token_PushAsStr(
    Token * tok,
    FlexBuf * buf
) {
    do {
        switch (tok->tag) {
        case TokTag_Name:
            return FlexBuf_PushFmt(buf, "<%s \"%.*s\">",
                TokTag_ToStr(tok->tag),
                FixedBuf_Size(tok->ext.name.str),
                FixedBuf_Data(tok->ext.name.str));

        case TokTag_NumLit:
            return FlexBuf_PushFmt(buf, "<%s %zu>",
                TokTag_ToStr(tok->tag),
                tok->ext.num_lit.val);

        case TokTag_StrLit: {
            FixedBuf * escaped_str =
                FixedBuf_Escape(tok->ext.str_lit.str);
            if (escaped_str == NULL) {
                return false;
            }

            bool res = FlexBuf_PushFmt(buf, "<%s \"%.*s\">",
                TokTag_ToStr(tok->tag),
                FixedBuf_Size(escaped_str),
                FixedBuf_Data(escaped_str));

            FixedBuf_Free(escaped_str);

            return res;
        }

        default:
            break;
        }
    } while (false);

    if (tok->tag == TokTag_Eof) {
        const char * str = TokTag_ToStr(tok->tag);
        return FlexBuf_PushFmt(buf, "<Keyword %s>", str);
    }

    const char * str = TokTag_ToStr(tok->tag);
    return FlexBuf_PushFmt(buf, "<Keyword '%s'>", str);
}

static
void
Token_Free(
    Token * tok
) {
    switch (tok->tag) {
    case TokTag_Let:
    case TokTag_Assign:
    case TokTag_Equ:
    case TokTag_Neq:
    case TokTag_GreaterThan:
    case TokTag_Gte:
    case TokTag_LessThan:
    case TokTag_Lte:
    case TokTag_LeftParen:
    case TokTag_RightParen:
    case TokTag_LeftBracket:
    case TokTag_RightBracket:
    case TokTag_LeftBrace:
    case TokTag_RightBrace:
    case TokTag_Semicolon:
        break;

    case TokTag_Name:
        FixedBuf_Free(tok->ext.name.str);
        break;

    case TokTag_NumLit:
        break;

    case TokTag_StrLit:
        FixedBuf_Free(tok->ext.str_lit.str);
        break;

    case TokTag_Eof:
        break;
    }
}

typedef struct _TokSeq {
    FlexBuf * buf;
    usize num;
} TokSeq;

TokSeq *
TokSeq_New(void) {
    FlexBuf * buf = FlexBuf_New();
    if (buf == NULL) {
        goto Exit;
    }

    TokSeq * seq = (TokSeq *)MeMem_Malloc(sizeof(TokSeq));
    if (seq == NULL) {
        goto FreeFlexBuf;
    }

    seq->buf = buf;
    seq->num = 0;

    return seq;

FreeFlexBuf:
    FlexBuf_Free(buf);

Exit:
    return NULL;
}

bool
TokSeq_Push(
    TokSeq * seq,
    Token * tok
) {
    if (FlexBuf_PushBuf(seq->buf, tok, sizeof(Token)) == false) {
        return false;
    }

    seq->num += 1;

    return true;
}

Token *
TokSeq_Data(
    TokSeq * seq
) {
    return (Token *)FlexBuf_Data(seq->buf);
}

usize
TokSeq_Count(
    TokSeq * seq
) {
    return seq->num;
}

Token *
TokSeq_At(
    TokSeq * seq,
    usize idx
) {
    if (idx >= seq->num) {
        return NULL;
    }

    return (Token *)FlexBuf_Data(seq->buf) + idx;
}

bool
TokSeq_Compact(
    TokSeq * seq
) {
    return FlexBuf_Compact(seq->buf);
}

/**
 * @brief Formats a TokSeq object as a string and appends it to a FlexBuf.
 *
 * This function takes a TokSeq object and formats it as a structured string
 * representation, appending the formatted output to a provided FlexBuf buffer.
 * The format includes an optional line break and indentation after each token
 * based on the specified indentation level.
 *
 * @param seq A pointer to the TokSeq object to be formatted.
 * @param buf A pointer to the FlexBuf to which the formatted string will be
 *            appended.
 * @param ind Indentation indicator:
 *            - If `ind == 0`, the output will be compact with no line breaks.
 *            - If `ind > 0`, tokens are printed on new lines with `ind` spaces
 *              at the start of each line.
 *            - If `ind < 0`, tokens are printed on new lines with no additional
 *              indentation.
 *
 * @return `true` if the operation is successful, `false` if an error occurs
 *         (e.g., memory allocation failure).
 */
bool
TokSeq_PushAsStr(
    TokSeq * seq,
    FlexBuf * buf,
    ssize ind
) {
    bool res = false;

    FlexBuf * tmp = FlexBuf_New();
    if (tmp == NULL) {
        goto Exit;
    }

    if (FlexBuf_PushFmt(tmp, "<TokSeq(%zu)", seq->num) == false) {
        goto FreeTmp;
    }

    Token * buf_toks = TokSeq_Data(seq);
    usize num_toks = TokSeq_Count(seq);

    if (num_toks != 0) {
        if (FlexBuf_PushStr(tmp, ": [") == false) {
            goto FreeTmp;
        }

        if ((ind >= 0 && FlexBuf_PushByte(tmp, '\n') == false) ||
            (ind > 0 && FlexBuf_PushDupByte(tmp, ' ', (usize)ind) == false)) {
            goto FreeTmp;
        }

        usize i;

        for (i = 0; i < num_toks - 1; i++) {
            if (Token_PushAsStr(buf_toks + i, tmp) == false ||
                FlexBuf_PushStr(tmp, ", ") == false ||
                (ind >= 0 && FlexBuf_PushByte(tmp, '\n') == false) ||
                (ind > 0 &&
                 FlexBuf_PushDupByte(tmp, ' ', (usize)ind) == false)) {
                goto FreeTmp;
            }
        }

        if (Token_PushAsStr(buf_toks + i, tmp) == false ||
            (ind >= 0 && FlexBuf_PushByte(tmp, '\n') == false)) {
            goto FreeTmp;
        }
    }

    if (FlexBuf_PushStr(tmp, "]>") == false ||
        FlexBuf_Merge(buf, tmp) == false) {
        goto FreeTmp;
    }

    res = true;

FreeTmp:
    FlexBuf_Free(tmp);

Exit:
    return res;
}

static
void
TokSeq_FreeTokens(
    TokSeq * seq
) {
    Token * buf_toks = (Token *)FlexBuf_Data(seq->buf);
    usize num_toks = seq->num;

    for (usize i = 0; i < num_toks; i++) {
        Token_Free(buf_toks + i);
    }
}

void
TokSeq_Free(
    TokSeq * seq
) {
    TokSeq_FreeTokens(seq);
    FlexBuf_Free(seq->buf);
    MeMem_Free(seq);
}