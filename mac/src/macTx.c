/*
 * PICOCOM PROPRIETARY INFORMATION
 *
 * This software is supplied under the terms of a license agreement or
 * nondisclosure agreement with PICOCOM and may not be copied
 * or disclosed except in accordance with the terms of that agreement.
 *
 * Copyright PICOCOM.
 */

#include "macTx.h"
#include "mac.h"

#include "cmm_zlog.h"
#include "macTrace.h"
#include "pcxx_ipc.h"

#define UINT32_ALIGN(x, a) (((x) + ((a)-1)) & ~((a)-1))
#define MSG_HDR_SIZE 0x8

typedef struct
{
    FAPI_MSG_HDR* generalMsgHeader;
    char* curMsgHeader; /*for pcxxx api*/
    uint32_t curMsgLen; /*32bit alignment  for pcxxx api*/
    uint32_t maxRemLen; /*max remain length in phy channel*/
    uint32_t totalLen;
} PhyApiTxCtxt_s;

static PhyApiTxCtxt_s phyCtrlMsgCtxt[MAC_PC802_INDEX_MAX][MAC_MAX_CELL_NUM] = { 0 };

void* allocL1CtrlMsgMem(uint32_t size, uint8_t dev_index, uint8_t cellId)
{
    PhyApiTxCtxt_s* pTxCtx = &phyCtrlMsgCtxt[dev_index][cellId];

    pTxCtx->curMsgHeader = NULL;
    pTxCtx->curMsgLen = 0;

    /*allocate pcxxx api memory */
#ifndef MAC_MULTI_PC802
    pcxxCtrlAlloc(&pTxCtx->curMsgHeader, &pTxCtx->maxRemLen);
#else
    pcxxCtrlAlloc(&pTxCtx->curMsgHeader, &pTxCtx->maxRemLen, dev_index, cellId);
#endif

    ASSERT(pTxCtx->curMsgHeader);
    ASSERT(pTxCtx->maxRemLen >= size);

    return pTxCtx->curMsgHeader;
}

/*fill msg group header for messages*/
void setL1CtrlMsgGrpHeader(uint8_t dev_index, uint8_t cellId)
{
    PhyApiTxCtxt_s* pTxCtx = &phyCtrlMsgCtxt[dev_index][cellId];

    // memset(&phyCtrlMsgCtxt, 0, sizeof(phyCtrlMsgCtxt));
    // memset(&phyCtrlMsgCtxt[dev_index][cellId], 0, sizeof(phyCtrlMsgCtxt[dev_index][cellId]));
    memset(pTxCtx, 0, sizeof(*pTxCtx));
    pTxCtx->generalMsgHeader = NULL;

    /* pcxxx allocate memory api*/
#ifndef MAC_MULTI_PC802
    pcxxCtrlAlloc((char**)&pTxCtx->generalMsgHeader, &pTxCtx->maxRemLen);
#else
    pcxxCtrlAlloc((char**)&pTxCtx->generalMsgHeader, &pTxCtx->maxRemLen, dev_index, cellId);
#endif

    ASSERT(pTxCtx->generalMsgHeader);
    ASSERT(pTxCtx->maxRemLen > 32);

    pTxCtx->generalMsgHeader->msgNum = 0;
    pTxCtx->generalMsgHeader->opaque = cellId;

    pTxCtx->curMsgHeader = (char*)pTxCtx->generalMsgHeader;
    pTxCtx->curMsgLen = sizeof(*pTxCtx->generalMsgHeader);
    pTxCtx->totalLen = pTxCtx->curMsgLen;
#ifndef MAC_MULTI_PC802
    pcxxCtrlSend((char*)pTxCtx->generalMsgHeader, sizeof(*pTxCtx->generalMsgHeader));
#else
    pcxxCtrlSend((char*)pTxCtx->generalMsgHeader, sizeof(*pTxCtx->generalMsgHeader), dev_index, cellId);
#endif

    return;
}

/*fill msg group header for messages*/
void traceL1CtrlMsg(uint8_t dev_index, uint8_t cellId)
{
    addPhyTraceItem(phyCtrlMsgCtxt[dev_index][cellId].generalMsgHeader, phyCtrlMsgCtxt[dev_index][cellId].totalLen);

    return;
}
void appendL1MsgToList(void* data, uint32_t len, uint16_t sfn, uint8_t slot, uint8_t msgType, uint8_t dev_index, uint8_t cellId)
{
    ASSERT(data);
    ASSERT(sfn < 1024);
    ASSERT(slot < 160);
    ASSERT(msgType < 254);
    PhyApiTxCtxt_s* pTxCtx = &phyCtrlMsgCtxt[dev_index][cellId];

    /*update pcxxx information*/
    pTxCtx->curMsgLen += len;
    pTxCtx->totalLen += len;
    ASSERT(pTxCtx->curMsgLen < pTxCtx->maxRemLen);
#ifndef MAC_MULTI_PC802
    if (pcxxCtrlSend(pTxCtx->curMsgHeader, pTxCtx->curMsgLen) != 0) {
        printf("pcxx Ctrl send fail\n");
    }
#else
    if (pcxxCtrlSend(pTxCtx->curMsgHeader, pTxCtx->curMsgLen, dev_index, cellId) != 0) {
        printf("dev:%d cell:%d, pcxx Ctrl send fail\n",dev_index,cellId);
    }
#endif

    pTxCtx->generalMsgHeader->msgNum++;
    // addPhyTraceItem(data, len);
    return;
}

