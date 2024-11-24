#include <string.h>

#include "greatest.h"
#include "menos.h"
#include "util/flex_buf.h"

TEST CreateEmptyBuffer(void) {
    FlexBuf * buf = FlexBuf_New();
    ASSERT_NEQ(NULL, buf);
    ASSERT_EQ_FMT(NULL, FlexBuf_Data(buf), "%p");
    ASSERT_EQ_FMT(0UL, FlexBuf_Capacity(buf), "%zu");
    ASSERT_EQ_FMT(0UL, FlexBuf_Size(buf), "%zu");

    FlexBuf_Free(buf);

    PASS();
}

TEST PushByte(void) {
    FlexBuf * buf = FlexBuf_New();
    ASSERT_NEQ(NULL, buf);
    ASSERT_EQ_FMT(NULL, FlexBuf_Data(buf), "%p");
    ASSERT_EQ_FMT(0UL, FlexBuf_Capacity(buf), "%zu");
    ASSERT_EQ_FMT(0UL, FlexBuf_Size(buf), "%zu");

    const u8 BYTE = 0xAA;
    const usize LEN = sizeof(BYTE);

    ASSERT(FlexBuf_PushByte(buf, BYTE));
    ASSERT_NEQ(NULL, FlexBuf_Data(buf));
    ASSERT_NEQ(0UL, FlexBuf_Capacity(buf));
    ASSERT_EQ_FMT(LEN, FlexBuf_Size(buf), "%zu");
    ASSERT_MEM_EQ(&BYTE, FlexBuf_Data(buf), LEN);

    FlexBuf_Free(buf);

    PASS();
}

TEST PushBuffer(void) {
    FlexBuf * buf = FlexBuf_New();
    ASSERT_NEQ(NULL, buf);
    ASSERT_EQ_FMT(NULL, FlexBuf_Data(buf), "%p");
    ASSERT_EQ_FMT(0UL, FlexBuf_Capacity(buf), "%zu");
    ASSERT_EQ_FMT(0UL, FlexBuf_Size(buf), "%zu");

    const u8 BUF_1[] = { 0xAA, 0x55, 0xAA, 0x55 };
    const usize LEN_1 = sizeof(BUF_1);

    ASSERT(FlexBuf_PushBuf(buf, BUF_1, LEN_1));
    ASSERT_NEQ(NULL, FlexBuf_Data(buf));
    ASSERT_NEQ(0UL, FlexBuf_Capacity(buf));
    ASSERT_EQ_FMT(LEN_1, FlexBuf_Size(buf), "%zu");
    ASSERT_MEM_EQ(BUF_1, FlexBuf_Data(buf), LEN_1);

    const char * BUF_2 = "Hello";
    const usize LEN_2 = strlen(BUF_2);

    const char BUF_3[] = "\xAA\x55\xAA\x55Hello";
    const usize LEN_3 = LEN_1 + LEN_2;

    ASSERT(FlexBuf_PushBuf(buf, BUF_2, LEN_2));
    ASSERT_NEQ(NULL, FlexBuf_Data(buf));
    ASSERT_NEQ(0UL, FlexBuf_Capacity(buf));
    ASSERT_EQ_FMT(LEN_3, FlexBuf_Size(buf), "%zu");
    ASSERT_MEM_EQ(BUF_3, FlexBuf_Data(buf), LEN_3);

    FlexBuf_Free(buf);

    PASS();
}


SUITE(FlexBufSuite) {
    RUN_TEST(CreateEmptyBuffer);
    RUN_TEST(PushByte);
    RUN_TEST(PushBuffer);
}