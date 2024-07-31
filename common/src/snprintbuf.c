/*
 * PICOCOM PROPRIETARY INFORMATION
 *
 * This software is supplied under the terms of a license agreement or
 * nondisclosure agreement with PICOCOM and may not be copied
 * or disclosed except in accordance with the terms of that agreement.
 *
 * Copyright PICOCOM.
 */

#include <stdarg.h>
#include <stdio.h>

#include "cmmTypes.h"

int snprintfBuf(CmmBuf_s* buf, unsigned spare, const char* format, ...)
{
    int rev;
    unsigned i;
    va_list _argv;
    va_start(_argv, format);
    if (buf->offset + (4 * spare) >= buf->octCnt - 100) {
        return -1;
    }
    for (i = 0; i < spare; i++) {
        sprintf((char*)buf->data + buf->offset, "    ");
        buf->offset += 4;
    }

    if (buf->octCnt <= buf->offset) {
        return -1;
    }

    rev = vsnprintf((char*)buf->data + buf->offset, (buf->octCnt - buf->offset - 101), format, _argv);
    if (rev > 0) {
        buf->offset += rev;

    } else {
        buf->offset = buf->octCnt;
    }

    va_end(_argv);
    return rev;
}
