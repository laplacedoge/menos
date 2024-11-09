#include <string.h>
#include <stdarg.h>
#include <stdio.h>

#include "flex_buf.h"
#include "memory/allocate.h"
#include "menos.h"
#include "util/fixed_buf.h"

typedef struct _FlexBuf {
    u8 * buf;
    usize cap;
    usize len;
} FlexBuf;

static const usize INIT_CAP = 16;

FlexBuf *
FlexBuf_New(void) {
    FlexBuf * obj = (FlexBuf *)MeMem_Malloc(sizeof(FlexBuf));
    if (obj == NULL) {
        return NULL;
    }

    obj->buf = NULL;
    obj->cap = 0;
    obj->len = 0;

    return obj;
}

/**
 * @brief Appends a byte to the end of a FlexBuf.
 *
 * This function adds a single byte (`byte`) to the end of the FlexBuf (`obj`).
 * If there is insufficient space in the buffer, it reallocates memory to
 * accommodate the new byte. The buffer's length is updated accordingly.
 *
 * @param obj A pointer to the FlexBuf to which the byte will be appended.
 * @param byte The byte to be added to the FlexBuf.
 *
 * @return `true` if the byte is successfully appended, `false` if memory
 *         allocation fails.
 */
bool
FlexBuf_PushByte(
    FlexBuf * obj,
    u8 byte
) {
    usize new_len = obj->len + 1;

    if (new_len > obj->cap) {
        usize new_cap = new_len;
        u8 * new_buf;

        if (new_cap < INIT_CAP) {
            new_cap = INIT_CAP;
        } else {
            new_cap = (new_cap & (~0x7UL)) << 1;
        }

        if (obj->buf == NULL) {
            new_buf = MeMem_Malloc(new_cap);
        } else {
            new_buf = MeMem_Realloc(obj->buf, new_cap);
        }

        if (new_buf == NULL) {
            return false;
        }

        obj->buf = new_buf;
        obj->cap = new_cap;
    }

    obj->buf[obj->len] = byte;
    obj->len += 1;

    return true;
}

/**
 * @brief Appends multiple copies of a byte to the end of a FlexBuf.
 *
 * This function adds a specified number (`cnt`) of copies of a byte (`byte`)
 * to the end of the FlexBuf (`obj`). If there is insufficient space in the
 * buffer, it reallocates memory to accommodate the new bytes. The buffer's
 * length is updated accordingly.
 *
 * @param obj A pointer to the FlexBuf to which the bytes will be appended.
 * @param byte The byte to be duplicated and added to the FlexBuf.
 * @param cnt The number of times to duplicate the byte and append it.
 *
 * @return `true` if the bytes are successfully appended, `false` if memory
 *         allocation fails.
 */
bool
FlexBuf_PushDupByte(
    FlexBuf * obj,
    u8 byte,
    usize cnt
) {
    usize new_len = obj->len + cnt;

    if (new_len > obj->cap) {
        usize new_cap = new_len;
        u8 * new_buf;

        if (new_cap < INIT_CAP) {
            new_cap = INIT_CAP;
        } else {
            new_cap = (new_cap & (~0x7UL)) << 1;
        }

        if (obj->buf == NULL) {
            new_buf = MeMem_Malloc(new_cap);
        } else {
            new_buf = MeMem_Realloc(obj->buf, new_cap);
        }

        if (new_buf == NULL) {
            return false;
        }

        obj->buf = new_buf;
        obj->cap = new_cap;
    }

    memset(obj->buf + obj->len, byte, cnt);
    obj->len += cnt;

    return true;
}

/**
 * @brief Appends a block of data to the end of a FlexBuf.
 *
 * This function adds a specified block of data (`buf` with length `len`) to
 * the end of the FlexBuf (`obj`). If there is insufficient space in the buffer,
 * it reallocates memory to accommodate the new data. The buffer's length is
 * updated accordingly.
 *
 * @param obj A pointer to the FlexBuf to which the data will be appended.
 * @param buf A pointer to the block of data to be added to the FlexBuf.
 * @param len The length of the data block to be added.
 *
 * @return `true` if the data is successfully appended, `false` if memory
 *         allocation fails.
 */
