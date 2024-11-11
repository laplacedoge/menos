#ifndef __ME_UTIL_FIXED_BUF_H__
#define __ME_UTIL_FIXED_BUF_H__

#include "menos.h"

typedef struct _FixedBuf FixedBuf;

FixedBuf *
FixedBuf_NewWithLen(
    usize len
);

FixedBuf *
FixedBuf_NewFromBuf(
    const void * buf,
    usize len
);

FixedBuf *
FixedBuf_NewFromStr(
    const char * str
);

FixedBuf *
FixedBuf_NewFromStrWithNull(
    const char * str
);

FixedBuf *
FixedBuf_NewFromFmt(
    const char * fmt,
    ...
);

FixedBuf *
FixedBuf_NewFromFile(
    const char * path
);

u8 *
FixedBuf_Data(
    FixedBuf * buf
);

usize
FixedBuf_Size(
    FixedBuf * buf
);

FixedBuf *
FixedBuf_Escape(
    FixedBuf * buf
);

void
FixedBuf_Strip(
    FixedBuf * buf
);

FixedBuf *
FixedBuf_Clone(
    FixedBuf * buf
);

FixedBuf *
FixedBuf_Join(
    FixedBuf * buf_1,
    FixedBuf * buf_2
);

void
FixedBuf_Free(
    FixedBuf * buf
);

#endif