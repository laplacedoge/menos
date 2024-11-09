#ifndef __ME_UTIL_FLEX_BUF_H__
#define __ME_UTIL_FLEX_BUF_H__

#include "menos.h"
#include "fixed_buf.h"

typedef struct _FlexBuf FlexBuf;

FlexBuf *
FlexBuf_New(void);

bool
FlexBuf_PushByte(
    FlexBuf * obj,
    u8 byte
);

bool
FlexBuf_PushDupByte(
    FlexBuf * obj,
    u8 byte,
    usize cnt
);

bool
FlexBuf_PushBuf(
    FlexBuf * obj,
    const void * buf,
    usize len
);

bool
FlexBuf_PushStr(
    FlexBuf * obj,
    const char * str
);

bool
FlexBuf_PushStrWithNull(
    FlexBuf * obj,
    const char * str
);

bool
FlexBuf_PushFmt(
    FlexBuf * obj,
    const char * fmt,
    ...
);

u8 *
FlexBuf_Data(
    FlexBuf * obj
);

usize
FlexBuf_Capacity(
    FlexBuf * obj
);

usize
FlexBuf_Size(
    FlexBuf * obj
);

bool
FlexBuf_Compact(
    FlexBuf * obj
);

bool
FlexBuf_Merge(
    FlexBuf * dst_obj,
    FlexBuf * src_obj
);

FlexBuf *
FlexBuf_Join(
    FlexBuf * obj_1,
    FlexBuf * obj_2
);

FixedBuf *
FlexBuf_ToFixedBuf(
    FlexBuf * obj
);

void
FlexBuf_Free(
    FlexBuf * obj
);

#endif