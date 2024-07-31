/*
 * PICOCOM PROPRIETARY INFORMATION
 *
 * This software is supplied under the terms of a license agreement or
 * nondisclosure agreement with PICOCOM and may not be copied
 * or disclosed except in accordance with the terms of that agreement.
 *
 * Copyright PICOCOM.
 */

#ifndef __QUICKMSGRING_PR_H__
#define __QUICKMSGRING_PR_H__
#include "quickMsgRing_pu.h"
#ifdef __cplusplus
extern "C" {
#endif /*__cplusplus*/
#ifndef SUCCESS
#define SUCCESS 0
#endif
#ifndef FAILURE
#define FAILURE -1
#endif
#ifndef MEMMEMSET
#define MEMMEMSET memset
#endif
#ifndef MEMMEMCPY
#define MEMMEMCPY(a, b, c)                   \
    do {                                     \
        if (NULL != a && NULL != b && c > 0) \
            memcpy(a, b, c);                 \
    } while (0)
#endif
#ifndef MEMMALLOC
#define MEMMALLOC malloc
#endif
#ifndef MEMFREE
#define MEMFREE(p)   \
    do {             \
        if (p)       \
            free(p); \
        p = NULL;    \
    } while (0)
#endif
#ifdef WIN32
#define __FILENAME__ strrchr(__FILE__, '\\') ? strrchr(__FILE__, '\\') + 1 : __FILE__
#elif defined(LINUX)
#define __FILENAME__ strrchr(__FILE__, '/') ? strrchr(__FILE__, '/') + 1 : __FILE__
#endif
#ifdef WIN32
#ifndef OUT_PRINT
#define OUT_PRINT(a, level, format, ...)                                                                        \
    do {                                                                                                        \
        if ((a)->pfnPrint) {                                                                                    \
            (a)->pfnPrint(__FILENAME__, __LINE__, __FUNCTION__, (a)->pCbPrintPara, level, format, __VA_ARGS__); \
        }                                                                                                       \
    } while (0)
#endif
#elif defined(LINUX)
#ifndef OUT_PRINT
#define OUT_PRINT(a, level, format...)                                                               \
    do {                                                                                             \
        if ((a)->pfnPrint) {                                                                         \
            (a)->pfnPrint(__FILENAME__, __LINE__, __FUNCTION__, (a)->pCbPrintPara, level, ##format); \
        }                                                                                            \
    } while (0)
#endif
#endif

#ifdef WIN32
#define inline __inline
#endif

#ifndef NULL
#define NULL ((void*)0)
#endif

#ifdef DEBUG
#define ASSERT(n) assert(n)
#else
#define ASSERT(n)
#endif

#ifdef WIN32
#ifndef likely
#define likely(x) (!!(x))
#endif /* likely */
#ifndef unlikely
#define unlikely(x) (!!(x))
#endif /* unlikely */
#else
#ifndef likely
#define likely(x) __builtin_expect(!!(x), 1)
#endif /* likely */
#ifndef unlikely
#define unlikely(x) __builtin_expect(!!(x), 0)
#endif /* unlikely */
#endif

#define RESERV_LEN 1

struct stQuickMsgRing;
typedef struct stQuickMsgRing {
    uint32_t* buf;
    uint32_t flag;
    uint32_t emptyFlag;
    uint32_t size;
    uint32_t nextIn;
    uint32_t nextOut;
    /*tail*/
    volatile uint32_t in;
    /*header*/
    volatile uint32_t out;
    void* pCbPrintPara;
    P_QUICKMSGRING_OUTPUT_PRINTFUNC pfnPrint;
} stQuickMsgRing;

static int QuickMsgRingSendMsg(stQuickMsgRingH* pThis, char* pMsg, uint32_t msgLen);
static void QuickMsgRingRecvMsgComplete(stQuickMsgRingH* pThis);
static int QuickMsgRingGetBuf(stQuickMsgRingH* pThis, uint32_t** msg, uint32_t wordCount);
static int QuickMsgRingBeginRecvMsg(stQuickMsgRingH* pThis, uint32_t** msg, uint32_t* msgLen);
static int QuickMsgRingSendMsgWithPackFunc(stQuickMsgRingH* pThis, uint32_t msgWordSz, int (*pFunc)(void* pArg, void* pMsg), void* pArg);
static int QuickMsgRingRecvMsgWithUnpackFunc(stQuickMsgRingH* pThis, int (*pFunc)(void* pArg, void* pMsg, uint32_t msgLen), void* pArg);
static int QuickMsgRingGetBufBulk(stQuickMsgRingH* pThis, uint32_t** msg, uint32_t* wordCount, int msgCount);
static int QuickMsgRingBeginRecvMsgBulk(stQuickMsgRingH* pThis, uint32_t** msg, uint32_t* msgLen, int maxMsgCount);
static inline int isPowerOf2(uint32_t n);
static void* initMem(int size);
static int setPrintFunc(stQuickMsgRingH* pThis, void* pCbPrintPara, P_QUICKMSGRING_OUTPUT_PRINTFUNC pfnPrint);
#ifdef __cplusplus
}
#endif
#endif