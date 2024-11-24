#include <sys/stat.h>
#include <unistd.h>
#include <stdarg.h>
#include <string.h>
#include <stdio.h>
#include <fcntl.h>

#include "fixed_buf.h"
#include "memory/allocate.h"
#include "flex_buf.h"
#include "menos.h"

typedef struct _FixedBuf {
    u8 * buf;
    usize len;
} FixedBuf;

FixedBuf *
FixedBuf_NewWithLen(
    usize len
) {
    u8 * buf;

    if (len == 0) {
        buf = NULL;
    } else {
        buf = (u8 *)MeMem_Malloc(len);
        if (buf == NULL) {
            return NULL;
        }
    }

    FixedBuf * obj = (FixedBuf *)MeMem_Malloc(sizeof(FixedBuf));
    if (obj == NULL) {
        goto FreeBuf;
    }

    obj->len = len;
    obj->buf = buf;

    return obj;

FreeBuf:
    if (buf != NULL) {
        MeMem_Free(buf);
    }

Exit:
    return NULL;
}

FixedBuf *
FixedBuf_NewFromBuf(
    const void * buf,
    usize len
) {
    FixedBuf * obj = FixedBuf_NewWithLen(len);
    if (obj == NULL) {
        return NULL;
    }

    if (len != 0) {
        memcpy(obj->buf, buf, len);
    }

    return obj;
}

FixedBuf *
FixedBuf_NewFromStr(
    const char * str
) {
    return FixedBuf_NewFromBuf(str, strlen(str));
}

FixedBuf *
FixedBuf_NewFromStrWithNull(
    const char * str
) {
    return FixedBuf_NewFromBuf(str, strlen(str) + 1);
}

FixedBuf *
FixedBuf_NewFromFmt(
    const char * fmt,
    ...
) {
    char mini_buf[256];
    char * buf = mini_buf;
    va_list args;
    FixedBuf * obj;
    int len;

    va_start(args, fmt);

    len = vsnprintf(NULL, 0, fmt, args);

    if (len > sizeof(mini_buf)) {
        buf = (char *)MeMem_Malloc(len + 1);
        if (buf == NULL) {
            va_end(args);
            return false;
        }
    }

    va_end(args);

    va_start(args, fmt);

    vsnprintf(buf, len + 1, fmt, args);

    va_end(args);

    obj = FixedBuf_NewFromBuf(buf, len);

    if (buf != mini_buf) {
        MeMem_Free(buf);
    }

    return obj;
}

FixedBuf *
FixedBuf_NewFromFile(
    const char * path
) {
    int fd = open(path, O_RDONLY);
    if (fd < 0) {
        goto Exit;
    }

    struct stat file_stat;
    if (fstat(fd, &file_stat) != 0) {
        goto CloseFile;
    }

    usize file_size = (usize)file_stat.st_size;

    FixedBuf * obj = FixedBuf_NewWithLen(file_size);
    if (obj == NULL) {
        goto CloseFile;
    }

    ssize_t read_len = read(fd, obj->buf, file_size);
    if (read_len != (ssize_t)file_size) {
        goto FreeBuf;
    }

    close(fd);

    return obj;

FreeBuf:
    FixedBuf_Free(obj);

CloseFile:
    close(fd);

Exit:
    return NULL;
}

u8 *
FixedBuf_Data(
    FixedBuf * obj
) {
    return obj->buf;
}

usize
FixedBuf_Size(
    FixedBuf * obj
) {
    return obj->len;
}

