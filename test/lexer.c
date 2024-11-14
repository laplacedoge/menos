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
    ASSERT_EQ_FMT(0UL, Token_Row(tok), "%zu");
    ASSERT_EQ_FMT(1UL, Token_Column(tok), "%zu");
    ASSERT_EQ_FMT(9UL, Token_Length(tok), "%zu");
    ASSERT_TOK_TAG_EQ(TokTag_Name, tok->tag);
    ASSERT_EQ_FMT(tok_len, FixedBuf_Size(tok->ext.name.str), "%zu");
    ASSERT_MEM_EQ(TOK_STR, FixedBuf_Data(tok->ext.name.str), tok_len);
    tok_idx += 1;

    TOK_STR = "VAR_2";
    tok_len = strlen(TOK_STR);
    ASSERT_NEQ(NULL, tok = TokSeq_At(seq, tok_idx));
    ASSERT_EQ_FMT(0UL, Token_Row(tok), "%zu");
    ASSERT_EQ_FMT(11UL, Token_Column(tok), "%zu");
    ASSERT_EQ_FMT(5UL, Token_Length(tok), "%zu");
    ASSERT_TOK_TAG_EQ(TokTag_Name, tok->tag);
    ASSERT_EQ_FMT(tok_len, FixedBuf_Size(tok->ext.name.str), "%zu");
    ASSERT_MEM_EQ(TOK_STR, FixedBuf_Data(tok->ext.name.str), tok_len);
    tok_idx += 1;

    ASSERT_NEQ(NULL, tok = TokSeq_At(seq, tok_idx));
    ASSERT_EQ_FMT(0UL, Token_Row(tok), "%zu");
    ASSERT_EQ_FMT(17UL, Token_Column(tok), "%zu");
    ASSERT_EQ_FMT(2UL, Token_Length(tok), "%zu");
    ASSERT_TOK_TAG_EQ(TokTag_NumLit, tok->tag);
    ASSERT_EQ_FMT(47UL, tok->ext.num_lit.val, "%zu");
    tok_idx += 1;

    TOK_STR = "agent";
    tok_len = strlen(TOK_STR);
    ASSERT_NEQ(NULL, tok = TokSeq_At(seq, tok_idx));
    ASSERT_EQ_FMT(0UL, Token_Row(tok), "%zu");
    ASSERT_EQ_FMT(19UL, Token_Column(tok), "%zu");
    ASSERT_EQ_FMT(5UL, Token_Length(tok), "%zu");
    ASSERT_TOK_TAG_EQ(TokTag_Name, tok->tag);
    ASSERT_EQ_FMT(tok_len, FixedBuf_Size(tok->ext.name.str), "%zu");
    ASSERT_MEM_EQ(TOK_STR, FixedBuf_Data(tok->ext.name.str), tok_len);
    tok_idx += 1;

    TOK_STR = "ak47";
    tok_len = strlen(TOK_STR);
    ASSERT_NEQ(NULL, tok = TokSeq_At(seq, tok_idx));
    ASSERT_EQ_FMT(0UL, Token_Row(tok), "%zu");
    ASSERT_EQ_FMT(25UL, Token_Column(tok), "%zu");
    ASSERT_EQ_FMT(4UL, Token_Length(tok), "%zu");
    ASSERT_TOK_TAG_EQ(TokTag_Name, tok->tag);
    ASSERT_EQ_FMT(tok_len, FixedBuf_Size(tok->ext.name.str), "%zu");
    ASSERT_MEM_EQ(TOK_STR, FixedBuf_Data(tok->ext.name.str), tok_len);
    tok_idx += 1;

    TOK_STR = "api32sucks";
    tok_len = strlen(TOK_STR);
    ASSERT_NEQ(NULL, tok = TokSeq_At(seq, tok_idx));
    ASSERT_EQ_FMT(0UL, Token_Row(tok), "%zu");
    ASSERT_EQ_FMT(30UL, Token_Column(tok), "%zu");
    ASSERT_EQ_FMT(10UL, Token_Length(tok), "%zu");
    ASSERT_TOK_TAG_EQ(TokTag_Name, tok->tag);
    ASSERT_EQ_FMT(tok_len, FixedBuf_Size(tok->ext.name.str), "%zu");
    ASSERT_MEM_EQ(TOK_STR, FixedBuf_Data(tok->ext.name.str), tok_len);
    tok_idx += 1;

    ASSERT_NEQ(NULL, tok = TokSeq_At(seq, tok_idx));
    ASSERT_EQ_FMT(0UL, Token_Row(tok), "%zu");
    ASSERT_EQ_FMT(40UL, Token_Column(tok), "%zu");
    ASSERT_EQ_FMT(0UL, Token_Length(tok), "%zu");
    ASSERT_TOK_TAG_EQ(TokTag_Eof, tok->tag);

    TokSeq_Free(seq);

    Lexer_Free(lex);

    PASS();
}

