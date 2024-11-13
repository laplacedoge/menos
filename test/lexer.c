#include <string.h>

#include "greatest.h"
#include "menos.h"
#include "lexer/lexer.h"

const char *
TokTag_ToStr_Intf(
    int tag
) {
    return TokTag_ToStr((TokTag)tag);
}

#define ASSERT_TOK_TAG_EQ(expected, actual) \
    ASSERT_ENUM_EQ(expected, actual, TokTag_ToStr_Intf);

TEST NameTokens(void) {
    const char * INPUT_STR = " __cache__ VAR_2 47agent ak47 api32sucks";
    const usize INPUT_LEN = strlen(INPUT_STR);
    const usize NUM_TOKS = 6 + 1;

    Lexer * lex = Lexer_New();
    ASSERT_NEQ(NULL, lex);

    TokSeq * seq;
    ASSERT(Lexer_Feed(lex, INPUT_STR, INPUT_LEN));
    ASSERT(Lexer_Finalize(lex, &seq));

    ASSERT_NEQ(NULL, seq);
    ASSERT_EQ_FMT(NUM_TOKS, TokSeq_Count(seq), "%zu");

    Token * tok;

    const char * TOK_STR;
    usize tok_len;
    usize tok_idx = 0;

    TOK_STR = "__cache__";
    tok_len = strlen(TOK_STR);
    ASSERT_NEQ(NULL, tok = TokSeq_At(seq, tok_idx));
    ASSERT_TOK_TAG_EQ(TokTag_Name, tok->tag);
    ASSERT_EQ_FMT(tok_len, FixedBuf_Size(tok->ext.name.str), "%zu");
    ASSERT_MEM_EQ(TOK_STR, FixedBuf_Data(tok->ext.name.str), tok_len);
    tok_idx += 1;

    TOK_STR = "VAR_2";
    tok_len = strlen(TOK_STR);
    ASSERT_NEQ(NULL, tok = TokSeq_At(seq, tok_idx));
    ASSERT_TOK_TAG_EQ(TokTag_Name, tok->tag);
    ASSERT_EQ_FMT(tok_len, FixedBuf_Size(tok->ext.name.str), "%zu");
    ASSERT_MEM_EQ(TOK_STR, FixedBuf_Data(tok->ext.name.str), tok_len);
    tok_idx += 1;

    ASSERT_NEQ(NULL, tok = TokSeq_At(seq, tok_idx));
    ASSERT_TOK_TAG_EQ(TokTag_NumLit, tok->tag);
    ASSERT_EQ_FMT(47UL, tok->ext.num_lit.val, "%zu");
    tok_idx += 1;

    TOK_STR = "agent";
    tok_len = strlen(TOK_STR);
    ASSERT_NEQ(NULL, tok = TokSeq_At(seq, tok_idx));
    ASSERT_TOK_TAG_EQ(TokTag_Name, tok->tag);
    ASSERT_EQ_FMT(tok_len, FixedBuf_Size(tok->ext.name.str), "%zu");
    ASSERT_MEM_EQ(TOK_STR, FixedBuf_Data(tok->ext.name.str), tok_len);
    tok_idx += 1;

    TOK_STR = "ak47";
    tok_len = strlen(TOK_STR);
    ASSERT_NEQ(NULL, tok = TokSeq_At(seq, tok_idx));
    ASSERT_TOK_TAG_EQ(TokTag_Name, tok->tag);
    ASSERT_EQ_FMT(tok_len, FixedBuf_Size(tok->ext.name.str), "%zu");
    ASSERT_MEM_EQ(TOK_STR, FixedBuf_Data(tok->ext.name.str), tok_len);
    tok_idx += 1;

    TOK_STR = "api32sucks";
    tok_len = strlen(TOK_STR);
    ASSERT_NEQ(NULL, tok = TokSeq_At(seq, tok_idx));
    ASSERT_TOK_TAG_EQ(TokTag_Name, tok->tag);
    ASSERT_EQ_FMT(tok_len, FixedBuf_Size(tok->ext.name.str), "%zu");
    ASSERT_MEM_EQ(TOK_STR, FixedBuf_Data(tok->ext.name.str), tok_len);
    tok_idx += 1;

    TokSeq_Free(seq);

    Lexer_Free(lex);

    PASS();
}

SUITE(LexerSuite) {
    RUN_TEST(NameTokens);
}