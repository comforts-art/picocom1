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
#include <memory.h>
#include <stdint.h>
#include <stdio.h>
#include <stdlib.h>

#include "quickMsgRing_pr.h"

/**
 *@author:
 *@brief:
 *@note:
 *@version:
 *@date:
 *@param:
 *  pThis --
 *  pCbPrintPara --
 *  pfnPrint --
 *@return:
**/
static int setPrintFunc(stQuickMsgRingH* pThis, void* pCbPrintPara, P_QUICKMSGRING_OUTPUT_PRINTFUNC pfnPrint)
{
    stQuickMsgRing* tmpQuickMsgRing = (stQuickMsgRing*)pThis->pQuickMsgRing;
    tmpQuickMsgRing->pCbPrintPara = pCbPrintPara;
    tmpQuickMsgRing->pfnPrint = pfnPrint;
    return SUCCESS;
}

/**
 *@author:
 *@brief:
 *@note:
 *@version:
 *@date:
 *@param:
 *  pThis --
 *  pMsg --
 *  msgLen --
 *@return:
**/
static int QuickMsgRingSendMsg(stQuickMsgRingH* pThis, char* pMsg, uint32_t msgLen)
{
    stQuickMsgRing* tmpQuickMsgRing = (stQuickMsgRing*)pThis->pQuickMsgRing;
    if (pMsg && (pMsg < (char*)tmpQuickMsgRing->buf || pMsg > (char*)(tmpQuickMsgRing->buf + tmpQuickMsgRing->size))) { // not get buf  before send, so we must copy msg
        char* pBuf = NULL;
        QuickMsgRingGetBuf(pThis, (uint32_t**)&pBuf, msgLen);
        if (!pBuf)
            return FAILURE;
        MEMMEMCPY(pBuf, pMsg, msgLen);
    }
    tmpQuickMsgRing->in = tmpQuickMsgRing->nextIn;

    return SUCCESS;
}

/**
 *@author:
 *@brief:
 *@note:
 *@version:
 *@date:
 *@param:
 *  pThis --
 *@return:
**/
static void QuickMsgRingRecvMsgComplete(stQuickMsgRingH* pThis)
{
    stQuickMsgRing* tmpQuickMsgRing = (stQuickMsgRing*)pThis->pQuickMsgRing;
    /*
        * Ensure that we get msge from the ring before 
        * we update the r->out index; 
        */
    tmpQuickMsgRing->out = tmpQuickMsgRing->nextOut;
    tmpQuickMsgRing->emptyFlag = (tmpQuickMsgRing->out == tmpQuickMsgRing->in);

    return;
}

/**
 *@author:
 *@brief:
 *@note:
 *@version:
 *@date:
 *@param:
 *  pThis --
 *  msg --
 *  wordCount --
 *@return:
**/
static int QuickMsgRingGetBuf(stQuickMsgRingH* pThis, uint32_t** msg, uint32_t wordCount)
{
    return QuickMsgRingGetBufBulk(pThis, msg, &wordCount, 1);
}

/**
 *@author:
 *@brief:
 *@note:
 *@version:
 *@date:
 *@param:
 *  pThis --
 *  msg --
 *  msgLen --
 *@return:
**/
static int QuickMsgRingBeginRecvMsg(stQuickMsgRingH* pThis, uint32_t** msg, uint32_t* msgLen)
{
    return QuickMsgRingBeginRecvMsgBulk(pThis, msg, msgLen, 1);
}

/**
 *@author:
 *@brief:
 *@note:
 *@version:
 *@date:
 *@param:
 *  pThis --
 *  msgWordSz --
 *  (*pFunc)(void* pArg, void* pMsg) --
 *  pArg --
 *@return:
**/
static int QuickMsgRingSendMsgWithPackFunc(stQuickMsgRingH* pThis, uint32_t msgWordSz, int (*pFunc)(void* pArg, void* pMsg), void* pArg)
{
    uint32_t* pmsg;
    int ret;
    ret = QuickMsgRingGetBuf(pThis, &pmsg, msgWordSz);
    if (1 != ret)
        return -1;
    ret = pFunc(pArg, (void*)pmsg);
    if (0 != ret)
        return ret;
    QuickMsgRingSendMsg(pThis, (char*)pmsg, msgWordSz);
    return 0;
}

/**
 *@author:
 *@brief:
 *@note:
 *@version:
 *@date:
 *@param:
 *  pThis --
 *  (*pFunc)(void* pArg, void* pMsg, uint32_t msgLen) --
 *  pArg --
 *@return:
**/
static int QuickMsgRingRecvMsgWithUnpackFunc(stQuickMsgRingH* pThis, int (*pFunc)(void* pArg, void* pMsg, uint32_t msgLen), void* pArg)
{
    uint32_t* pmsg = NULL;
    int ret;
    uint32_t msgLen = 0;
    ret = QuickMsgRingBeginRecvMsg(pThis, &pmsg, &msgLen);
    if (1 != ret)
        return ret;
    ret = pFunc(pArg, (void*)pmsg, msgLen);
    if (0 != ret)
        return ret;
    QuickMsgRingRecvMsgComplete(pThis);
    return 0;
}