TEST ComparisonOperatorTokens(void) {
    const char * INPUT_STR = " == = != > < >= <= ";
    const usize INPUT_LEN = strlen(INPUT_STR);
    const usize NUM_TOKS = 7 + 1;

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

    ASSERT_NEQ(NULL, tok = TokSeq_At(seq, tok_idx));
    ASSERT_EQ_FMT(0UL, Token_Row(tok), "%zu");
    ASSERT_EQ_FMT(1UL, Token_Column(tok), "%zu");
    ASSERT_EQ_FMT(2UL, Token_Length(tok), "%zu");
    ASSERT_TOK_TAG_EQ(TokTag_Equ, tok->tag);
    tok_idx += 1;

    ASSERT_NEQ(NULL, tok = TokSeq_At(seq, tok_idx));
    ASSERT_EQ_FMT(0UL, Token_Row(tok), "%zu");
    ASSERT_EQ_FMT(4UL, Token_Column(tok), "%zu");
    ASSERT_EQ_FMT(1UL, Token_Length(tok), "%zu");
    ASSERT_TOK_TAG_EQ(TokTag_Assign, tok->tag);
    tok_idx += 1;

    ASSERT_NEQ(NULL, tok = TokSeq_At(seq, tok_idx));
    ASSERT_EQ_FMT(0UL, Token_Row(tok), "%zu");
    ASSERT_EQ_FMT(6UL, Token_Column(tok), "%zu");
    ASSERT_EQ_FMT(2UL, Token_Length(tok), "%zu");
    ASSERT_TOK_TAG_EQ(TokTag_Neq, tok->tag);
    tok_idx += 1;

    ASSERT_NEQ(NULL, tok = TokSeq_At(seq, tok_idx));
    ASSERT_EQ_FMT(0UL, Token_Row(tok), "%zu");
    ASSERT_EQ_FMT(9UL, Token_Column(tok), "%zu");
    ASSERT_EQ_FMT(1UL, Token_Length(tok), "%zu");
    ASSERT_TOK_TAG_EQ(TokTag_GreaterThan, tok->tag);
    tok_idx += 1;

    ASSERT_NEQ(NULL, tok = TokSeq_At(seq, tok_idx));
    ASSERT_EQ_FMT(0UL, Token_Row(tok), "%zu");
    ASSERT_EQ_FMT(11UL, Token_Column(tok), "%zu");
    ASSERT_EQ_FMT(1UL, Token_Length(tok), "%zu");
    ASSERT_TOK_TAG_EQ(TokTag_LessThan, tok->tag);
    tok_idx += 1;

    ASSERT_NEQ(NULL, tok = TokSeq_At(seq, tok_idx));
    ASSERT_EQ_FMT(0UL, Token_Row(tok), "%zu");
    ASSERT_EQ_FMT(13UL, Token_Column(tok), "%zu");
    ASSERT_EQ_FMT(2UL, Token_Length(tok), "%zu");
    ASSERT_TOK_TAG_EQ(TokTag_Gte, tok->tag);
    tok_idx += 1;

    ASSERT_NEQ(NULL, tok = TokSeq_At(seq, tok_idx));
    ASSERT_EQ_FMT(0UL, Token_Row(tok), "%zu");
    ASSERT_EQ_FMT(16UL, Token_Column(tok), "%zu");
    ASSERT_EQ_FMT(2UL, Token_Length(tok), "%zu");
    ASSERT_TOK_TAG_EQ(TokTag_Lte, tok->tag);
    tok_idx += 1;

    ASSERT_NEQ(NULL, tok = TokSeq_At(seq, tok_idx));
    ASSERT_EQ_FMT(0UL, Token_Row(tok), "%zu");
    ASSERT_EQ_FMT(19UL, Token_Column(tok), "%zu");
    ASSERT_EQ_FMT(0UL, Token_Length(tok), "%zu");
    ASSERT_TOK_TAG_EQ(TokTag_Eof, tok->tag);

    TokSeq_Free(seq);

    Lexer_Free(lex);

    PASS();
}