bool
FlexBuf_PushBuf(
    FlexBuf * obj,
    const void * buf,
    usize len
) {
    if (len == 0) {
        return true;
    }

    usize new_len = obj->len + len;

    if (new_len > obj->cap) {
        usize new_cap = new_len;
        u8 * new_buf;

        if (new_cap < INIT_CAP) {
            new_cap = INIT_CAP;
        } else {
            new_cap = (new_cap & (~0x7UL)) << 1;
        }

        if (obj->buf == NULL) {
            new_buf = MeMem_Malloc(new_cap);
        } else {
            new_buf = MeMem_Realloc(obj->buf, new_cap);
        }

        if (new_buf == NULL) {
            return false;
        }

        obj->buf = new_buf;
        obj->cap = new_cap;
    }

    memcpy(obj->buf + obj->len, buf, len);
    obj->len += len;

    return true;
}

/**
 * @brief Appends a null-terminated string to the end of a FlexBuf.
 *
 * This function appends a string (`str`) to the end of the FlexBuf (`obj`).
 * It automatically calculates the length of the string and then calls
 * `FlexBuf_PushBuf` to append it. The string is assumed to be null-terminated.
 *
 * @param obj A pointer to the FlexBuf to which the string will be appended.
 * @param str A pointer to the null-terminated string to be added to the FlexBuf.
 *
 * @return `true` if the string is successfully appended, `false` if memory
 *         allocation fails.
 */
bool
FlexBuf_PushStr(
    FlexBuf * obj,
    const char * str
) {
    return FlexBuf_PushBuf(obj, str, strlen(str));
}

/**
 * @brief Appends a string including its null terminator to the end of a
 *        FlexBuf.
 *
 * This function appends a string (`str`) and its null terminator to the end
 * of the FlexBuf (`obj`). The length of the string, including the null byte,
 * is used when calling `FlexBuf_PushBuf`.
 *
 * @param obj A pointer to the FlexBuf to which the string (with null byte)
 *            will be appended.
 * @param str A pointer to the null-terminated string to be added to the
 *            FlexBuf.
 *
 * @return `true` if the string and null terminator are successfully appended,
 *         `false` if memory allocation fails.
 */
bool
FlexBuf_PushStrWithNull(
    FlexBuf * obj,
    const char * str
) {
    return FlexBuf_PushBuf(obj, str, strlen(str) + 1);
}

/**
 * @brief Appends a formatted string to the end of a FlexBuf.
 *
 * This function formats the given string (`fmt`) with the provided arguments
 * and appends it to the end of the FlexBuf (`obj`). The formatted string is
 * first stored in a temporary buffer (or dynamically allocated if too large)
 * before being added to the FlexBuf.
 *
 * @param obj A pointer to the FlexBuf to which the formatted string will be
 *            appended.
 * @param fmt A format string that specifies how subsequent arguments are
 *            formatted.
 * @param ... Additional arguments to be formatted according to `fmt`.
 *
 * @return `true` if the formatted string is successfully appended, `false`
 *         if memory allocation or appending fails.
 */
bool
FlexBuf_PushFmt(
    FlexBuf * obj,
    const char * fmt,
    ...
) {
    char mini_buf[256];
    char * buf = mini_buf;
    va_list args;
    bool res;
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

    res = FlexBuf_PushBuf(obj, buf, len);

    if (buf != mini_buf) {
        MeMem_Free(buf);
    }

    return res;
}

/**
 * @brief Retrieves the data buffer of a FlexBuf.
 *
 * This function returns a pointer to the internal buffer (`buf`) of the
 * FlexBuf (`obj`), which holds the actual data stored in the FlexBuf.
 *
 * @param obj A pointer to the FlexBuf from which the data buffer is retrieved.
 *
 * @return A pointer to the internal buffer of the FlexBuf.
 */
u8 *
FlexBuf_Data(
    FlexBuf * obj
) {
    return obj->buf;
}

/**
 * @brief Retrieves the capacity of a FlexBuf.
 *
 * This function returns the current capacity (`cap`) of the FlexBuf (`obj`),
 * indicating the total amount of memory allocated for the buffer.
 *
 * @param obj A pointer to the FlexBuf from which the capacity is retrieved.
 *
 * @return The current capacity of the FlexBuf.
 */
usize
FlexBuf_Capacity(
    FlexBuf * obj
) {
    return obj->cap;
}

