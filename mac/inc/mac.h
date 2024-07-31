/*
 * PICOCOM PROPRIETARY INFORMATION
 *
 * This software is supplied under the terms of a license agreement or
 * nondisclosure agreement with PICOCOM and may not be copied
 * or disclosed except in accordance with the terms of that agreement.
 *
 * Copyright PICOCOM.
 */

#ifndef _MAC_H__
#define _MAC_H__

#include "l1_api.h"
#include "quickMsgRing_pu.h"
#include "testmac_cfg.h"
#include "ttiCmm.h"


#define MAX_PRE_SCHEDULE_SFN_WINDOW 4
#define MAX_PRE_SCHEDULE_SFN_WINDOW_MASK (MAX_PRE_SCHEDULE_SFN_WINDOW - 1)
#define MAC_MAX_CELL_NUM 2

#ifdef MULTI_PC802
   #define MAC_MULTI_PC802
#endif

#ifdef MAC_MULTI_PC802
#define MAC_PC802_INDEX_MAX 4
#else
#define MAC_PC802_INDEX_MAX 1
#endif

typedef enum {
    PHY_IDLE,
    PHY_CFG,
    PHY_INIT,
    PHY_ACTIVE
} MacStatus_e;

typedef enum {
    SCH_STOP,
    SCH_NORMAL,
    SCH_DEFAULT
} MacSchStatus_e;

typedef enum {
    L1_PARAM_REQUEST,
    L1_PARAM_RESPONSE,
    L1_CONFIG_REQUEST,
    L1_CONFIG_RESPONSE,
    L1_START_REQUEST,
    L1_STOP_REQUEST,
    L1_STOP_INDICATION,
    L1_ERROR_INDICATION,
    L1_DL_TTI_REQUEST,
    L1_UL_TTI_REQUEST,
    L1_SLOT_INDICATION,
    L1_UL_DCI_REQUEST,
    L1_TX_DATA_REQUEST,
    L1_RX_DATA_INDICATION,
    L1_CRC_INDICATION,
    L1_UCI_INDICATION,
    L1_SRS_INDICATION,
    L1_PRACH_INDICATION
} FAPIMsgId_e;

typedef struct {
    uint8_t dlttiTxEnable;
    uint8_t ulttiTxEnable;
    uint8_t txdataTxEnble;
    uint8_t csiTxEnable;
} MacTxEnable_t;

typedef struct {
    uint8_t msgNum;
    uint8_t opaque;
    uint16_t rev;
} FAPI_MSG_HDR;

typedef struct {
    TestMacRunMode_e runMode;
    // MacStatus_e state[MAC_MAX_CELL_NUM];
    // MacSchStatus_e schState[MAC_MAX_CELL_NUM];
    MacStatus_e state;
    MacSchStatus_e schState;
    TestMacOperMode_e operMode;
    TestMacPdschMode_e pdschMode;
    CmmBool_t phyStub;
    uint32_t waitRspTmr;
    stQuickMsgRingH msgqH; // rev message from CLI
    stQuickMsgRingH msgqApp; // rev message from main core
    uint32_t runttiPerTc;
    CmmBool_t sfnMode;
} MacContent_s;

extern uint64_t maxTimeOut;
extern uint64_t minTimeOut;

extern uint8_t gCellNum;
extern L1SlotIndication_t gL2RxGSfn[MAC_PC802_INDEX_MAX][MAC_MAX_CELL_NUM];
extern L1SlotIndication_t gL2TxGSfn[MAC_PC802_INDEX_MAX][MAC_MAX_CELL_NUM];

