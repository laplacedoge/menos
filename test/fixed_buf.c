#include <string.h>

#include "greatest.h"
#include "menos.h"
#include "util/fixed_buf.h"

TEST CreateWithZeroLength(void) {
    FixedBuf * buf = FixedBuf_NewWithLen(0);
    ASSERT_NEQ(NULL, buf);
    ASSERT_EQ_FMT(NULL, FixedBuf_Data(buf), "%p");
    ASSERT_EQ_FMT(0UL, FixedBuf_Size(buf), "%zu");

    FixedBuf_Free(buf);

    PASS();
}

TEST CreateFromBuffer(void) {
    const u8 BUF[] = {
        0xAA, 0x55, 0xAA, 0x55, 0xAA, 0x55, 0xAA, 0x55,
        0xAA, 0x55, 0xAA, 0x55, 0xAA, 0x55, 0xAA, 0x55,
        0xAA, 0x55, 0xAA, 0x55, 0xAA, 0x55, 0xAA, 0x55,
        0xAA, 0x55, 0xAA, 0x55, 0xAA, 0x55, 0xAA, 0x55,
    };

    FixedBuf * buf = FixedBuf_NewFromBuf(BUF, sizeof(BUF));
    ASSERT_NEQ(NULL, buf);
    ASSERT_NEQ(NULL, FixedBuf_Data(buf));
    ASSERT_EQ_FMT(sizeof(BUF), FixedBuf_Size(buf), "%zu");
    ASSERT_MEM_EQ(BUF, FixedBuf_Data(buf), sizeof(BUF));

    FixedBuf_Free(buf);

    PASS();
}

TEST CreateFromString(void) {
    const char * STR = "Hello, fixed buffer!";
    usize len = strlen(STR);

    FixedBuf * buf = FixedBuf_NewFromStr(STR);
    ASSERT_NEQ(NULL, buf);
    ASSERT_NEQ(NULL, FixedBuf_Data(buf));
    ASSERT_EQ_FMT(len, FixedBuf_Size(buf), "%zu");
    ASSERT_MEM_EQ(STR, FixedBuf_Data(buf), len);

    FixedBuf_Free(buf);

    PASS();
}

TEST CreateFromStringWithNull(void) {
    const char * STR = "¡Porque menos es más!";
    usize len = strlen(STR) + 1;

    FixedBuf * buf = FixedBuf_NewFromStrWithNull(STR);
    ASSERT_NEQ(NULL, buf);
    ASSERT_NEQ(NULL, FixedBuf_Data(buf));
    ASSERT_EQ_FMT(len, FixedBuf_Size(buf), "%zu");
    ASSERT_MEM_EQ(STR, FixedBuf_Data(buf), len);

    FixedBuf_Free(buf);

    PASS();
}

TEST CreateFromFormat(void) {
    const char * FMT = "%s: %zu";
    const char * ATTR = "version";
    const usize ATTR_LEN = strlen(ATTR);
    const usize VER = 47;
    const usize VER_LEN = 2;
    const usize BUF_LEN = ATTR_LEN + 2 + VER_LEN;

    FixedBuf * buf = FixedBuf_NewFromFmt(FMT, ATTR, VER);
    ASSERT_NEQ(NULL, buf);
    ASSERT_NEQ(NULL, FixedBuf_Data(buf));
    ASSERT_EQ_FMT(BUF_LEN, FixedBuf_Size(buf), "%zu");
    ASSERT_MEM_EQ(ATTR, FixedBuf_Data(buf), ATTR_LEN);
    ASSERT_MEM_EQ("47", FixedBuf_Data(buf) + ATTR_LEN + 2, VER_LEN);

    FixedBuf_Free(buf);

    PASS();
}

TEST StripLeftSide(void) {
    const char * SRC_STR = "  \t \r\r \n Hello";
    const char * DST_STR = "Hello";
    const usize DST_LEN = strlen(DST_STR);

    FixedBuf * buf = FixedBuf_NewFromStr(SRC_STR);
    ASSERT_NEQ(NULL, buf);

    FixedBuf_Strip(buf);
    ASSERT_NEQ(NULL, FixedBuf_Data(buf));
    ASSERT_EQ_FMT(DST_LEN, FixedBuf_Size(buf), "%zu");

    FixedBuf_Free(buf);

    PASS();
}