/**
 *@author:
 *@brief:
 *@note: we need avoiding judging full and empty at the same time to set memory and thus cause memory access.
    memset operation maybe cause SIG_ILL problem, it is because one cpu core memset the memory and another 
    cpu core writing data into the same memory at the same time and causes memory access confliction and SIG_ILL problem. 
    recving party only reads memory and sending party modifies the memories
 *@version:
 *@date:
 *@param:
 *  pThis --
 *  msg --
 *  wordCount --
 *  msgCount --
 *@return:
**/
static int QuickMsgRingGetBufBulk(stQuickMsgRingH* pThis, uint32_t** msg, uint32_t* wordCount, int msgCount)
{
    stQuickMsgRing* tmpQuickMsgRing = (stQuickMsgRing*)pThis->pQuickMsgRing;
    uint32_t topUnused = 0;
    uint32_t unused = 0;
    uint32_t tmpIn = 0;
    uint32_t tmpOut = 0;
    uint32_t tmpWordCnt = 0;
    int n = 0;
    ASSERT(msg);
    *msg = NULL;
    tmpOut = tmpQuickMsgRing->out;
    tmpIn = tmpQuickMsgRing->in;
    while (likely(n < msgCount)) {
        unused = (tmpIn > tmpOut) * (tmpQuickMsgRing->size - tmpIn + tmpOut) + (tmpIn < tmpOut) * (tmpOut - tmpIn) + (tmpIn == tmpOut) * (tmpQuickMsgRing->emptyFlag == 1) * tmpQuickMsgRing->size;
        tmpWordCnt = ((*wordCount % sizeof(uint32_t)) > 0) * (*wordCount / sizeof(uint32_t) + 1) + ((*wordCount % sizeof(uint32_t)) == 0) * (*wordCount / sizeof(uint32_t));
        /*
              * Enusre that  we sample the r->out index before we
              * start allocating msg buffer.
              */
        if (unlikely(unused <= tmpWordCnt + RESERV_LEN)) { //not enough space
            break;
        }
        if (tmpIn >= tmpOut) {
            topUnused = (tmpQuickMsgRing->size - tmpIn);
            if (likely(topUnused >= tmpWordCnt + RESERV_LEN)) {
                tmpQuickMsgRing->buf[tmpIn] = tmpWordCnt;
                *msg = tmpQuickMsgRing->buf + tmpIn + RESERV_LEN;
                MEMMEMSET(*msg, 0, tmpWordCnt * sizeof(uint32_t));
                tmpIn += tmpWordCnt + RESERV_LEN;
                tmpIn = (tmpIn % tmpQuickMsgRing->size);
                n++;
                msg++;
                wordCount++;
            } else if (unused >= ((tmpQuickMsgRing->size - tmpIn) + tmpWordCnt + 3 * RESERV_LEN)) {
                tmpQuickMsgRing->buf[tmpIn] = tmpQuickMsgRing->size; //holderplace
                tmpQuickMsgRing->buf[0] = tmpWordCnt;
                *msg = tmpQuickMsgRing->buf + RESERV_LEN;
                MEMMEMSET(*msg, 0, tmpWordCnt * sizeof(uint32_t));
                tmpIn = RESERV_LEN + tmpWordCnt; //rewind
                tmpIn = (tmpIn % tmpQuickMsgRing->size);
                n++;
                msg++;
                wordCount++;
            } else {
                break;
            }
        } else if ((tmpIn < tmpOut) && (likely(unused > tmpWordCnt + RESERV_LEN))) {
            tmpQuickMsgRing->buf[tmpIn] = tmpWordCnt;
            *msg = tmpQuickMsgRing->buf + tmpIn + RESERV_LEN;
            MEMMEMSET(*msg, 0, tmpWordCnt * sizeof(uint32_t));
            tmpIn += tmpWordCnt + RESERV_LEN;
            tmpIn = (tmpIn == tmpQuickMsgRing->size) * 0 + (tmpIn != tmpQuickMsgRing->size) * tmpIn;
            n++;
            msg++;
            wordCount++;
        } else {
            break;
        }
    }
    tmpQuickMsgRing->nextIn = tmpIn;
    return n;
}