extern L1ConfigRequest_t* gL1ConfigRequest[MAC_PC802_INDEX_MAX][MAC_MAX_CELL_NUM];
extern L1DLTTIReq_t* gL1DLTTIReq[MAC_PC802_INDEX_MAX][MAC_MAX_CELL_NUM][MAX_PRE_SCHEDULE_SFN_WINDOW][20];
extern L1TXDATAREQ_t* gL1TXDATAREQ[MAC_PC802_INDEX_MAX][MAC_MAX_CELL_NUM][MAX_PRE_SCHEDULE_SFN_WINDOW][20];
extern L1ULTTI_t* gL1ULTTI[MAC_PC802_INDEX_MAX][MAC_MAX_CELL_NUM][MAX_PRE_SCHEDULE_SFN_WINDOW][20];

extern uint32_t gL1ConfigReqSize[MAC_PC802_INDEX_MAX][MAC_MAX_CELL_NUM];
extern uint32_t gL1DlttiReqSize[MAC_PC802_INDEX_MAX][MAC_MAX_CELL_NUM][MAX_PRE_SCHEDULE_SFN_WINDOW][20];
extern uint32_t gL1UlttiReqSize[MAC_PC802_INDEX_MAX][MAC_MAX_CELL_NUM][MAX_PRE_SCHEDULE_SFN_WINDOW][20];
extern uint32_t gL1TxdataReqSize[MAC_PC802_INDEX_MAX][MAC_MAX_CELL_NUM][MAX_PRE_SCHEDULE_SFN_WINDOW][20];

extern MacContent_s gMacCtx[MAC_PC802_INDEX_MAX][MAC_MAX_CELL_NUM];

// extern uint32_t gcurrentScs[2];

void loadTxCfg(int txCfg);
void getTxCfg(MacTxEnable_t* macTxEnable);

void macInit(TestMacRunMode_e runMode, TestMacOperMode_e operMode, int* cpuId, CmmBool_t phyStub, TestMacPdschMode_e pdschMode, uint32_t runTtiPerTc, int8_t atllloglevel, CmmBool_t sfnMode);
void macStop(uint8_t dev_index, uint8_t cellId);

uint32_t ConfigReqParams(L1ConfigRequest_t* configReq, uint8_t* OutputStart);

uint32_t DlttiParams(L1DLTTIReq_t* dltti, uint8_t* OutputStart, uint8_t dev_index, uint8_t cellId);
uint32_t UlttiParams(L1ULTTI_t* ultti, uint8_t* OutputStart, uint8_t dev_index, uint8_t cellId);
uint32_t TxdataParams(L1TXDATAREQ_t* txdata, uint8_t* OutputStart, uint8_t dev_index, uint8_t cellId);

int macTcDes();

void setPhyState(uint8_t dev_index, uint8_t cellId, MacStatus_e state);
MacStatus_e getPhyState(uint8_t dev_index, uint8_t cellId);

void macAppMgrSend(uint8_t dev_index, uint8_t cellId, char* pBuf, uint32_t bufLen);

void macRevCtrlMsg();

void macRevAppCtrlMsg(uint8_t dev_index, uint8_t cellId);

void macTxStartIndHandle(L1SlotIndication_t* msg, uint8_t dev_index, uint8_t cellId);
void macRevCfgResonse(L1ConfigResponse_t* rsp, uint8_t dev_index, uint8_t cellId);
void macRevStopIndication(uint8_t dev_index, uint8_t cellId);
void macRevErrorIndication(L1ErrorIndication_t* errorInd, uint8_t dev_index, uint8_t cellId);
void macRxDataPduHandle(L1SlotIndication_t* revSlot, L1RxDataPduInfo_t* rxDataPdu);
void macDlSchBuildPayload(uint32_t size, uint32_t* offset, uint8_t dev_index, uint8_t cellId);
#ifdef MAC_MULTI_PC802
uint32_t macRevL1Ctrl(const char* buf, uint32_t payloadSize, uint8_t dev_index, uint8_t cellId);
#else
uint32_t macRevL1Ctrl(const char* buf, uint32_t payloadSize);
#endif
uint32_t macSendL1Ctrl(const char* buf, uint32_t payloadSize);
uint32_t macSendL1Data(const char* buf, uint32_t payloadSize);

#endif