/**
 * @brief Retrieves the size of a FlexBuf.
 *
 * This function returns the current size (`len`) of the FlexBuf (`obj`),
 * indicating the amount of data currently stored in the buffer.
 *
 * @param obj A pointer to the FlexBuf from which the size is retrieved.
 *
 * @return The current size of the FlexBuf.
 */
usize
FlexBuf_Size(
    FlexBuf * obj
) {
    return obj->len;
}

/**
 * @brief Compacts a FlexBuf by resizing its buffer to fit its content.
 *
 * This function reduces the capacity of the FlexBuf's buffer (`obj`) to
 * match its current length, freeing unused memory and keeping only the
 * necessary portion. If the buffer is already compact (capacity equals
 * length) or if it is empty, no action is taken.
 *
 * @param obj A pointer to the FlexBuf to be compacted.
 *
 * @return `true` if the compaction is successful or no compaction is needed,
 *         `false` if memory reallocation fails.
 */
bool
FlexBuf_Compact(
    FlexBuf * obj
) {
    if (obj->buf == NULL) {
        return true;
    }

    if (obj->len == obj->cap) {
        return true;
    }

    u8 * new_buf = (u8 *)MeMem_Realloc(obj->buf, obj->len);
    if (new_buf == NULL) {
        return false;
    }

    obj->buf = new_buf;
    obj->cap = obj->len;

    return true;
}

/**
 * @brief Merges the contents of one FlexBuf into another.
 *
 * This function appends the contents of the source FlexBuf (`src_obj`)
 * to the destination FlexBuf (`dst_obj`). The source buffer remains
 * unchanged, while the destination buffer grows to include the
 * additional data.
 *
 * @param dst_obj A pointer to the destination FlexBuf, which will receive
 *                the contents of `src_obj`.
 * @param src_obj A pointer to the source FlexBuf, whose contents will be
 *                appended to `dst_obj`.
 *
 * @return `true` if the merge is successful, `false` otherwise.
 */
bool
FlexBuf_Merge(
    FlexBuf * dst_obj,
    FlexBuf * src_obj
) {
    return FlexBuf_PushBuf(dst_obj, src_obj->buf, src_obj->len);
}

/**
 * @brief Joins two FlexBuf objects into a new FlexBuf.
 *
 * This function creates a new FlexBuf object that contains the
 * concatenated contents of two existing FlexBuf objects, `obj_1` and
 * `obj_2`. If memory allocation for the new buffer fails or if there
 * is an error in concatenation, the function returns `NULL`.
 *
 * @param obj_1 A pointer to the first FlexBuf to be joined.
 * @param obj_2 A pointer to the second FlexBuf to be joined.
 *
 * @return A pointer to the newly created FlexBuf containing the
 *         concatenated contents of `obj_1` and `obj_2`, or `NULL`
 *         if an error occurs.
 */
FlexBuf *
FlexBuf_Join(
    FlexBuf * obj_1,
    FlexBuf * obj_2
) {
    FlexBuf * obj = FlexBuf_New();
    if (obj == NULL) {
        goto Exit;
    }

    if (FlexBuf_PushBuf(obj, obj_1->buf, obj_1->len) == false ||
        FlexBuf_PushBuf(obj, obj_2->buf, obj_2->len) == false) {

        goto FreeBuf;
    }

    return obj;

FreeBuf:
    FlexBuf_Free(obj);

Exit:
    return NULL;
}

/**
 * @brief Converts a FlexBuf to a FixedBuf.
 *
 * This function creates a new FixedBuf object using the contents of the
 * given FlexBuf (`obj`). The new FixedBuf will have the same data as the
 * FlexBuf, but with a fixed size corresponding to the FlexBuf's length.
 *
 * @param obj A pointer to the FlexBuf to be converted.
 *
 * @return A pointer to a newly created FixedBuf containing the contents
 *         of the FlexBuf, or `NULL` if memory allocation fails.
 */
FixedBuf *
FlexBuf_ToFixedBuf(
    FlexBuf * obj
) {
    return FixedBuf_NewFromBuf(obj->buf, obj->len);
}

void
FlexBuf_Free(
    FlexBuf * obj
) {
    if (obj->buf != NULL) {
        MeMem_Free(obj->buf);
    }

    MeMem_Free(obj);
}