static
FlexBuf *
FixedBuf_ToEscapedFlexBuf(
    FixedBuf * obj
) {
    FlexBuf * dst_obj = FlexBuf_New();
    if (dst_obj == NULL) {
        goto Exit;
    }

    bool res;

    for (usize i = 0; i < obj->len; i++) {
        u8 byte = obj->buf[i];

        if (byte >= 0x20 &&
            byte <= 0x7E) {

            if (byte == '\\') {
                res = FlexBuf_PushBuf(dst_obj, "\\\\", 2);
            } else if (byte == '"') {
                res = FlexBuf_PushBuf(dst_obj, "\\\"", 2);
            } else {
                res = FlexBuf_PushByte(dst_obj, byte);
            }
        } else {
            if (byte == '\t') {
                res = FlexBuf_PushBuf(dst_obj, "\\t", 2);
            } else if (byte == '\n') {
                res = FlexBuf_PushBuf(dst_obj, "\\n", 2);
            } else if (byte == '\r') {
                res = FlexBuf_PushBuf(dst_obj, "\\r", 2);
            } else {
                const char * CHAR_MAP = "0123456789ABCDEF";
                u8 buf[4] = { '\\', 'x', '0', '0' };

                buf[2] = CHAR_MAP[byte >> 4];
                buf[3] = CHAR_MAP[byte & 0x0F];

                res = FlexBuf_PushBuf(dst_obj, buf, sizeof(buf));
            }
        }

        if (res == false) {
            goto FreeDst;
        }
    }

    return dst_obj;

FreeDst:
    FlexBuf_Free(dst_obj);

Exit:
    return NULL;
}

FixedBuf *
FixedBuf_Escape(
    FixedBuf * obj
) {
    FlexBuf * tmp_obj = FixedBuf_ToEscapedFlexBuf(obj);
    if (tmp_obj == NULL) {
        goto Exit;
    }

    FixedBuf * new_obj = FixedBuf_NewFromBuf(
        FlexBuf_Data(tmp_obj), FlexBuf_Size(tmp_obj));

    FlexBuf_Free(tmp_obj);

Exit:
    return new_obj;
}

void
FixedBuf_Strip(
    FixedBuf * obj
) {
    if (obj->buf == NULL) {
        return;
    }

    usize left_idx = 0;

    while (left_idx != obj->len) {
        u8 byte = obj->buf[left_idx];

        if (byte == ' ' ||
            byte == '\t' ||
            byte == '\r' ||
            byte == '\n') {

            left_idx += 1;
        } else {
            break;
        }
    }

    if (left_idx == obj->len) {
        MeMem_Free(obj->buf);
        obj->buf = NULL;
        obj->len = 0;
        return;
    }

    usize right_idx = obj->len - 1;

    while (right_idx != left_idx) {
        u8 byte = obj->buf[right_idx];

        if (byte == ' ' ||
            byte == '\t' ||
            byte == '\r' ||
            byte == '\n') {

            right_idx -= 1;
        } else {
            break;
        }
    }

    if (right_idx == left_idx) {
        MeMem_Free(obj->buf);
        obj->buf = NULL;
        obj->len = 0;
        return;
    }

    usize new_len = (right_idx + 1) - left_idx;

    memmove(obj->buf, obj->buf + left_idx, new_len);
    obj->len = new_len;
}

FixedBuf *
FixedBuf_Clone(
    FixedBuf * obj
) {
    u8 * buf;
    usize len;

    if (obj->buf != NULL) {
        buf = (u8 *)MeMem_Malloc(obj->len);
        if (buf == NULL) {
            goto Exit;
        }

        memcpy(buf, obj->buf, obj->len);
        len = obj->len;
    } else {
        buf = NULL;
        len = 0;
    }

    FixedBuf * new_obj = (FixedBuf *)MeMem_Malloc(sizeof(FixedBuf));
    if (new_obj == NULL) {
        goto FreeBuffer;
    }

    new_obj->buf = buf;
    new_obj->len = len;

    return new_obj;

FreeBuffer:
    if (buf != NULL) {
        MeMem_Free(buf);
    }

Exit:
    return NULL;
}

FixedBuf *
FixedBuf_Join(
    FixedBuf * obj_1,
    FixedBuf * obj_2
) {
    usize len = obj_1->len + obj_2->len;
    FixedBuf * buf = FixedBuf_NewWithLen(len);
    if (buf == NULL) {
        return NULL;
    }

    memcpy(buf->buf, obj_1->buf, obj_1->len);
    memcpy(buf->buf + obj_1->len, obj_2->buf, obj_2->len);

    return buf;
}

void
FixedBuf_Clear(
    FixedBuf * obj
) {
    if (obj->buf == NULL) {
        return;
    }

    MeMem_Free(obj->buf);
    obj->buf = NULL;
    obj->len = 0;
}

void
FixedBuf_Free(
    FixedBuf * obj
) {
    if (obj->buf != NULL) {
        MeMem_Free(obj->buf);
    }

    MeMem_Free(obj);
}