/**
 *@author:
 *@brief:
 *@note:
 *@version:
 *@date:
 *@param:
 *  pThis --
 *  msg --
 *  msgLen --
 *  maxMsgCount --
 *@return:
**/
static int QuickMsgRingBeginRecvMsgBulk(stQuickMsgRingH* pThis, uint32_t** msg, uint32_t* msgLen, int maxMsgCount)
{
    stQuickMsgRing* tmpQuickMsgRing = (stQuickMsgRing*)pThis->pQuickMsgRing;
    uint32_t* tmpMsg = NULL;
    uint32_t tmpOut = 0;
    int n = 0;
    tmpOut = tmpQuickMsgRing->out;
    while (likely(n < maxMsgCount)) {
        if (tmpOut == tmpQuickMsgRing->in && tmpQuickMsgRing->emptyFlag == 1) {
            //empty
            break;
        }
        /*
              * Ensure that we sample the r->in index before
              * we start getting new msg. 
              */
        if (tmpQuickMsgRing->buf[tmpOut] == tmpQuickMsgRing->size) //buf tail holderspace not used
        {
            tmpOut = 0;
        }
        tmpMsg = (tmpQuickMsgRing->buf + tmpOut) + RESERV_LEN;
        *msgLen = tmpQuickMsgRing->buf[tmpOut];
        tmpOut += tmpQuickMsgRing->buf[tmpOut] + RESERV_LEN;
        tmpOut = tmpOut % tmpQuickMsgRing->size;
        *msg = (uint32_t*)tmpMsg;
        msg++;
        msgLen++;
        n++;
    }
    tmpQuickMsgRing->nextOut = tmpOut;
    return n;
}

/**
 *@author:
 *@brief:
 *@note:
 *@version:
 *@date:
 *@param:
 *  n --
 *@return:
**/
static inline int isPowerOf2(uint32_t n)
{
    return n && !(n & (n - 1));
}

/**
 *@author:
 *@brief:
 *@note:
 *@version:
 *@date:
 *@param:
 *  size --
 *@return:
**/
static void* initMem(int size)
{
    void* ptr;
    ptr = MEMMALLOC((size / 64 + 1) * 64);
    if (!ptr)
        printf("fail to alloc memory\n");
    MEMMEMSET(ptr, 0, size);
    return ptr;
}

/**
 *@author:
 *@brief:
 *@note:
 *@version:
 *@date:
 *@param:
 *  pThis --
 *  pInitPara --
 *@return:
**/
int init_quickMsgRing(stQuickMsgRingH* pThis, stQuickMsgRing_para* pInitPara)
{
    stQuickMsgRing* tmpQuickMsgRing = (stQuickMsgRing*)MEMMALLOC(sizeof(stQuickMsgRing));
    if (pThis) {
        MEMMEMSET(pThis, 0, sizeof(stQuickMsgRingH));
        pThis->pQuickMsgRing = tmpQuickMsgRing;
    }
    MEMMEMSET(tmpQuickMsgRing, 0, sizeof(stQuickMsgRing));
    ASSERT(pInitPara->pBuf);
    ASSERT((pInitPara->size >> 29) == 0);
    if (pInitPara->size < 8)
        return FAILURE;
    tmpQuickMsgRing->size = pInitPara->size / sizeof(uint32_t);
    tmpQuickMsgRing->buf = pInitPara->pBuf ? pInitPara->pBuf : initMem(tmpQuickMsgRing->size * sizeof(uint32_t));
    tmpQuickMsgRing->flag = (!!(pInitPara->pBuf)) * 1;
    tmpQuickMsgRing->emptyFlag = 1;
    memset(tmpQuickMsgRing->buf, 0, tmpQuickMsgRing->size * sizeof(uint32_t));
    pThis->pfnQuickMsgRingSendMsg = QuickMsgRingSendMsg;
    pThis->pfnQuickMsgRingRecvMsgComplete = QuickMsgRingRecvMsgComplete;
    pThis->pfnQuickMsgRingGetBuf = QuickMsgRingGetBuf;
    pThis->pfnQuickMsgRingBeginRecvMsg = QuickMsgRingBeginRecvMsg;
    pThis->pfnQuickMsgRingSendMsgWithPackFunc = QuickMsgRingSendMsgWithPackFunc;
    pThis->pfnQuickMsgRingRecvMsgWithUnpackFunc = QuickMsgRingRecvMsgWithUnpackFunc;
    pThis->pfnQuickMsgRingGetBufBulk = QuickMsgRingGetBufBulk;
    pThis->pfnQuickMsgRingBeginRecvMsgBulk = QuickMsgRingBeginRecvMsgBulk;
    if (pThis)
        pThis->pfnSetPrintFunc = setPrintFunc;
    return SUCCESS;
}

/**
 *@author:
 *@brief:
 *@note:
 *@version:
 *@date:
 *@param:
 *  pThis --
 *@return:
**/
int uninit_quickMsgRing(stQuickMsgRingH* pThis)
{
    stQuickMsgRing* tmpQuickMsgRing = (stQuickMsgRing*)pThis->pQuickMsgRing;
    if (!tmpQuickMsgRing->flag)
        MEMFREE(tmpQuickMsgRing->buf);
    MEMFREE(pThis->pQuickMsgRing);
    MEMMEMSET(pThis, 0, sizeof(*pThis));
    return SUCCESS;
}