TEST AllKindsOfBracketsTokens(void) {
    const char * INPUT_STR = " () [] {} <> ";
    const usize INPUT_LEN = strlen(INPUT_STR);
    const usize NUM_TOKS = 8 + 1;

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

    ASSERT_NEQ(NULL, tok = TokSeq_At(seq, tok_idx));
    ASSERT_EQ_FMT(0UL, Token_Row(tok), "%zu");
    ASSERT_EQ_FMT(1UL, Token_Column(tok), "%zu");
    ASSERT_EQ_FMT(1UL, Token_Length(tok), "%zu");
    ASSERT_TOK_TAG_EQ(TokTag_LeftParen, tok->tag);
    tok_idx += 1;

    ASSERT_NEQ(NULL, tok = TokSeq_At(seq, tok_idx));
    ASSERT_EQ_FMT(0UL, Token_Row(tok), "%zu");
    ASSERT_EQ_FMT(2UL, Token_Column(tok), "%zu");
    ASSERT_EQ_FMT(1UL, Token_Length(tok), "%zu");
    ASSERT_TOK_TAG_EQ(TokTag_RightParen, tok->tag);
    tok_idx += 1;

    ASSERT_NEQ(NULL, tok = TokSeq_At(seq, tok_idx));
    ASSERT_EQ_FMT(0UL, Token_Row(tok), "%zu");
    ASSERT_EQ_FMT(4UL, Token_Column(tok), "%zu");
    ASSERT_EQ_FMT(1UL, Token_Length(tok), "%zu");
    ASSERT_TOK_TAG_EQ(TokTag_LeftBracket, tok->tag);
    tok_idx += 1;

    ASSERT_NEQ(NULL, tok = TokSeq_At(seq, tok_idx));
    ASSERT_EQ_FMT(0UL, Token_Row(tok), "%zu");
    ASSERT_EQ_FMT(5UL, Token_Column(tok), "%zu");
    ASSERT_EQ_FMT(1UL, Token_Length(tok), "%zu");
    ASSERT_TOK_TAG_EQ(TokTag_RightBracket, tok->tag);
    tok_idx += 1;

    ASSERT_NEQ(NULL, tok = TokSeq_At(seq, tok_idx));
    ASSERT_EQ_FMT(0UL, Token_Row(tok), "%zu");
    ASSERT_EQ_FMT(7UL, Token_Column(tok), "%zu");
    ASSERT_EQ_FMT(1UL, Token_Length(tok), "%zu");
    ASSERT_TOK_TAG_EQ(TokTag_LeftBrace, tok->tag);
    tok_idx += 1;

    ASSERT_NEQ(NULL, tok = TokSeq_At(seq, tok_idx));
    ASSERT_EQ_FMT(0UL, Token_Row(tok), "%zu");
    ASSERT_EQ_FMT(8UL, Token_Column(tok), "%zu");
    ASSERT_EQ_FMT(1UL, Token_Length(tok), "%zu");
    ASSERT_TOK_TAG_EQ(TokTag_RightBrace, tok->tag);
    tok_idx += 1;

    ASSERT_NEQ(NULL, tok = TokSeq_At(seq, tok_idx));
    ASSERT_EQ_FMT(0UL, Token_Row(tok), "%zu");
    ASSERT_EQ_FMT(10UL, Token_Column(tok), "%zu");
    ASSERT_EQ_FMT(1UL, Token_Length(tok), "%zu");
    ASSERT_TOK_TAG_EQ(TokTag_LessThan, tok->tag);
    tok_idx += 1;

    ASSERT_NEQ(NULL, tok = TokSeq_At(seq, tok_idx));
    ASSERT_EQ_FMT(0UL, Token_Row(tok), "%zu");
    ASSERT_EQ_FMT(11UL, Token_Column(tok), "%zu");
    ASSERT_EQ_FMT(1UL, Token_Length(tok), "%zu");
    ASSERT_TOK_TAG_EQ(TokTag_GreaterThan, tok->tag);
    tok_idx += 1;

    ASSERT_NEQ(NULL, tok = TokSeq_At(seq, tok_idx));
    ASSERT_EQ_FMT(0UL, Token_Row(tok), "%zu");
    ASSERT_EQ_FMT(13UL, Token_Column(tok), "%zu");
    ASSERT_EQ_FMT(0UL, Token_Length(tok), "%zu");
    ASSERT_TOK_TAG_EQ(TokTag_Eof, tok->tag);

    TokSeq_Free(seq);

    Lexer_Free(lex);

    PASS();
}