TEST StripRightSide(void) {
    const char * SRC_STR = "Hello  \t \r\r \n ";
    const char * DST_STR = "Hello";
    const usize DST_LEN = strlen(DST_STR);

    FixedBuf * buf = FixedBuf_NewFromStr(SRC_STR);
    ASSERT_NEQ(NULL, buf);

    FixedBuf_Strip(buf);
    ASSERT_NEQ(NULL, FixedBuf_Data(buf));
    ASSERT_EQ_FMT(DST_LEN, FixedBuf_Size(buf), "%zu");

    FixedBuf_Free(buf);

    PASS();
}

TEST StripBothSide(void) {
    const char * SRC_STR = "  \t \r\r \n Hello  \t \r\r \n ";
    const char * DST_STR = "Hello";
    const usize DST_LEN = strlen(DST_STR);

    FixedBuf * buf = FixedBuf_NewFromStr(SRC_STR);
    ASSERT_NEQ(NULL, buf);

    FixedBuf_Strip(buf);
    ASSERT_NEQ(NULL, FixedBuf_Data(buf));
    ASSERT_EQ_FMT(DST_LEN, FixedBuf_Size(buf), "%zu");

    FixedBuf_Free(buf);

    PASS();
}

TEST StripWhitespaceString(void) {
    const char * SRC_STR = "  \t \r\r \n \t \r\r \n ";

    FixedBuf * buf = FixedBuf_NewFromStr(SRC_STR);
    ASSERT_NEQ(NULL, buf);

    FixedBuf_Strip(buf);
    ASSERT_EQ(NULL, FixedBuf_Data(buf));
    ASSERT_EQ_FMT(0UL, FixedBuf_Size(buf), "%zu");

    FixedBuf_Free(buf);

    PASS();
}

TEST StripEmptyString(void) {
    FixedBuf * buf = FixedBuf_NewWithLen(0);
    ASSERT_NEQ(NULL, buf);

    FixedBuf_Strip(buf);
    ASSERT_EQ(NULL, FixedBuf_Data(buf));
    ASSERT_EQ_FMT(0UL, FixedBuf_Size(buf), "%zu");

    FixedBuf_Free(buf);

    PASS();
}

TEST CloneBuffer(void) {
    const char * STR = "Hello world!";
    const usize LEN = strlen(STR);

    FixedBuf * original_buf = FixedBuf_NewFromStr(STR);
    ASSERT_NEQ(NULL, original_buf);

    FixedBuf * cloned_buf = FixedBuf_Clone(original_buf);
    ASSERT_NEQ(NULL, cloned_buf);
    ASSERT_NEQ(NULL, FixedBuf_Data(cloned_buf));
    ASSERT_EQ_FMT(LEN, FixedBuf_Size(cloned_buf), "%zu");
    ASSERT_MEM_EQ(STR, FixedBuf_Data(cloned_buf), LEN);

    FixedBuf_Free(cloned_buf);
    FixedBuf_Free(original_buf);

    PASS();
}

TEST JoinBuffer(void) {
    const char * STR_1 = "master";
    const usize LEN_1 = strlen(STR_1);
    const char * STR_2 = "piece";
    const usize LEN_2 = strlen(STR_2);
    const char * STR_3 = "masterpiece";
    const usize LEN_3 = strlen(STR_3);

    FixedBuf * buf_1 = FixedBuf_NewFromStr(STR_1);
    ASSERT_NEQ(NULL, buf_1);

    FixedBuf * buf_2 = FixedBuf_NewFromStr(STR_2);
    ASSERT_NEQ(NULL, buf_2);

    FixedBuf * buf_3 = FixedBuf_Join(buf_1, buf_2);
    ASSERT_NEQ(NULL, buf_3);

    ASSERT_NEQ(NULL, FixedBuf_Data(buf_3));
    ASSERT_EQ_FMT(LEN_3, FixedBuf_Size(buf_3), "%zu");
    ASSERT_MEM_EQ(STR_3, FixedBuf_Data(buf_3), LEN_3);

    FixedBuf_Free(buf_3);
    FixedBuf_Free(buf_2);
    FixedBuf_Free(buf_1);

    PASS();
}

SUITE(FixedBufSuite) {
    RUN_TEST(CreateWithZeroLength);
    RUN_TEST(CreateFromBuffer);
    RUN_TEST(CreateFromString);
    RUN_TEST(CreateFromStringWithNull);
    RUN_TEST(CreateFromFormat);
    RUN_TEST(StripLeftSide);
    RUN_TEST(StripRightSide);
    RUN_TEST(StripBothSide);
    RUN_TEST(StripWhitespaceString);
    RUN_TEST(StripEmptyString);
    RUN_TEST(CloneBuffer);
    RUN_TEST(JoinBuffer);
}