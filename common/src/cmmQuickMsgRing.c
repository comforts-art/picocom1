/*
 * PICOCOM PROPRIETARY INFORMATION
 *
 * This software is supplied under the terms of a license agreement or
 * nondisclosure agreement with PICOCOM and may not be copied
 * or disclosed except in accordance with the terms of that agreement.
 *
 * Copyright PICOCOM.
 */

#include <assert.h>
#include <cmmQuickMsgRing.h>
#include <stdint.h>

#ifndef NULL
#define NULL ((void*)0)
#endif

#define ASSERT(n) assert(n)

#ifndef likely
#define likely(x) __builtin_expect(!!(x), 1)
#endif /* likely */

#ifndef unlikely
#define unlikely(x) __builtin_expect(!!(x), 0)
#endif /* unlikely */

static inline int isPowerOf2(uint32_t n)
{
    return n && !(n & (n - 1));
}

QuickMsgRing_t* QuickMsgRingInit(uint64_t* buf, uint32_t wordCount)
{
    QuickMsgRing_t* r;
    ASSERT(buf);
    ASSERT((wordCount >> 29) == 0);
    ASSERT(isPowerOf2(wordCount));

    r = (QuickMsgRing_t*)buf;
    r->size = wordCount;
    r->mask = wordCount - 1;
    r->in = 0;
    r->out = 0;
    r->nextIn = 0;
    r->nextOut = 0;
    return r;
}

int QuickMsgRingPrePutMsgBurst(QuickMsgRing_t* r, uint64_t** msg, uint16_t* wordCount, int msgCount)
{
    uint64_t* buf;
    uint32_t topUnused;
    uint32_t unused;
    uint32_t tmpIn;
    int n = 0;
    int idx;

    ASSERT(msg);

    buf = (uint64_t*)(r + 1);
    tmpIn = r->in;
    while (likely(n < msgCount)) {
        unused = r->size - tmpIn + r->out;

        /*
         * Enusre that  we sample the r->out index before we
         * start allocating msg buffer.
         */

        if (unlikely(unused < *wordCount)) {
            break;
        }

        topUnused = r->size - (tmpIn & r->mask);
        if (likely(topUnused >= *wordCount)) {
            idx = tmpIn & r->mask;
            buf[idx] = *wordCount;
            *msg = buf + idx;
            tmpIn += *wordCount;
            n++;
            msg++;
            wordCount++;
        } else if (unused >= (topUnused + *wordCount)) {
            buf[tmpIn & r->mask] = (MSG_ID_DUMMY << 16) | topUnused;
            buf[0] = *wordCount;
            *msg = buf;
            tmpIn += (topUnused + *wordCount);
            n++;
            msg++;
            wordCount++;
        } else {
            break;
        }
    }

    r->nextIn = tmpIn;
    return n;
}

int QuickMsgRingPrePutOneMsg(QuickMsgRing_t* r, uint64_t** msg, uint16_t wordCount)
{
    return QuickMsgRingPrePutMsgBurst(r, msg, &wordCount, 1);
}

int QuickMsgRingPreGetMsgBurst(QuickMsgRing_t* r, uint64_t** msg, int maxMsgCount)
{
    uint64_t* buf;
    Msg_t* tmpMsg;
    uint32_t tmpOut;
    int n = 0;

    buf = (uint64_t*)(r + 1);
    tmpOut = r->out;
    while (likely(n < maxMsgCount)) {
        if (tmpOut == r->in) {
            break;
        }
        /*
         * Ensure that we sample the r->in index before
         * we start getting new msg.
         */

        tmpMsg = (Msg_t*)(buf + (tmpOut & r->mask));
        if (tmpMsg->msgType == MSG_ID_DUMMY) {
            tmpOut += tmpMsg->wordCount;
            continue;
        }

        *msg = (uint64_t*)tmpMsg;
        tmpOut += tmpMsg->wordCount;
        msg++;
        n++;
    }

    r->nextOut = tmpOut;
    return n;
}

int QuickMsgRingPreGetOneMsg(QuickMsgRing_t* r, uint64_t** msg)
{
    return QuickMsgRingPreGetMsgBurst(r, msg, 1);
}

int QuickMsgRingPutOneMsg(QuickMsgRing_t* r, uint16_t msgWordSz, int (*pFunc)(Msg_t* pMsg, void* pArg), void* pArg)
{
    uint64_t* pmsg;
    int ret;

    ret = QuickMsgRingPrePutOneMsg(r, &pmsg, msgWordSz);
    if (1 != ret)
        return -1;

    ret = pFunc((Msg_t*)pmsg, pArg);
    if (0 != ret)
        return ret;

    QuickMsgRingPutMsgComplete(r);
    return 0;
}

int QuickMsgRingGetOneMsg(QuickMsgRing_t* r, int (*pFunc)(Msg_t* pMsg, void* pArg), void* pArg)
{
    uint64_t* pmsg;
    int ret;

    ret = QuickMsgRingPreGetOneMsg(r, &pmsg);
    if (1 != ret)
        return ret;

    ret = pFunc((Msg_t*)pmsg, pArg);
    if (0 != ret)
        return ret;

    QuickMsgRingGetMsgComplete(r);
    return 0;
}