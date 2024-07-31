/*
 * PICOCOM PROPRIETARY INFORMATION
 *
 * This software is supplied under the terms of a license agreement or
 * nondisclosure agreement with PICOCOM and may not be copied
 * or disclosed except in accordance with the terms of that agreement.
 *
 * Copyright PICOCOM.
 */

#ifndef __QUICKMSGRING_PU_H__
#define __QUICKMSGRING_PU_H__
#include <stdint.h>
#ifdef __cplusplus
extern "C" {
#endif /*__cplusplus*/

typedef void (*P_QUICKMSGRING_OUTPUT_PRINTFUNC)(const char* fileName, int line, const char* func, void* pCbPara, int level, char* format, ...);
typedef struct stQuickMsgRingH {
    void* pQuickMsgRing;
    int (*pfnQuickMsgRingSendMsg)(struct stQuickMsgRingH* pThis, char* pMsg, uint32_t msgLen);
    void (*pfnQuickMsgRingRecvMsgComplete)(struct stQuickMsgRingH* pThis);
    int (*pfnQuickMsgRingGetBuf)(struct stQuickMsgRingH* pThis, uint32_t** msg, uint32_t wordCount);
    int (*pfnQuickMsgRingBeginRecvMsg)(struct stQuickMsgRingH* pThis, uint32_t** msg, uint32_t* msgLen);
    int (*pfnQuickMsgRingSendMsgWithPackFunc)(struct stQuickMsgRingH* pThis, uint32_t msgWordSz, int (*pFunc)(void* pArg, void* pMsg), void* pArg);
    int (*pfnQuickMsgRingRecvMsgWithUnpackFunc)(struct stQuickMsgRingH* pThis, int (*pFunc)(void* pArg, void* pMsg, uint32_t msgLen), void* pArg);
    int (*pfnQuickMsgRingGetBufBulk)(struct stQuickMsgRingH* pThis, uint32_t** msg, uint32_t* wordCount, int msgCount);
    int (*pfnQuickMsgRingBeginRecvMsgBulk)(struct stQuickMsgRingH* pThis, uint32_t** msg, uint32_t* msgLen, int maxMsgCount);
    int (*pfnSetPrintFunc)(struct stQuickMsgRingH* pThis, void* pCbPrintPara, P_QUICKMSGRING_OUTPUT_PRINTFUNC pfnPrint);
} stQuickMsgRingH;

/*init para info*/
typedef struct {
    void* pBuf;
    uint32_t size;
} stQuickMsgRing_para;

int init_quickMsgRing(stQuickMsgRingH* pThis, stQuickMsgRing_para* pInitPara);
int uninit_quickMsgRing(stQuickMsgRingH* pThis);
#ifdef __cplusplus
}
#endif
#endif