void sendPhyConfigReq(uint8_t dev_index, uint8_t cellId)
{
    START_L1_MSG_PROC(dev_index, cellId);
    uint32_t msgSize = 0;
    uint32_t* l1MessageOutput = (uint32_t*)allocL1CtrlMsgMem(sizeof(L1GeneralMsgHdr_t), dev_index, cellId);
    L1GeneralMsgHdr_t* pHdr;
    pHdr = (L1GeneralMsgHdr_t*)l1MessageOutput;

    l1MessageOutput += sizeof(L1GeneralMsgHdr_t) >> 2;
    if (gMacCtx[dev_index][cellId].runMode == RUNMODE_FAPI_GENERATOR) {
        memcpy(l1MessageOutput, (uint8_t*)gL1ConfigRequest[dev_index][cellId] + 8, gL1ConfigReqSize[dev_index][cellId]);
        msgSize = gL1ConfigReqSize[dev_index][cellId] - 8;
    } else
        msgSize = ConfigReqParams(&gL1ConfigRequest[dev_index][cellId], (uint8_t*)l1MessageOutput);
    // memcpy(l1MessageOutput, gL1ConfigRequest, msgSize);
    //getCarrierConfig((L1ConfigRequest_t*)l1MessageOutput, dev_index, cellId);
    pHdr->msgId = FAPI_CONFIG_REQUEST;
    pHdr->msgSize = msgSize;

    msgSize += sizeof(L1GeneralMsgHdr_t);
    appendL1MsgToList(pHdr, msgSize, 0, 0, L1_CONFIG_REQUEST, dev_index, cellId);
    CMM_DEBUG(LOG_MAC, "dev:%d cell:%d,PHY CONFIG REQ",dev_index,cellId);
    END_L1_MSG_PROC(dev_index, cellId);
}

void sendPhyParamReq(uint8_t dev_index, uint8_t cellId)
{
    START_L1_MSG_PROC(dev_index, cellId);
    uint32_t msgSize = 0;
    uint32_t* l1MessageOutput = (uint32_t*)allocL1CtrlMsgMem(sizeof(L1GeneralMsgHdr_t), dev_index, cellId);
    L1GeneralMsgHdr_t* pHdr;
    pHdr = (L1GeneralMsgHdr_t*)l1MessageOutput;

    pHdr->msgId = FAPI_PARAM_REQUEST;
    pHdr->msgSize = msgSize;

    msgSize += sizeof(L1GeneralMsgHdr_t);
    appendL1MsgToList(pHdr, msgSize, 0, 0, FAPI_PARAM_REQUEST, dev_index, cellId);
    CMM_DEBUG(LOG_MAC, "dev:%d cell:%d, PHY PARAM REQ",dev_index,cellId);
    END_L1_MSG_PROC(dev_index, cellId);
}

void sendPhyStartReq(uint8_t dev_index, uint8_t cellId)
{
    START_L1_MSG_PROC(dev_index, cellId);
    uint32_t msgSize = 0;
    uint32_t* l1MessageOutput = (uint32_t*)allocL1CtrlMsgMem(sizeof(L1GeneralMsgHdr_t), dev_index, cellId);
    L1GeneralMsgHdr_t* pHdr;
    pHdr = (L1GeneralMsgHdr_t*)l1MessageOutput;

    pHdr->msgId = FAPI_START_REQUEST;
    pHdr->msgSize = msgSize;

    msgSize += sizeof(L1GeneralMsgHdr_t);
    appendL1MsgToList(pHdr, msgSize, 0, 0, L1_START_REQUEST, dev_index, cellId);
    CMM_DEBUG(LOG_MAC, "dev:%d cell:%d, PHY START REQ",dev_index,cellId);
    END_L1_MSG_PROC(dev_index, cellId);
}

