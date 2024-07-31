/*
 * PICOCOM PROPRIETARY INFORMATION
 *
 * This software is supplied under the terms of a license agreement or
 * nondisclosure agreement with PICOCOM and may not be copied
 * or disclosed except in accordance with the terms of that agreement.
 *
 * Copyright PICOCOM.
 */

#ifndef __QUICKMSGRING_H__
#define __QUICKMSGRING_H__

#include <stdint.h>

#define MEM_CACHE_LINE_SIZE 64

#define MSG_ID_DUMMY 0

struct stQuickMsgRing_t {
    uint32_t size;
    uint32_t mask;
    uint32_t nextIn;
    uint32_t nextOut;
    volatile uint32_t in;
    volatile uint32_t out;
} __attribute__((__aligned__(MEM_CACHE_LINE_SIZE)));

typedef struct stQuickMsgRing_t QuickMsgRing_t;

#define QUICK_MSG_BUF_SIZE(u64Cnt) (sizeof(QuickMsgRing_t) + sizeof(uint64_t) * (u64Cnt))

typedef union {
    uint64_t head;
    struct
        {
        uint16_t wordCount; //number of uint64_t, including msg header and body
        uint16_t msgType;
        uint32_t reseverd;
        uint64_t msgBody[0];
    };
} Msg_t;

#define MSG_U64_CNT(msgType) (sizeof(msgType) / sizeof(uint64_t))

static inline uint32_t QuickMsgRingUsed(QuickMsgRing_t* r)
{
    return (r->in - r->out);
}

static inline uint32_t QuickMsgRingUnused(QuickMsgRing_t* r)
{
    return (r->size - r->in + r->out);
}

QuickMsgRing_t* QuickMsgRingInit(uint64_t* buf, uint32_t wordCount);

int QuickMsgRingPrePutMsgBurst(QuickMsgRing_t* r, uint64_t** msg, uint16_t* wordCount, int msgCount);

static inline void QuickMsgRingPutMsgComplete(QuickMsgRing_t* r)
{
    /*
     * Enusre that allocate the msg buffer before
     * we update the r->in index;
     */

    r->in = r->nextIn;
}

int QuickMsgRingPrePutOneMsg(QuickMsgRing_t* r, uint64_t** msg, uint16_t wordCount);

int QuickMsgRingPreGetMsgBurst(QuickMsgRing_t* r, uint64_t** msg, int maxMsgCount);

static inline void QuickMsgRingGetMsgComplete(QuickMsgRing_t* r)
{
    /*
     * Ensure that we get msge from the ring before
     * we update the r->out index;
     */

    r->out = r->nextOut;
}

int QuickMsgRingPreGetOneMsg(QuickMsgRing_t* r, uint64_t** msg);

int QuickMsgRingPutOneMsg(QuickMsgRing_t* r, uint16_t msgWordSz, int (*pFunc)(Msg_t* pMsg, void* pArg), void* pArg);
int QuickMsgRingGetOneMsg(QuickMsgRing_t* r, int (*pFunc)(Msg_t* pMsg, void* pArg), void* pArg);

#endif