TEST ScanMultiLineInput(void) {
    const char * INPUT_STR =
        "   if age >= 18 { \r\n"
        "  \n"
        "  title = \"adult\";  \n"
        "} \r\n";
    const usize INPUT_LEN = strlen(INPUT_STR);
    const usize NUM_TOKS = 10 + 1;

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

    ASSERT_NEQ(NULL, tok = TokSeq_At(seq, tok_idx));
    ASSERT_EQ_FMT(0UL, Token_Row(tok), "%zu");
    ASSERT_EQ_FMT(3UL, Token_Column(tok), "%zu");
    ASSERT_EQ_FMT(2UL, Token_Length(tok), "%zu");
    ASSERT_TOK_TAG_EQ(TokTag_If, tok->tag);
    tok_idx += 4;

    ASSERT_NEQ(NULL, tok = TokSeq_At(seq, tok_idx));
    ASSERT_EQ_FMT(0UL, Token_Row(tok), "%zu");
    ASSERT_EQ_FMT(16UL, Token_Column(tok), "%zu");
    ASSERT_EQ_FMT(1UL, Token_Length(tok), "%zu");
    ASSERT_TOK_TAG_EQ(TokTag_LeftBrace, tok->tag);
    tok_idx += 1;

    ASSERT_NEQ(NULL, tok = TokSeq_At(seq, tok_idx));
    ASSERT_EQ_FMT(2UL, Token_Row(tok), "%zu");
    ASSERT_EQ_FMT(2UL, Token_Column(tok), "%zu");
    ASSERT_EQ_FMT(5UL, Token_Length(tok), "%zu");
    ASSERT_TOK_TAG_EQ(TokTag_Name, tok->tag);
    tok_idx += 4;

    ASSERT_NEQ(NULL, tok = TokSeq_At(seq, tok_idx));
    ASSERT_EQ_FMT(3UL, Token_Row(tok), "%zu");
    ASSERT_EQ_FMT(0UL, Token_Column(tok), "%zu");
    ASSERT_EQ_FMT(1UL, Token_Length(tok), "%zu");
    ASSERT_TOK_TAG_EQ(TokTag_RightBrace, tok->tag);
    tok_idx += 1;

    ASSERT_NEQ(NULL, tok = TokSeq_At(seq, tok_idx));
    ASSERT_EQ_FMT(4UL, Token_Row(tok), "%zu");
    ASSERT_EQ_FMT(0UL, Token_Column(tok), "%zu");
    ASSERT_EQ_FMT(0UL, Token_Length(tok), "%zu");
    ASSERT_TOK_TAG_EQ(TokTag_Eof, tok->tag);

    TokSeq_Free(seq);

    Lexer_Free(lex);

    PASS();
}

SUITE(LexerSuite) {
    RUN_TEST(NameTokens);
    RUN_TEST(ComparisonOperatorTokens);
    RUN_TEST(AllKindsOfBracketsTokens);
    RUN_TEST(ScanMultiLineInput);
}