void sendPhyStopReq(uint8_t dev_index, uint8_t cellId)
{
    START_L1_MSG_PROC(dev_index, cellId);
    uint32_t msgSize = 0;
    uint32_t* l1MessageOutput = (uint32_t*)allocL1CtrlMsgMem(sizeof(L1GeneralMsgHdr_t), dev_index, cellId);
    L1GeneralMsgHdr_t* pHdr;
    pHdr = (L1GeneralMsgHdr_t*)l1MessageOutput;

    pHdr->msgId = FAPI_STOP_REQUEST;
    pHdr->msgSize = msgSize;

    msgSize += sizeof(L1GeneralMsgHdr_t);
    appendL1MsgToList(pHdr, msgSize, 0, 0, L1_STOP_REQUEST, dev_index, cellId);
    CMM_DEBUG(LOG_MAC, "dev:%d cell:%d,PHY STOP REQ",dev_index,cellId);
    END_L1_MSG_PROC(dev_index, cellId);
}

void sendDlTTIReq(uint16_t sfn, uint8_t slot, uint8_t dev_index, uint8_t cellId)
{
    uint32_t msgSize = 0;
    uint32_t* l1MessageOutput = (uint32_t*)allocL1CtrlMsgMem(sizeof(L1GeneralMsgHdr_t), dev_index, cellId);
    L1GeneralMsgHdr_t* pHdr;
    pHdr = (L1GeneralMsgHdr_t*)l1MessageOutput;
    l1MessageOutput += sizeof(L1GeneralMsgHdr_t) >> 2;
    L1DLTTIReq_t* dlttiReq = (L1DLTTIReq_t*)((uint8_t*)gL1DLTTIReq[dev_index][cellId][(sfn & MAX_PRE_SCHEDULE_SFN_WINDOW_MASK)][slot] + 8);

    if (gMacCtx[dev_index][cellId].runMode == RUNMODE_FAPI_GENERATOR) {
        dlttiReq->SFN = sfn;
        dlttiReq->Slot = slot;
        // msgSize = DlttiParams(dlttiReq, (uint8_t*)l1MessageOutput);
        memcpy(l1MessageOutput, (uint8_t*)dlttiReq, gL1DlttiReqSize[dev_index][cellId][(sfn & MAX_PRE_SCHEDULE_SFN_WINDOW_MASK)][slot]);
        msgSize = gL1DlttiReqSize[dev_index][cellId][(sfn & MAX_PRE_SCHEDULE_SFN_WINDOW_MASK)][slot] - 8;
    } else
        msgSize = DlttiParams(gL1DLTTIReq[dev_index][cellId][(sfn & MAX_PRE_SCHEDULE_SFN_WINDOW_MASK)][slot], (uint8_t*)l1MessageOutput, dev_index, cellId);

    // memcpy(l1MessageOutput, gL1DLTTIReq[(sfn & MAX_PRE_SCHEDULE_SFN_WINDOW_MASK)][slot], msgSize);

    pHdr->msgId = FAPI_DL_TTI_REQUEST;
    pHdr->msgSize = msgSize;

    msgSize += sizeof(L1GeneralMsgHdr_t);

    appendL1MsgToList(pHdr, msgSize, sfn, slot, L1_DL_TTI_REQUEST, dev_index, cellId);
    CMM_DEBUG(LOG_MAC, "dev:%d cell:%d,DL TTI REQ SFN[%d] SLOT[%d]",dev_index,cellId, sfn, slot);
}

void sendUlTTIReq(uint16_t sfn, uint8_t slot, uint8_t dev_index, uint8_t cellId)
{
    uint32_t msgSize = 0;
    uint32_t* l1MessageOutput = (uint32_t*)allocL1CtrlMsgMem(sizeof(L1GeneralMsgHdr_t), dev_index, cellId);
    L1GeneralMsgHdr_t* pHdr;
    pHdr = (L1GeneralMsgHdr_t*)l1MessageOutput;
    l1MessageOutput += sizeof(L1GeneralMsgHdr_t) >> 2;
    L1ULTTI_t* ulttiReq = (L1ULTTI_t*)((uint8_t*)gL1ULTTI[dev_index][cellId][(sfn & MAX_PRE_SCHEDULE_SFN_WINDOW_MASK)][slot] + 8);
    if (gMacCtx[dev_index][cellId].runMode == RUNMODE_FAPI_GENERATOR) {
        ulttiReq->SFN = sfn;
        ulttiReq->Slot = slot;
        // msgSize = UlttiParams(ulttiReq, (uint8_t*)l1MessageOutput);
        memcpy(l1MessageOutput, (uint8_t*)ulttiReq, gL1UlttiReqSize[dev_index][cellId][(sfn & MAX_PRE_SCHEDULE_SFN_WINDOW_MASK)][slot]);
        msgSize = gL1UlttiReqSize[dev_index][cellId][(sfn & MAX_PRE_SCHEDULE_SFN_WINDOW_MASK)][slot] - 8;
    } else
        msgSize = UlttiParams(gL1ULTTI[dev_index][cellId][(sfn & MAX_PRE_SCHEDULE_SFN_WINDOW_MASK)][slot], (uint8_t*)l1MessageOutput, dev_index, cellId);
    // memcpy(l1MessageOutput, gL1ULTTI[(sfn & MAX_PRE_SCHEDULE_SFN_WINDOW_MASK)][slot], msgSize);
    pHdr->msgId = FAPI_UL_TTI_REQUEST;
    pHdr->msgSize = msgSize;

    msgSize += sizeof(L1GeneralMsgHdr_t);

    appendL1MsgToList(pHdr, msgSize, sfn, slot, L1_UL_TTI_REQUEST, dev_index, cellId);
    CMM_DEBUG(LOG_MAC, "dev:%d cell:%d,UL TTI REQ SFN[%d] SLOT[%d]", dev_index,cellId,sfn, slot);
}

void sendTxdataReq(uint16_t sfn, uint8_t slot, uint8_t dev_index, uint8_t cellId)
{
    uint32_t msgSize = 0;
    uint32_t* l1MessageOutput = (uint32_t*)allocL1CtrlMsgMem(sizeof(L1GeneralMsgHdr_t), dev_index, cellId);
    L1GeneralMsgHdr_t* pHdr = (L1GeneralMsgHdr_t*)l1MessageOutput;
    l1MessageOutput += sizeof(L1GeneralMsgHdr_t) >> 2;

    if (gMacCtx[dev_index][cellId].runMode == RUNMODE_FAPI_GENERATOR) {
        msgSize = TxdataParams((L1TXDATAREQ_t*)((uint8_t*)gL1TXDATAREQ[dev_index][cellId][(sfn & MAX_PRE_SCHEDULE_SFN_WINDOW_MASK)][slot] + 8), (uint8_t*)l1MessageOutput, dev_index, cellId);
        // memcpy(l1MessageOutput, (uint8_t*)gL1TXDATAREQ[(sfn & MAX_PRE_SCHEDULE_SFN_WINDOW_MASK)][slot]+8, gL1TxdataReqSize[(sfn & MAX_PRE_SCHEDULE_SFN_WINDOW_MASK)][slot]);
        // msgSize = gL1TxdataReqSize[(sfn & MAX_PRE_SCHEDULE_SFN_WINDOW_MASK)][slot];
    } else
        msgSize = TxdataParams(gL1TXDATAREQ[dev_index][cellId][(sfn & MAX_PRE_SCHEDULE_SFN_WINDOW_MASK)][slot], (uint8_t*)l1MessageOutput, dev_index, cellId);
    // memcpy(l1MessageOutput, gL1TXDATAREQ[(sfn & MAX_PRE_SCHEDULE_SFN_WINDOW_MASK)][slot], msgSize);

    pHdr->msgId = FAPI_TX_DATA_REQUEST;
    pHdr->msgSize = msgSize;

    msgSize += sizeof(L1GeneralMsgHdr_t);

    appendL1MsgToList(pHdr, msgSize, sfn, slot, L1_TX_DATA_REQUEST, dev_index, cellId);
    CMM_DEBUG(LOG_MAC, "dev:%d cell:%d,TX DATA REQ SFN[%d] SLOT[%d]", dev_index,cellId,sfn, slot);
}
void sendSingleDlTTIReq(uint16_t sfn, uint8_t slot, uint8_t dev_index, uint8_t cellId)
{
    START_L1_MSG_PROC(dev_index, cellId);
    sendDlTTIReq(sfn, slot, dev_index, cellId);
    END_L1_MSG_PROC(dev_index, cellId);
}
void sendSingleUlTTIReq(uint16_t sfn, uint8_t slot, uint8_t dev_index, uint8_t cellId)
{
    START_L1_MSG_PROC(dev_index, cellId);
    sendUlTTIReq(sfn, slot, dev_index, cellId);
    END_L1_MSG_PROC(dev_index, cellId);
}
void sendSingleTxDataReq(uint16_t sfn, uint8_t slot, uint8_t dev_index, uint8_t cellId)
{
    START_L1_MSG_PROC(dev_index, cellId);
    sendTxdataReq(sfn, slot, dev_index, cellId);
    END_L1_MSG_PROC(dev_index, cellId);
}

void macTtiTx(uint16_t sfn, uint8_t slot, uint8_t dev_index, uint8_t cellId)
{
    MacTxEnable_t macTxEnable = { 0 };
    getTxCfg(&macTxEnable);

    START_L1_MSG_PROC(dev_index, cellId);
    if (macTxEnable.dlttiTxEnable == 1)
        sendDlTTIReq(sfn, slot, dev_index, cellId);
    if (macTxEnable.ulttiTxEnable == 1)
        sendUlTTIReq(sfn, slot, dev_index, cellId);
    if (macTxEnable.txdataTxEnble == 1)
        sendTxdataReq(sfn, slot, dev_index, cellId);
    END_L1_MSG_PROC(dev_index, cellId);
}
