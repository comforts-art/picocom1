/*
 * PICOCOM PROPRIETARY INFORMATION
 *
 * This software is supplied under the terms of a license agreement or
 * nondisclosure agreement with PICOCOM and may not be copied
 * or disclosed except in accordance with the terms of that agreement.
 *
 * Copyright PICOCOM.
 */

#define _GNU_SOURCE
#include <arpa/inet.h>
#include <assert.h>
#include <errno.h>
#include <fcntl.h>
#include <netinet/in.h>
#include <pthread.h>
#include <sched.h>
#include <stdio.h>
#include <string.h>
#include <sys/socket.h>
#include <sys/uio.h>
#include <unistd.h>

#include "cmm_thread.h"
#include "cmm_zlog.h"
#include "mac.h"
#include "macLoad.h"
#include "macTrace.h"
#include "macTx.h"
#include "mac_rx.h"
#include "mac_utility.h"
#include "moduleInf.h"
#include "pcxx_ipc.h"
#include "quickMsgRing_pu.h"
#include "ttiCmm.h"

#ifdef ATLL_PCIE
#include "rte_pmd_pc802.h"
#endif

extern int g_devMapPort[MAC_PC802_INDEX_MAX];
extern uint8_t g_cellNum[MAC_PC802_INDEX_MAX];

extern uint32_t macGetSlotNumPerSfn(uint16_t dev_index, uint16_t cellid);

#define MAX_CONF_MSG_SIZE (1 << 16)
#define MAX_DL_TTI_MSG_SIZE (1 << 16)
#define MAX_UL_TTI_MSG_SIZE (1 << 12)
#define MAX_TX_DATA_MSG_SIZE (1 << 12)

static uint16_t sTcIdInRunning = 0;

uint32_t gSystemTtiNum[MAC_PC802_INDEX_MAX][MAC_MAX_CELL_NUM] = { 0 };
L1SlotIndication_t gL2RxGSfn[MAC_PC802_INDEX_MAX][MAC_MAX_CELL_NUM] = { 0 };
L1SlotIndication_t gL2TxGSfn[MAC_PC802_INDEX_MAX][MAC_MAX_CELL_NUM] = { 0 };

uint16_t gMacTxRfn[MAC_PC802_INDEX_MAX][MAC_MAX_CELL_NUM] = { 0 }; // +3
uint8_t gMacTxSlot[MAC_PC802_INDEX_MAX][MAC_MAX_CELL_NUM] = { 0 }; // +3

uint16_t gMacRxRfn[MAC_PC802_INDEX_MAX][MAC_MAX_CELL_NUM] = { 0 };
uint8_t gMacRxSlot[MAC_PC802_INDEX_MAX][MAC_MAX_CELL_NUM] = { 0 }; // +1
uint8_t gCellNum = 1;
uint8_t gDlttiTxEnable = 0;
uint8_t gUlttiTxEnable = 0;
uint8_t gTxdataTxEnble = 0;
uint8_t gCsiTxEnable = 0;

L1ConfigRequest_t* gL1ConfigRequest[MAC_PC802_INDEX_MAX][MAC_MAX_CELL_NUM];
L1DLTTIReq_t* gL1DLTTIReq[MAC_PC802_INDEX_MAX][MAC_MAX_CELL_NUM][MAX_PRE_SCHEDULE_SFN_WINDOW][20];
L1TXDATAREQ_t* gL1TXDATAREQ[MAC_PC802_INDEX_MAX][MAC_MAX_CELL_NUM][MAX_PRE_SCHEDULE_SFN_WINDOW][20];
L1ULTTI_t* gL1ULTTI[MAC_PC802_INDEX_MAX][MAC_MAX_CELL_NUM][MAX_PRE_SCHEDULE_SFN_WINDOW][20];

uint32_t gL1ConfigReqSize[MAC_PC802_INDEX_MAX][MAC_MAX_CELL_NUM] = { 0 };
uint32_t gL1DlttiReqSize[MAC_PC802_INDEX_MAX][MAC_MAX_CELL_NUM][MAX_PRE_SCHEDULE_SFN_WINDOW][20];
uint32_t gL1UlttiReqSize[MAC_PC802_INDEX_MAX][MAC_MAX_CELL_NUM][MAX_PRE_SCHEDULE_SFN_WINDOW][20];
uint32_t gL1TxdataReqSize[MAC_PC802_INDEX_MAX][MAC_MAX_CELL_NUM][MAX_PRE_SCHEDULE_SFN_WINDOW][20];
uint64_t maxTimeOut = 0;
uint64_t minTimeOut = 0;

MacContent_s gMacCtx[MAC_PC802_INDEX_MAX][MAC_MAX_CELL_NUM] = { 0 };

#define MAX_DLSCH_PAYLOAD_BYTES_NUM 100

uint8_t BsMacPayloads_pdsch[MAX_DLSCH_PAYLOAD_BYTES_NUM] = {
    0x0f, 0x70, 0xb3, 0x6f, 0x43, 0x98, 0x48, 0xae, 0xbc, 0x97, 0x38, 0x1d,
    0xd3, 0xd4, 0xa0, 0x55, 0x7d, 0x68, 0x37, 0x6d, 0x60, 0xbb, 0xe3, 0xcd,
    0x35, 0xc6, 0x8b, 0xfa, 0x58, 0xa6, 0x30, 0x19, 0x95, 0x93, 0xf6, 0x92,
    0x6f, 0xcb, 0x50, 0xa2, 0x76, 0x5e, 0xc3, 0x54, 0xe4, 0x31, 0x08, 0x04,
    0x46, 0x47, 0x56, 0xc7, 0x12, 0xa3, 0x67, 0xcf, 0x16, 0xe5, 0x20, 0x99,
    0xd1, 0xf7, 0x83, 0xfe, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
    0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
    0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
    0x00, 0x00, 0x00, 0x00
};

uint8_t Predefine_Data_Buf[MAX_PRE_SCHEDULE_SFN_WINDOW][20][1 << 18] = {};
long filesize = 0;

int set_cpu(int i)
{
    cpu_set_t mask;
    CPU_ZERO(&mask);

    CPU_SET(i, &mask);

    printf("thread %x, i = %d\n", (uint32_t)pthread_self(), i);
    if (-1 == pthread_setaffinity_np(pthread_self(), sizeof(mask), &mask)) {
        fprintf(stderr, "pthread_setaffinity_np erro\n");
        return -1;
    }
    return 0;
}

void loadTxCfg(int txCfg)
{
    gDlttiTxEnable = txCfg & 0x1;
    gUlttiTxEnable = (txCfg >> 1) & 0x1;
    gCsiTxEnable = (txCfg >> 2) & 0x1;
    gTxdataTxEnble = (txCfg >> 3) & 0x1;
}

void getTxCfg(MacTxEnable_t* macTxEnable)
{
    macTxEnable->dlttiTxEnable = gDlttiTxEnable;
    macTxEnable->txdataTxEnble = gTxdataTxEnble;
    macTxEnable->csiTxEnable = gCsiTxEnable;
    macTxEnable->ulttiTxEnable = gUlttiTxEnable;
}

void clearDltti(uint8_t dev_index, uint8_t cellid, uint16_t sfn, uint16_t slot)
{
    gL1DLTTIReq[dev_index][cellid][sfn][slot]->nPDUs = 0;
    gL1DLTTIReq[dev_index][cellid][sfn][slot]->nGroup = 0;
    gL1DlttiReqSize[dev_index][cellid][sfn][slot] = 0;
}

void clearTxdata(uint8_t dev_index, uint8_t cellid, uint16_t sfn, uint16_t slot)
{
    gL1TXDATAREQ[dev_index][cellid][sfn][slot]->numPDUs = 0;
    gL1TxdataReqSize[dev_index][cellid][sfn][slot] = 0;
}

void clearUltti(uint8_t dev_index, uint8_t cellid, uint16_t sfn, uint16_t slot)
{
    gL1ULTTI[dev_index][cellid][sfn][slot]->nPDUs = 0;
    gL1ULTTI[dev_index][cellid][sfn][slot]->nGroup = 0;
    gL1UlttiReqSize[dev_index][cellid][sfn][slot] = 0;
}

static int initCtrlMgrMsgqH()
{
    uint8_t dev, cell;
    stQuickMsgRing_para initPara;
    memset(&initPara, 0, sizeof(stQuickMsgRing_para));
    initPara.pBuf = NULL;
    initPara.size = 4096;
    for (dev = 0; dev < MAC_PC802_INDEX_MAX; dev++) {
        for (cell = 0; cell < MAC_MAX_CELL_NUM; cell++) {
            init_quickMsgRing(&gMacCtx[dev][cell].msgqH, &initPara);
            init_quickMsgRing(&gMacCtx[dev][cell].msgqApp, &initPara);
        }
    }
    return 0;
}

typedef struct
{
    uint32_t msgId;
    uint32_t msgSize;
} MsgHdr_t;

#ifdef MAC_MULTI_PC802
uint32_t macRevL1Ctrl(const char* buf, uint32_t payloadSize, uint8_t dev_index, uint8_t cellId)
#else
uint32_t macRevL1Ctrl(const char* buf, uint32_t payloadSize)
#endif
{
#ifndef MAC_MULTI_PC802
    uint8_t cellId = 0;
#endif
    unsigned long long beginTicks = 0;
    unsigned long long endTicks = 0;
    unsigned long long differVal = 0;
    beginTicks = currentcycles();
    ASSERT(buf);
    ASSERT(payloadSize > 0);
    uint32_t singleMsgLen = 0;
    uint32_t _offset = 0;
    uint32_t recvCnt = 0;
    uint8_t fapi_cellId = 0;
    // CMM_DEBUG(LOG_MAC, ">>>>>read L1Ctrl plSize[%u]", payloadSize);

    FAPI_MSG_HDR* fapiHdr = (FAPI_MSG_HDR*)buf;

    _offset = sizeof(*fapiHdr);
    uint32_t* l1MsgInput = (uint32_t*)(buf + _offset);
    recvCnt = fapiHdr->msgNum;
    fapi_cellId = fapiHdr->opaque;
    if (fapi_cellId != cellId) {
        CMM_ERROR(LOG_MAC, "CellId error! cellId: %d fapi_cellId: %d", cellId, fapi_cellId);
    }

    if (0 == recvCnt) {
        CMM_DEBUG(LOG_APP, "NO FAPI MSG");
        goto COMPLETE;
    }

    if (0xFF <= recvCnt) {
        CMM_DEBUG(LOG_APP, "FAPI Check Recv Error");
        goto COMPLETE;
    }

    while (recvCnt) {
        MsgHdr_t* msgHdr = (MsgHdr_t*)l1MsgInput;
        singleMsgLen = msgHdr->msgSize + 8;
#ifdef MAC_MULTI_PC802
        singleMsgLen = fapiMsgDec(l1MsgInput, dev_index, cellId);
#else
        singleMsgLen = fapiMsgDec(l1MsgInput, 0, 0);
#endif
        if (-1 == (int)singleMsgLen) {
            CMM_ERROR(LOG_MAC, "!!! mac Rev illegal message len %d", singleMsgLen);
            return payloadSize;
        }
        l1MsgInput = l1MsgInput + ((singleMsgLen + 3) >> 2);
        _offset += singleMsgLen;
        recvCnt--;
    }
    addPhyTraceItem((char*)fapiHdr, payloadSize);
    endTicks = currentcycles();
    differVal = endTicks - beginTicks;
    if (differVal >= 1500000) {
        CMM_ERROR(LOG_MAC, "!!! mac Rev L1Ctrl overtime ticks[%llu] ticksLimit[1500000]", differVal);
    }
    if (payloadSize != _offset) {
        CMM_WARN(LOG_MAC, "mac Rev payloadSize[%d] != _offset%d], msgNum[%d]", payloadSize, _offset, fapiHdr->msgNum);
        return payloadSize;
    }
COMPLETE:
    return _offset;
}

uint32_t macSendL1Ctrl(const char* buf, uint32_t payloadSize)
{
    if (0 == payloadSize)
        return 0;
    ASSERT(buf);
    ASSERT(payloadSize > 0);

    return 0;
}

uint32_t macSendL1Data(const char* buf, uint32_t payloadSize)
{
    ASSERT(buf);
    ASSERT(payloadSize > 0);

    CMM_DEBUG(LOG_MAC, "<<<<<write L1Data plSize[%u]", payloadSize);

    return 0;
}

static int s_rtRun[MAC_PC802_INDEX_MAX][MAC_MAX_CELL_NUM] = { { 1, 1 }, { 1, 1 }, { 1, 1 }, { 1, 1 } };

static void* rtThreadMain(void* arg)
{
    Thread_s* pThread = (Thread_s*)arg;
    if (NULL == arg) {
        printf("RT thread Start\n");
    } else {
        if (set_cpu(*(int*)pThread->UsrArg)) {
            printf("set cpu erro\n");
        }
    }

    while (s_rtRun[0][0]) {
        unsigned long long beginTicks = 0;
        unsigned long long endTicks = 0;
        unsigned long long differVal = 0;
        beginTicks = currentcycles();
#ifndef MAC_MULTI_PC802
        macRevAppCtrlMsg(0, 0);
#endif
        endTicks = currentcycles();
        differVal = endTicks - beginTicks;
        if (differVal >= 1500000) {
            CMM_ERROR(LOG_MAC, "!!! mac Rev1 L1Ctrl overtime ticks[%llu] ticksLimit[1500000]", differVal);
        }
#ifndef MAC_MULTI_PC802
        pcxxCtrlRecv();
#endif
        endTicks = currentcycles();
        differVal = endTicks - beginTicks;
        if (differVal >= 1500000) {
            CMM_ERROR(LOG_MAC, "!!! mac Rev2 L1Ctrl overtime ticks[%llu] ticksLimit[1500000]", differVal);
        }
    }
    printf("RT thread Exit\n");
    return NULL;
}

int phyRecvThreadMain(__attribute__((unused)) void* arg)
{
    while (s_rtRun[0][0]) {
        macRevAppCtrlMsg(0, 0); /*dev_index=0, cellId=0*/
#ifndef MAC_MULTI_PC802
        pcxxCtrlRecv();
#else
        pcxxCtrlRecv(0, 0); /*dev_index=0, cellId=0*/
#endif
    }
    printf("RT thread Exit\n");
    return NULL;
}

#ifdef MAC_MULTI_PC802
int phyRecvThreadMainDev0Cell1(__attribute__((unused)) void* arg)
{
    while (s_rtRun[0][1]) {
        macRevAppCtrlMsg(0, 1); /*dev_index=0, cellId=1*/
        pcxxCtrlRecv(0, 1); /*dev_index=0, cellId=1*/
    }
    printf("RT thread Exit\n");
    return NULL;
}

int phyRecvThreadMainDev1Cell0(__attribute__((unused)) void* arg)
{
    while (s_rtRun[1][0]) {
        macRevAppCtrlMsg(1, 0); /*dev_index=1, cellId=0*/
        pcxxCtrlRecv(1, 0); /*dev_index=1, cellId=0*/
    }
    printf("RT thread Exit\n");
    return NULL;
}

int phyRecvThreadMainDev1Cell1(__attribute__((unused)) void* arg)
{
    while (s_rtRun[1][1]) {
        macRevAppCtrlMsg(1, 1); /*dev_index=1, cellId=1*/
        pcxxCtrlRecv(1, 1); /*dev_index=1, cellId=1*/
    }
    printf("RT thread Exit\n");
    return NULL;
}

int phyRecvThreadMainDev2Cell0(__attribute__((unused)) void* arg)
{
    while (s_rtRun[2][0]) {
        macRevAppCtrlMsg(2, 0); /*dev_index=2, cellId=0*/
        pcxxCtrlRecv(2, 0); /*dev_index=2, cellId=0*/
    }
    printf("RT thread Exit\n");
    return NULL;
}

int phyRecvThreadMainDev2Cell1(__attribute__((unused)) void* arg)
{
    while (s_rtRun[2][1]) {
        macRevAppCtrlMsg(2, 1); /*dev_index=2, cellId=1*/
        pcxxCtrlRecv(2, 1); /*dev_index=2, cellId=1*/
    }
    printf("RT thread Exit\n");
    return NULL;
}

int phyRecvThreadMainDev3Cell0(__attribute__((unused)) void* arg)
{
    while (s_rtRun[3][0]) {
        macRevAppCtrlMsg(3, 0); /*dev_index=3, cellId=0*/
        pcxxCtrlRecv(3, 0); /*dev_index=3, cellId=0*/
    }
    printf("RT thread Exit\n");
    return NULL;
}

int phyRecvThreadMainDev3Cell1(__attribute__((unused)) void* arg)
{
    while (s_rtRun[3][1]) {
        macRevAppCtrlMsg(3, 1); /*dev_index=3, cellId=1*/
        pcxxCtrlRecv(3, 1); /*dev_index=3, cellId=1*/
    }
    printf("RT thread Exit\n");
    return NULL;
}

typedef int (*rtThreadMainFunc)(void*);
rtThreadMainFunc g_rtThreadMain[MAC_PC802_INDEX_MAX][MAC_MAX_CELL_NUM] = {
    { phyRecvThreadMain, phyRecvThreadMainDev0Cell1 },
    { phyRecvThreadMainDev1Cell0, phyRecvThreadMainDev1Cell1 },
    { phyRecvThreadMainDev2Cell0, phyRecvThreadMainDev2Cell1 },
    { phyRecvThreadMainDev3Cell0, phyRecvThreadMainDev3Cell1 },
};
#endif

void macStop(uint8_t dev_index, uint8_t cellId)
{
    s_rtRun[dev_index][cellId] = 0;
#ifndef MAC_MULTI_PC802
    pcxxCtrlDestroy();
    pcxxDataDestroy();
#else

    pcxxCtrlDestroy(dev_index, cellId);
    pcxxDataDestroy(dev_index, cellId);
#endif
}

#ifdef ATLL_PCIE
double getCpuFrequency()
{
    FILE* fp;
    char str[81];
    memset(str, 0, 81);
    fp = popen("cat /proc/cpuinfo|grep cpu\\ MHz|sed -e 's/.*:[^0-9]//'", "r");
    if (fp < 0) {
        printf("can not read CPU\n");
        exit(1);
    }
    fgets(str, 80, fp);
    fclose(fp);
    double spd = atof(str);
    return spd;
}

void macInit(TestMacRunMode_e runMode, TestMacOperMode_e operMode, int* cpuId, CmmBool_t phyStub, TestMacPdschMode_e pdschMode, uint32_t runTtiPerTc, int8_t atllloglevel, CmmBool_t sfnMode)
{
    initCtrlMgrMsgqH();
#ifdef MAC_TRACE_OPEN
    openL1Trace();
#endif
    CMM_DEBUG(LOG_MAC, "L1DLTTIReq_t size is %d", sizeof(L1DLTTIReq_t));
    CMM_DEBUG(LOG_MAC, "L1TXDATAREQ_t size is %d", sizeof(L1TXDATAREQ_t));
    CMM_DEBUG(LOG_MAC, "L1ULDCI_t size is %d", sizeof(L1ULDCI_t));
    CMM_DEBUG(LOG_MAC, "L1ULTTI_t size is %d", sizeof(L1ULTTI_t));
    double cpuFrequency = 0;
    cpuFrequency = getCpuFrequency();
#ifdef __ARM_ARCH
    maxTimeOut = 15000;
    minTimeOut = 5000;
#else
    maxTimeOut = cpuFrequency * 1000 * 0.6;
    minTimeOut = cpuFrequency * 1000 * 0.2;
#endif
    CMM_NOTICE(LOG_MAC, "MaxTime[%d], MinTime[%d]", maxTimeOut, minTimeOut);
    int i, j, k, l;
    for (l = 0; l < MAC_PC802_INDEX_MAX; l++) {
        for (k = 0; k < MAC_MAX_CELL_NUM; k++) {
            gL1ConfigRequest[l][k] = (L1ConfigRequest_t*)malloc(MAX_CONF_MSG_SIZE);
            for (i = 0; i < MAX_PRE_SCHEDULE_SFN_WINDOW; i++) {
                for (j = 0; j < 20; j++) {
                    gL1DLTTIReq[l][k][i][j] = (L1DLTTIReq_t*)malloc(MAX_DL_TTI_MSG_SIZE);
                    gL1DLTTIReq[l][k][i][j]->SFN = i;
                    gL1DLTTIReq[l][k][i][j]->Slot = j;
                    clearDltti(l, k, i, j);
                    gL1TXDATAREQ[l][k][i][j] = (L1TXDATAREQ_t*)malloc(MAX_TX_DATA_MSG_SIZE);
                    gL1TXDATAREQ[l][k][i][j]->SFN = i;
                    gL1TXDATAREQ[l][k][i][j]->Slot = j;
                    clearTxdata(l, k, i, j);
                    gL1ULTTI[l][k][i][j] = (L1ULTTI_t*)malloc(MAX_UL_TTI_MSG_SIZE);
                    gL1ULTTI[l][k][i][j]->SFN = i;
                    gL1ULTTI[l][k][i][j]->Slot = j;
                    clearUltti(l, k, i, j);
                }
            }
            gMacCtx[l][k].runMode = runMode;
            gMacCtx[l][k].operMode = operMode;
            gMacCtx[l][k].phyStub = phyStub;
            gMacCtx[l][k].pdschMode = pdschMode;
            gMacCtx[l][k].state = PHY_IDLE;
            gMacCtx[l][k].schState = SCH_STOP;
            gMacCtx[l][k].runttiPerTc = runTtiPerTc;
            gMacCtx[l][k].sfnMode = sfnMode;
        }
    }
    CMM_NOTICE(LOG_MAC, "runttiPerTc[%d]", runTtiPerTc);
#ifndef MAC_MULTI_PC802
    rte_eal_remote_launch(phyRecvThreadMain, NULL, cpuId[0]);
#else
    uint8_t pc802_index = 0;
    uint8_t cell0 = 0, cell1 = 1;
    uint8_t cpu_index = 0;
    for (pc802_index = 0; pc802_index < MAC_PC802_INDEX_MAX; pc802_index++) {
        if (g_devMapPort[pc802_index] >= 0) {
            CMM_NOTICE(LOG_MAC,"start thread for cell0,pc802_index:%d cpuId:%d\n", pc802_index, cpuId[cpu_index]);
            rte_eal_remote_launch(g_rtThreadMain[pc802_index][cell0], NULL, cpuId[cpu_index]);
            cpu_index++;
			if (g_cellNum[pc802_index] > 1) {
				CMM_NOTICE(LOG_MAC,"start thread for cell1,pc802_index:%d cpuId:%d\n", pc802_index, cpuId[cpu_index]);
                rte_eal_remote_launch(g_rtThreadMain[pc802_index][cell1], NULL, cpuId[cpu_index]);
                cpu_index++;
			}
        }
    }
#endif
}
#endif

#ifdef ATLL_SHM
void macInit(TestMacRunMode_e runMode, TestMacOperMode_e operMode, int* cpuId, CmmBool_t phyStub, TestMacPdschMode_e pdschMode, uint32_t runTtiPerTc, int8_t atllloglevel, CmmBool_t sfnMode)
{
    initCtrlMgrMsgqH();
    openL1Trace();
    CMM_DEBUG(LOG_MAC, "L1DLTTIReq_t size is %d", sizeof(L1DLTTIReq_t));
    CMM_DEBUG(LOG_MAC, "L1TXDATAREQ_t size is %d", sizeof(L1TXDATAREQ_t));
    CMM_DEBUG(LOG_MAC, "L1ULDCI_t size is %d", sizeof(L1ULDCI_t));
    CMM_DEBUG(LOG_MAC, "L1ULTTI_t size is %d", sizeof(L1ULTTI_t));
    int i, j, k;
    for (k = 0; k < MAC_MAX_CELL_NUM; k++) {
        gL1ConfigRequest[k] = (L1ConfigRequest_t*)malloc(MAX_CONF_MSG_SIZE);
        for (i = 0; i < 4; i++) {
            for (j = 0; j < 20; j++) {
                gL1DLTTIReq[k][i][j] = (L1DLTTIReq_t*)malloc(MAX_DL_TTI_MSG_SIZE);
                gL1DLTTIReq[k][i][j]->SFN = i;
                gL1DLTTIReq[k][i][j]->Slot = j;
                clearDltti(k, i, j);
                gL1TXDATAREQ[k][i][j] = (L1TXDATAREQ_t*)malloc(MAX_TX_DATA_MSG_SIZE);
                gL1TXDATAREQ[k][i][j]->SFN = i;
                gL1TXDATAREQ[k][i][j]->Slot = j;
                clearTxdata(k, i, j);
                gL1ULTTI[k][i][j] = (L1ULTTI_t*)malloc(MAX_UL_TTI_MSG_SIZE);
                gL1ULTTI[k][i][j]->SFN = i;
                gL1ULTTI[k][i][j]->Slot = j;
                clearUltti(k, i, j);
            }
        }
    }
    gMacCtx.runMode = runMode;
    gMacCtx.operMode = operMode;
    gMacCtx.phyStub = phyStub;
    gMacCtx.pdschMode = pdschMode;
    for (i = 0; i < MAC_MAX_CELL_NUM; i++) {
        gMacCtx.state[i] = PHY_IDLE;
        gMacCtx.schState[i] = SCH_STOP;
    }
    gMacCtx.runttiPerTc = runTtiPerTc;
    gMacCtx.sfnMode = sfnMode;
    pcxxInfo_s pcxxCtrl = { .readHandle = macRevL1Ctrl, .writeHandle = macSendL1Ctrl };
    pcxxInfo_s pcxxData = { .readHandle = NULL, .writeHandle = macSendL1Data };
    /*init phy pxxApi*/
    log_init(atllloglevel);
    pcxxCtrlOpen(&pcxxCtrl);
    pcxxDataOpen(&pcxxData);

    /*start rt thread*/
    Thread_s* rtThread = NULL;
    rtThread = cmm_threadCreate("rtThread", rtThreadMain, (void*)cpuId, 99, 0x100000);
    if (NULL == rtThread) {

        exit(0);
    }
}
#endif
void DlttiDefaultParams(L1DLTTIReq_t* dltti)
{

    dltti->nPDUs = 0;
    dltti->nGroup = 0;
}
void UlttiDefaultParams(L1ULTTI_t* ultti)
{
    ultti->nPDUs = 0;

    ultti->nGroup = 0;
}

void UldciDefaultParams(L1ULDCI_t* uldci)
{

    uldci->numPDUs = 0;
}

void TxdataDefaultParams(L1TXDATAREQ_t* txdata)
{

    txdata->numPDUs = 0;
}

void RxdataDefaultParams(L1RxdataIndication_t* rxdata)
{

    rxdata->numPDUs = 0;
}

void CrcDefaultParams(L1CrcIndication_t* crc)
{

    crc->numCRCs = 0;
}

void UciDefaultParams(L1UciIndication_t* uci)
{

    uci->numUcis = 0;
}

void SrsDefaultParams(L1SrsIndication_t* srs)
{

    srs->numPDUs = 0;
}

void RachDefaultParams(L1RachIndication_t* rach)
{

    rach->numPDUs = 0;
}
void setPhyState(uint8_t dev_index, uint8_t cellId, MacStatus_e state)
{
    gMacCtx[dev_index][cellId].state = state;
}

MacStatus_e getPhyState(uint8_t dev_index, uint8_t cellId)
{
    return gMacCtx[dev_index][cellId].state;
}
uint32_t ConfigReqParams(L1ConfigRequest_t* configReq, uint8_t* OutputStart)
{
    int i, j, k;
    uint32_t offset = 0;
    uint8_t* Output = OutputStart;

    offset += sizeof(L1CarriConfigInfo_t);

    offset += sizeof(L1CellConfigInfo_t);

    offset += sizeof(L1SsbConfigInfo_t);

    L1PrachConfigInfo_t* pPrachConfigInfo_t = &configReq->PRACHCfgInfo;
    offset += offsetof(L1PrachConfigInfo_t, PrachConfiguration);
    memcpy(Output, configReq, offset);
    Output += offset;
    for (i = 0; i < pPrachConfigInfo_t->numPrachConfigurations; i++) {
        L1PrachConfiguration_t* pPrachConfiguration_t = &pPrachConfigInfo_t->PrachConfiguration[i];
        offset += offsetof(L1PrachConfiguration_t, PrachFdOccaCfgInfos);
        memcpy(Output, pPrachConfiguration_t, offsetof(L1PrachConfiguration_t, PrachFdOccaCfgInfos));
        Output += offsetof(L1PrachConfiguration_t, PrachFdOccaCfgInfos);
        for (j = 0; j < pPrachConfiguration_t->numPrachFdOccasions; j++) {
            L1PrachFdOccasionConfigInfo_t* pPrachFdOccasionConfig = &pPrachConfiguration_t->PrachFdOccaCfgInfos[j];
            offset += sizeof(L1PrachFdOccasionConfigInfo_t);
            memcpy(Output, pPrachFdOccasionConfig, sizeof(L1PrachFdOccasionConfigInfo_t));
            Output += sizeof(L1PrachFdOccasionConfigInfo_t);
        }
    }

    L1SsbTableConfigInfo_t* pSsbTableConfig = &configReq->SSBtableInfo;
    offset += sizeof(L1SsbTableConfigInfo_t);
    memcpy(Output, pSsbTableConfig, sizeof(L1SsbTableConfigInfo_t));
    Output += sizeof(L1SsbTableConfigInfo_t);

    L1TddTableConfigInfo_t* pTddTableConfig = &configReq->TDDtableInfo;
    offset += sizeof(L1TddTableConfigInfo_t);
    memcpy(Output, pTddTableConfig, sizeof(L1TddTableConfigInfo_t));
    Output += sizeof(L1TddTableConfigInfo_t);

    L1MeasurementConfigInfo_t* pMeasurementConfig = &configReq->MeasurementCfgInfo;
    offset += sizeof(L1MeasurementConfigInfo_t);
    memcpy(Output, pMeasurementConfig, sizeof(L1MeasurementConfigInfo_t));
    Output += sizeof(L1MeasurementConfigInfo_t);

    L1UciConfigurationInfo_t* pUciConfigurationInfo = &configReq->UciConfigurationInfo;
    offset += offsetof(L1UciConfigurationInfo_t, mapIndex);
    memcpy(Output, pUciConfigurationInfo, offsetof(L1UciConfigurationInfo_t, mapIndex));
    Output += offsetof(L1UciConfigurationInfo_t, mapIndex);
    for (i = 0; i < pUciConfigurationInfo->numUci2Maps; i++) {
        L1MapIndex_t* pMapIndex = &pUciConfigurationInfo->mapIndex[i];
        offset += offsetof(L1MapIndex_t, sizesPart1Params);
        memcpy(Output, pMapIndex, offsetof(L1MapIndex_t, sizesPart1Params));
        Output += offsetof(L1MapIndex_t, sizesPart1Params);
        for (j = 0; j < pMapIndex->numPart1Params; j++) {
            uint8_t* psizesPart1Param = &pMapIndex->sizesPart1Params[j];
            offset++;
            memcpy(Output, psizesPart1Param, 1);
            Output++;
        }
    }

    L1PrbSym_t* pPrbSym = &configReq->PrbSymol;
    offset += offsetof(L1PrbSym_t, CoresetRmPatterns);
    memcpy(Output, pPrbSym, offsetof(L1PrbSym_t, CoresetRmPatterns));
    Output += offsetof(L1PrbSym_t, CoresetRmPatterns);
    for (i = 0; i < pPrbSym->numCoresetRmPatterns; i++) {
        L1CoresetRmPattern_t* pCoresetRmPattern = &pPrbSym->CoresetRmPatterns[i];
        offset += sizeof(L1CoresetRmPattern_t);
        memcpy(Output, pCoresetRmPattern, sizeof(L1CoresetRmPattern_t));
        Output += sizeof(L1CoresetRmPattern_t);
    }
    for (i = 0; i < pPrbSym->numberOfPrbSymbRateMatchPatterns; i++) {
        L1PrbSymbRateMatchPatterns_t* pPrbSymbRateMatchPatterns = &pPrbSym->PrbSymbRateMatchPatterns[i];
        offset += sizeof(L1PrbSymbRateMatchPatterns_t);
        memcpy(Output, pPrbSymbRateMatchPatterns, sizeof(L1PrbSymbRateMatchPatterns_t));
        Output += sizeof(L1PrbSymbRateMatchPatterns_t);
    }

    L1PucchSSConfig_t* pPucchSSConfig_t = &configReq->PucchSSConfig;
    offset += offsetof(L1PucchSSConfig_t, UlBwpIds);
    memcpy(Output, pPucchSSConfig_t, offsetof(L1PucchSSConfig_t, UlBwpIds));
    Output += offsetof(L1PucchSSConfig_t, UlBwpIds);
    for (i = 0; i < pPucchSSConfig_t->numUlBwpIds; i++) {
        L1UlBwpIds_t* pUlBwpId = &pPucchSSConfig_t->UlBwpIds[i];
        offset += sizeof(L1UlBwpIds_t);
        memcpy(Output, pUlBwpId, sizeof(L1UlBwpIds_t));
        Output += sizeof(L1UlBwpIds_t);
    }

    L1PdschConfig_t* pPdschConfig = &configReq->PdschConfig;
    offset += sizeof(L1PdschConfig_t);
    memcpy(Output, pPdschConfig, sizeof(L1PdschConfig_t));
    Output += sizeof(L1PdschConfig_t);

    L1DBTConfigInfo_t* pDBTConfigInfo = &configReq->DBTInfo;
    offset += offsetof(L1DBTConfigInfo_t, digBeam);
    memcpy(Output, pDBTConfigInfo, offsetof(L1DBTConfigInfo_t, digBeam));
    Output += offsetof(L1DBTConfigInfo_t, digBeam);
    for (i = 0; i < pDBTConfigInfo->numDigBeams; i++) {
        L1DigBeam_t* pDigBeam = (L1DigBeam_t*)((uint8_t*)configReq + offset);
        offset += offsetof(L1DigBeam_t, TXRU);
        memcpy(Output, pDigBeam, offsetof(L1DigBeam_t, TXRU));
        Output += offsetof(L1DigBeam_t, TXRU);
        for (j = 0; j < configReq->DBTInfo.numBasebandPorts; j++) {
            L1TxRu_t* pTxRu = (L1TxRu_t*)((uint8_t*)configReq + offset);
            offset += sizeof(L1TxRu_t);
            memcpy(Output, pTxRu, sizeof(L1TxRu_t));
            Output += sizeof(L1TxRu_t);
        }
    }

    L1PMCfgTableInfo_t* pPMConfigTable = &configReq->PMInfo;
    offset += offsetof(L1PMCfgTableInfo_t, pmTable);
    memcpy(Output, pPMConfigTable, offsetof(L1PMCfgTableInfo_t, pmTable));
    Output += offsetof(L1PMCfgTableInfo_t, pmTable);
    for (j = 0; j < pPMConfigTable->numPMTables; j++) {
        L1PMConfigInfo_t* pPMConfigInfo = (L1PMConfigInfo_t*)((uint8_t*)configReq + offset);
        offset += offsetof(L1PMConfigInfo_t, layers);
        memcpy(Output, pPMConfigInfo, offsetof(L1PMConfigInfo_t, layers));
        Output += offsetof(L1PMConfigInfo_t, layers);
        for (i = 0; i < pPMConfigInfo->numLayers; i++) {
            L1PMLayer_t* pPMLayer = (L1PMLayer_t*)((uint8_t*)configReq + offset);
            for (k = 0; k < pPMConfigInfo->numAntPorts; k++) {
                L1PrecoderInfo_t* pPrecoder = (L1PrecoderInfo_t*)((uint8_t*)configReq + offset);;
                offset += sizeof(L1PrecoderInfo_t);
                memcpy(Output, pPrecoder, sizeof(L1PrecoderInfo_t));
                Output += sizeof(L1PrecoderInfo_t);
            }
        }
    }

    return offset;
}

uint16_t getCarrierConfig(L1ConfigRequest_t* configReq, uint8_t dev_index, uint8_t cellId)
{
    L1CarriConfigInfo_t* pCarrier = &configReq->CarriConfig;

    if (pCarrier->dlGridSize[0] != 0) {
        macCellSetScs(15, dev_index, cellId);
		return 15;
    }
    if (pCarrier->dlGridSize[1] != 0) {
        macCellSetScs(30, dev_index, cellId);
		return 30;
    }
    if (pCarrier->dlGridSize[2] != 0) {
        macCellSetScs(60, dev_index, cellId);
		return 60;
    }
    if (pCarrier->dlGridSize[3] != 0) {
        macCellSetScs(120, dev_index, cellId);
		return 120;
    }
    if (pCarrier->dlGridSize[4] != 0) {
        macCellSetScs(240, dev_index, cellId);
		return 240;
    }
}

typedef enum {
    PDCCH_PDU,
    PDSCH_PDU,
    CSI_RS_PDU,
    SSB_PDU
} DlPduType_ev;

uint32_t DlttiParams(L1DLTTIReq_t* dltti, uint8_t* OutputStart, uint8_t dev_index, uint8_t cellId)
{
    uint32_t offset = 0;
    uint16_t j, i;
    uint8_t* Output = OutputStart;

    /*update sfn and slot*/
    dltti->SFN = gMacTxRfn[dev_index][cellId];
    dltti->Slot = gMacTxSlot[dev_index][cellId];

    offset += offsetof(L1DLTTIReq_t, pdus);
    memcpy(Output, dltti, offset);
    Output += offset;

    for (j = 0; j < dltti->nPDUs; j++) {
        uint16_t PduSize = offset;
        L1DlPduInfo_t* pDlttiPdu = (L1DlPduInfo_t*)((uint8_t*)dltti + offset);
        offset += offsetof(L1DlPduInfo_t, DlPDUConfig);
        uint8_t* OutputSize = Output;
        Output += offsetof(L1DlPduInfo_t, DlPDUConfig);
        switch (pDlttiPdu->PDUType) {
        case PDCCH_PDU: {
            L1PdcchPduInfo_t* PdcchPduInfo = &pDlttiPdu->DlPDUConfig.u.PdcchPdu;
            offset += offsetof(L1PdcchPduInfo_t, DlDCIs);
            memcpy(Output, PdcchPduInfo, offsetof(L1PdcchPduInfo_t, DlDCIs));
            Output += offsetof(L1PdcchPduInfo_t, DlDCIs);

            for (i = 0; i < PdcchPduInfo->numDlDci; i++) {
                L1DlDciInfo_t* DlDciInfo = &PdcchPduInfo->DlDCIs[i];
                offset += sizeof(L1DlDciInfo_t);
                memcpy(Output, DlDciInfo, sizeof(L1DlDciInfo_t));
                Output += sizeof(L1DlDciInfo_t);
				L1TxPrecodingBeamformingPdu_t* txPrecodeBeamInfo = &DlDciInfo->BeamformingInfo;
				for (i = 0; i < txPrecodeBeamInfo->numPRGs; i++) {
		            uint16_t* PMidx = (uint16_t*)((uint8_t*)dltti + offset);					
		            offset += 2;
		            memcpy(Output, PMidx, 2);
		            Output += 2;
		        }
		        for (i = 0; i < txPrecodeBeamInfo->digBFInterfaces; i++) {
		            uint16_t* beamIdx = (uint16_t*)((uint8_t*)dltti + offset);
		            offset += 2;
		            memcpy(Output, beamIdx, 2);
		            Output += 2;
		        }
            }
        } break;
        case PDSCH_PDU: {
            L1PdschPduInfo_t* pPdschdu = &pDlttiPdu->DlPDUConfig.u.PdschPdu;
            offset += offsetof(L1PdschPduInfo_t, CodewordInfo);
            memcpy(Output, pPdschdu, offsetof(L1PdschPduInfo_t, CodewordInfo));
            Output += offsetof(L1PdschPduInfo_t, CodewordInfo);

            L1CodewordInfo_t* pCodeWordInfo = &pPdschdu->CodewordInfo;
            offset += sizeof(L1CodewordInfo_t);
            memcpy(Output, pCodeWordInfo, sizeof(L1CodewordInfo_t));
            Output += sizeof(L1CodewordInfo_t);
            offset += 8;
            memcpy(Output, &(pPdschdu->nIdPdsch), 8);
            Output += 8;
            L1PdschDmrsInfo_t* pPdschDmrsInfo = &pPdschdu->DMRSInfo;
            offset += sizeof(L1PdschDmrsInfo_t);
            memcpy(Output, pPdschDmrsInfo, sizeof(L1PdschDmrsInfo_t));
            Output += sizeof(L1PdschDmrsInfo_t);

            L1PdschAllocationFdInfo_t* pPdschAllocationFdInfo = &pPdschdu->ResourceAllocFd;
            offset += sizeof(L1PdschAllocationFdInfo_t);
            memcpy(Output, pPdschAllocationFdInfo, sizeof(L1PdschAllocationFdInfo_t));
            Output += sizeof(L1PdschAllocationFdInfo_t);

            L1RateMatchingInformation_t* pRateMatchingInformation = &pPdschdu->RateMatchingInformation;
            offset += offsetof(L1RateMatchingInformation_t, coresetRMPIndex);
            memcpy(Output, pRateMatchingInformation, offsetof(L1RateMatchingInformation_t, coresetRMPIndex));
            Output += offsetof(L1RateMatchingInformation_t, coresetRMPIndex);
            for (i = 0; i < pRateMatchingInformation->numCoresetRMP; i++) {
                uint8_t* CoresetRMP = &pRateMatchingInformation->coresetRMPIndex[i];
                offset++;
                memcpy(Output, CoresetRMP, 1);
                Output++;
            }
            for (i = 0; i < pRateMatchingInformation->numCsiRsForRM; i++) {
                uint16_t* CsiRsForRM = &pRateMatchingInformation->csiRsForRM[i];
                offset += 2;
                memcpy(Output, CsiRsForRM, 2);
                Output += 2;
            }

            L1PxschAllocationTdInfo_t* pPxschAllocationTdInfo = &pPdschdu->ResourceAllocTd;
            offset += sizeof(L1PxschAllocationTdInfo_t);
            memcpy(Output, pPxschAllocationTdInfo, sizeof(L1PxschAllocationTdInfo_t));
            Output += sizeof(L1PxschAllocationTdInfo_t);

            L1TxPowerInfo_t* pTxPowerInfo = &pPdschdu->TxPowerInfo;
            offset += sizeof(L1TxPowerInfo_t);
            memcpy(Output, pTxPowerInfo, sizeof(L1TxPowerInfo_t));
            Output += sizeof(L1TxPowerInfo_t);

            if (0 != (pPdschdu->pduBitmap & 1)) {
                L1PtrsInfo_t* pPtrsInfo = &pPdschdu->PTRS;
                offset += sizeof(L1PtrsInfo_t);
                memcpy(Output, pPtrsInfo, sizeof(L1PtrsInfo_t));
                Output += sizeof(L1PtrsInfo_t);
            }

            if (0 != (pPdschdu->pduBitmap & 2)) {
                L1CbgFeildInfo_t* pCbgFeildInfo = &pPdschdu->CbgInfo;
                offset += sizeof(L1CbgFeildInfo_t);
                memcpy(Output, pCbgFeildInfo, sizeof(L1CbgFeildInfo_t));
                Output += sizeof(L1CbgFeildInfo_t);
            }
			
			L1TxPrecodingBeamformingPdu_t* txPrecodeBeamInfo = &pPdschdu->BeamformingInfo;
			offset += sizeof(L1TxPrecodingBeamformingPdu_t);
            memcpy(Output, txPrecodeBeamInfo, sizeof(L1TxPrecodingBeamformingPdu_t));
            Output += sizeof(L1TxPrecodingBeamformingPdu_t);
			for (i = 0; i < txPrecodeBeamInfo->numPRGs; i++) {
                uint16_t* PMidx = (uint16_t*)((uint8_t*)dltti + offset);
                offset += 2;
                memcpy(Output, PMidx, 2);
                Output += 2;
            }
            for (i = 0; i < txPrecodeBeamInfo->digBFInterfaces; i++) {
                uint16_t* beamIdx = (uint16_t*)((uint8_t*)dltti + offset);
                offset += 2;
                memcpy(Output, beamIdx, 2);
                Output += 2;
            }
        } break;
        case CSI_RS_PDU: {
            L1CsiRsPduInfo_t* CsiPduInfo = &pDlttiPdu->DlPDUConfig.u.CsiRsPdu;
            offset += sizeof(L1CsiRsPduInfo_t);
            memcpy(Output, CsiPduInfo, sizeof(L1CsiRsPduInfo_t));
            Output += sizeof(L1CsiRsPduInfo_t);
            L1TxPrecodingBeamformingPdu_t* txPrecodeBeamInfo = &CsiPduInfo->BeamformingInfo;
	    for (i = 0; i < txPrecodeBeamInfo->numPRGs; i++) {
	        uint16_t* PMidx = (uint16_t*)((uint8_t*)dltti + offset);
	        offset += 2;
	        memcpy(Output, PMidx, 2);
	        Output += 2;
	    }
	    for (i = 0; i < txPrecodeBeamInfo->digBFInterfaces; i++) {
	        uint16_t* beamIdx = (uint16_t*)((uint8_t*)dltti + offset);
	        offset += 2;
	        memcpy(Output, beamIdx, 2);
	        Output += 2;
	    }
        } break;
        case SSB_PDU: {
            L1SsbPduInfo_t* SsbPduInfo = &pDlttiPdu->DlPDUConfig.u.SsbPdu;
            offset += sizeof(L1SsbPduInfo_t);
            memcpy(Output, SsbPduInfo, sizeof(L1SsbPduInfo_t));
            Output += sizeof(L1SsbPduInfo_t);
	    L1TxPrecodingBeamformingPdu_t* txPrecodeBeamInfo = &SsbPduInfo->BeamformingInfo;
	    for (i = 0; i < txPrecodeBeamInfo->numPRGs; i++) {
	        uint16_t* PMidx = (uint16_t*)((uint8_t*)dltti + offset);
	        offset += 2;
	        memcpy(Output, PMidx, 2);
                 Output += 2;
            }
	    for (i = 0; i < txPrecodeBeamInfo->digBFInterfaces; i++) {
	        uint16_t* beamIdx = (uint16_t*)((uint8_t*)dltti + offset);
	        offset += 2;
                memcpy(Output, beamIdx, 2);
     		Output += 2;
	     }
          } break;
        }
        pDlttiPdu->PDUSize = offset - PduSize;
        memcpy(OutputSize, pDlttiPdu, offsetof(L1DlPduInfo_t, DlPDUConfig));
    }

    for (j = 0; j < dltti->nGroup; j++) {
        L1DlGroupInfo_t* pDlGroup = (L1DlGroupInfo_t*)((uint8_t*)dltti + offset);
        offset += offsetof(L1DlGroupInfo_t, Ues);
        memcpy(Output, pDlGroup, offsetof(L1DlGroupInfo_t, Ues));
        Output += offsetof(L1DlGroupInfo_t, Ues);
        for (i = 0; i < pDlGroup->nUe; i++) {
            L1GroupUeInfo_t* pGroupUe = &pDlGroup->Ues[i];
            offset += sizeof(L1GroupUeInfo_t);
            memcpy(Output, pGroupUe, sizeof(L1GroupUeInfo_t));
            Output += sizeof(L1GroupUeInfo_t);
        }
    }

    return offset;
}

typedef enum {
    PRACH_PDU,
    PUSCH_PDU,
    PUCCH_PDU,
    SRS_PDU
} UlPduType_ev;

uint32_t UlttiParams(L1ULTTI_t* ultti, uint8_t* OutputStart, uint8_t dev_index, uint8_t cellId)
{
    int j, i;
    uint32_t offset = 0;
    uint8_t* Output = OutputStart;

    ultti->SFN = gMacRxRfn[dev_index][cellId];
    ultti->Slot = gMacRxSlot[dev_index][cellId];

    L1PuschPduInfo_t* pPuschPdu;
    L1PuschPtrs_t* pPuschPtrs;
    offset += offsetof(L1ULTTI_t, pdus);
    memcpy(Output, ultti, offset);
    Output += offset;

    for (j = 0; j < ultti->nPDUs; j++) {
        uint16_t PduSize = offset;
        L1UlPduInfo_t* pUlttiPdu = (L1UlPduInfo_t*)((uint8_t*)ultti + offset);
        offset += offsetof(L1UlPduInfo_t, UlPDUConfig);
        uint8_t* OutputSize = Output;
        Output += offsetof(L1UlPduInfo_t, UlPDUConfig);

        switch (pUlttiPdu->PDUType) {
        case PRACH_PDU: {
            L1PrachPdu_t* pPrachPdu = &pUlttiPdu->UlPDUConfig.u.PrachPdu;
            offset += sizeof(L1PrachPdu_t);
            memcpy(Output, pPrachPdu, sizeof(L1PrachPdu_t));
            Output += sizeof(L1PrachPdu_t);
            break;
        }
        case PUSCH_PDU: {
            pPuschPdu = &pUlttiPdu->UlPDUConfig.u.PuschPdu;
            offset += offsetof(L1PuschPduInfo_t, puschData);
            memcpy(Output, pPuschPdu, offsetof(L1PuschPduInfo_t, puschData));
            Output += offsetof(L1PuschPduInfo_t, puschData);
            if (0 != (pPuschPdu->pduBitmap & 1)) {
                L1PuschData_t* pPuschData = &pPuschPdu->puschData;
                offset += sizeof(L1PuschData_t);
                memcpy(Output, pPuschData, sizeof(L1PuschData_t));
                Output += sizeof(L1PuschData_t);
            }
            if (0 != (pPuschPdu->pduBitmap & 2)) {
	            L1PuschUci_t* pPuschUci = &pPuschPdu->puschUci;
                offset += offsetof(L1PuschUci_t, uci1ToUci2Info);
                memcpy(Output, pPuschUci, offsetof(L1PuschUci_t, uci1ToUci2Info));
                Output += offsetof(L1PuschUci_t, uci1ToUci2Info);
		        L1Uci1ToUci2Info_t* pUciInfo = &pPuschUci->uci1ToUci2Info;
		        offset += offsetof(L1Uci1ToUci2Info_t, UCIpart2);
		        memcpy(Output, pUciInfo, offsetof(L1Uci1ToUci2Info_t, UCIpart2));
		        Output += offsetof(L1Uci1ToUci2Info_t, UCIpart2);					
		        for (i = 0; i < pPuschUci->uci1ToUci2Info.numPart2Types; i++) {
			        L1UCIPart2Types_t* pPart2Types = &pUciInfo->UCIpart2[i];
			        offset += sizeof(L1UCIPart2Types_t);
			        memcpy(Output, pPart2Types, sizeof(L1UCIPart2Types_t));
			        Output += sizeof(L1UCIPart2Types_t);
		        }
            }
            if (0 != (pPuschPdu->pduBitmap & 4)) {
                pPuschPtrs = &pPuschPdu->puschPtrs;
                offset += offsetof(L1PuschPtrs_t, PtrsPorts);
                memcpy(Output, pPuschPtrs, offsetof(L1PuschPtrs_t, PtrsPorts));
                Output += offsetof(L1PuschPtrs_t, PtrsPorts);
                for (i = 0; i < pPuschPtrs->numPtrsPorts; i++) {
                    L1PtrsPortInfo_t* pPtrsPortInfo = &pPuschPtrs->PtrsPorts[i];
                    offset += sizeof(L1PtrsPortInfo_t);
                    memcpy(Output, pPtrsPortInfo, sizeof(L1PtrsPortInfo_t));
                    Output += sizeof(L1PtrsPortInfo_t);
                }
            }
            if (0 != (pPuschPdu->pduBitmap & 8)) {
                L1DftsOfdm_t* pDftsOfdm = &pPuschPdu->dftsOfdm;
                offset += sizeof(L1DftsOfdm_t);
                memcpy(Output, pDftsOfdm, sizeof(L1DftsOfdm_t));
                Output += sizeof(L1DftsOfdm_t);
            }
            break;
        }
        case PUCCH_PDU: {			
	        L1PucchPduInfo_t* pPucchPduInfo = &pUlttiPdu->UlPDUConfig.u.PucchPdu;
            offset += offsetof(L1PucchPduInfo_t,DMRS);
            memcpy(Output, pPucchPduInfo, offsetof(L1PucchPduInfo_t,DMRS));
            Output += offsetof(L1PucchPduInfo_t,DMRS);
	        L1PucchDMRSInfo_t* pDMRS =&pPucchPduInfo->DMRS;
	        offset += offsetof(L1PucchDMRSInfo_t, uci1ToUci2Info);
            memcpy(Output, pDMRS, offsetof(L1PucchDMRSInfo_t, uci1ToUci2Info));
            Output += offsetof(L1PucchDMRSInfo_t, uci1ToUci2Info);
	        L1Uci1ToUci2Info_t* pUciInfo = &pDMRS->uci1ToUci2Info;
	        offset += offsetof(L1Uci1ToUci2Info_t, UCIpart2);
	        memcpy(Output, pUciInfo, offsetof(L1Uci1ToUci2Info_t, UCIpart2));
	        Output += offsetof(L1Uci1ToUci2Info_t, UCIpart2);
	        for(i = 0; i < pPucchPduInfo->DMRS.uci1ToUci2Info.numPart2Types; i++) {
                L1UCIPart2Types_t* pPart2Types = &pUciInfo->UCIpart2[i];
	            offset += sizeof(L1UCIPart2Types_t);
	            memcpy(Output, pPart2Types, sizeof(L1UCIPart2Types_t));
	            Output += sizeof(L1UCIPart2Types_t);
	        }
            break;
        }
        case SRS_PDU: {
            L1SrsPdu_t* pSrsPdu = &pUlttiPdu->UlPDUConfig.u.SrsPdu;
            offset += sizeof(L1SrsPdu_t);
            memcpy(Output, pSrsPdu, sizeof(L1SrsPdu_t));
            Output += sizeof(L1SrsPdu_t);
            break;
        }
        }
        pUlttiPdu->PDUSize = offset - PduSize;
        memcpy(OutputSize, pUlttiPdu, offsetof(L1UlPduInfo_t, UlPDUConfig));
    }

    for (j = 0; j < ultti->nGroup; j++) {
        L1UlGroupInfo_t* pUlGroup = (L1UlGroupInfo_t*)((uint8_t*)ultti + offset);
        offset += offsetof(L1UlGroupInfo_t, Ues);
        memcpy(Output, pUlGroup, offsetof(L1UlGroupInfo_t, Ues));
        Output += offsetof(L1UlGroupInfo_t, Ues);
        for (i = 0; i < pUlGroup->nUe; i++) {
            L1GroupUeInfo_t* pGroupUe = &pUlGroup->Ues[i];
            offset += sizeof(L1GroupUeInfo_t);
            memcpy(Output, pGroupUe, sizeof(L1GroupUeInfo_t));
            Output += sizeof(L1GroupUeInfo_t);
        }
    }

    return offset;
}

void macTcLoad(uint16_t id, uint8_t cellNum, uint8_t dev_index)
{
    uint16_t i, j, _SFN;
	uint8_t _cellid = cellNum;


    for (i = 0; i < MAX_PRE_SCHEDULE_SFN_WINDOW; i++) {
        for (j = 0; j < 20; j++) {
            clearDltti(dev_index, _cellid, i, j);
            clearTxdata(dev_index, _cellid, i, j);
            clearUltti(dev_index, _cellid, i, j);
        }
    }

    uint8_t _slot;
    //uint8_t _cellid = cellNum;
    char fileName[256];
    FILE* fp;

    printf("macTcLoad, cellId = %d dev_index = %d\n", cellNum, dev_index);
    //for (_cellid = 0; _cellid < cellNum; _cellid++) {
        memset(fileName, 0, 256);
        sprintf(fileName, "test_CPHY/test_CPHY.t100.1.%d/config/fapi_config_cell%d_request.dat", id, _cellid);
        fp = fopen(fileName, "rb");
        // fseek(fp, 0L, SEEK_END);
        if (!fp) {
            printf("open %s error\n", fileName);
        } else {
            fseek(fp, 0L, SEEK_END);
            gL1ConfigReqSize[dev_index][_cellid] = ftell(fp);
            fseek(fp, 0L, SEEK_SET);
            fread(gL1ConfigRequest[dev_index][_cellid], gL1ConfigReqSize[dev_index][_cellid], 1, fp);
            fclose(fp);
        }
        // fclose(fp);
		uint16_t max_slot_num;
		uint8_t *pConfigReq = (uint8_t*)gL1ConfigRequest[dev_index][_cellid] + 8;
		(void)getCarrierConfig((L1ConfigRequest_t*)pConfigReq, dev_index, _cellid);
		max_slot_num = macGetSlotNumPerSfn(dev_index, _cellid);
		printf("max_slot_num :%d\n",max_slot_num);

        for (i = 0; i < 1; i++) {
            for (_SFN = 0; _SFN < MAX_PRE_SCHEDULE_SFN_WINDOW; _SFN++) {
                for (j = 0; j < max_slot_num; j++) {
                    _slot = j;
                    memset(fileName, 0, 256);
                    sprintf(fileName, "test_CPHY/test_CPHY.t100.1.%d/frame_config_sfn%d/fapi_ultti_request_cell%d_sfn%d_slot%d.dat", id, _SFN, _cellid, _SFN, _slot);
                    fp = fopen(fileName, "rb");
                    // fseek(fp, 0L, SEEK_END);
                    if (!fp) {
                        printf("open %s error\n", fileName);
                    } else {
                        fseek(fp, 0L, SEEK_END);
                        gL1UlttiReqSize[dev_index][_cellid][_SFN & MAX_PRE_SCHEDULE_SFN_WINDOW_MASK][_slot] = ftell(fp);
                        fseek(fp, 0L, SEEK_SET);
                        fread(gL1ULTTI[dev_index][_cellid][_SFN & MAX_PRE_SCHEDULE_SFN_WINDOW_MASK][_slot], gL1UlttiReqSize[_cellid][_SFN & MAX_PRE_SCHEDULE_SFN_WINDOW_MASK][_slot], 1, fp);
                        fclose(fp);
                    }
                    // fclose(fp);

                    memset(fileName, 0, 256);
                    sprintf(fileName, "test_CPHY/test_CPHY.t100.1.%d/frame_config_sfn%d/fapi_dltti_request_cell%d_sfn%d_slot%d.dat", id, _SFN, _cellid, _SFN, _slot);
                    fp = fopen(fileName, "rb");
                    // fseek(fp, 0L, SEEK_END);
                    if (!fp) {
                        printf("open %s error\n", fileName);
                    } else {
                        fseek(fp, 0L, SEEK_END);
                        gL1DlttiReqSize[dev_index][_cellid][_SFN & MAX_PRE_SCHEDULE_SFN_WINDOW_MASK][_slot] = ftell(fp);
                        fseek(fp, 0L, SEEK_SET);
                        fread(gL1DLTTIReq[dev_index][_cellid][_SFN & MAX_PRE_SCHEDULE_SFN_WINDOW_MASK][_slot], gL1DlttiReqSize[_cellid][_SFN & MAX_PRE_SCHEDULE_SFN_WINDOW_MASK][_slot], 1, fp);
                        fclose(fp);
                    }
                    // fclose(fp);

                    memset(fileName, 0, 256);
                    sprintf(fileName, "test_CPHY/test_CPHY.t100.1.%d/frame_config_sfn%d/fapi_txdata_request_cell%d_sfn%d_slot%d.dat", id, _SFN, _cellid, _SFN, _slot);
                    fp = fopen(fileName, "rb");
                    // fseek(fp, 0L, SEEK_END);
                    if (!fp) {
                        printf("open %s error\n", fileName);
                    } else {
                        fseek(fp, 0L, SEEK_END);
                        gL1TxdataReqSize[dev_index][_cellid][_SFN & MAX_PRE_SCHEDULE_SFN_WINDOW_MASK][_slot] = ftell(fp);
                        fseek(fp, 0L, SEEK_SET);
                        fread(gL1TXDATAREQ[dev_index][_cellid][_SFN & MAX_PRE_SCHEDULE_SFN_WINDOW_MASK][_slot], gL1TxdataReqSize[_cellid][_SFN & MAX_PRE_SCHEDULE_SFN_WINDOW_MASK][_slot], 1, fp);
                        fclose(fp);
                    }
                    // fclose(fp);
                    _slot++;
                }
            }
        }
    //}
}

void macSchedule(uint16_t sfn, uint16_t slot, uint8_t dev_index, uint8_t cellId)
{
    if (SCH_STOP == gMacCtx[dev_index][cellId].schState)
        return;
    macTtiTx(sfn, slot, dev_index, cellId);
}
void macRevCfgResonse(L1ConfigResponse_t* rsp, uint8_t dev_index, uint8_t cellId)
{
    CMM_DEBUG(LOG_MAC, "dev_index:%d cellId: %d, PHY CFG RSP", dev_index, cellId);
    if (rsp->ErrorCode == 0) {
        setPhyState(dev_index, cellId, PHY_INIT);
        CMM_DEBUG(LOG_MAC, "PHY START REQ");
        sendPhyStartReq(dev_index, cellId);
        if (AUTO == gMacCtx[dev_index][cellId].operMode) {
            // sendPhyStartReq();
            setPhyState(dev_index, cellId, PHY_INIT);
            CMM_DEBUG(LOG_MAC, "PHY START REQ");
            if (TRUE == gMacCtx[dev_index][cellId].phyStub) {
                L1ErrorIndication_t _ind;
                sleep(1);
                _ind.MessageId = L1_START_REQUEST;
                _ind.ErrorCode = 0;
                macRevErrorIndication(&_ind, dev_index, cellId);
            }
        }
    } else {
        setPhyState(dev_index, cellId, PHY_IDLE);
    }
}

void macRevStopIndication(uint8_t dev_index, uint8_t cellId)
{
    setPhyState(dev_index, cellId, PHY_IDLE);

    if ((AUTO == gMacCtx[dev_index][cellId].operMode) && (gMacCtx[dev_index][cellId].schState != SCH_STOP))
        return;
    sendPhyConfigReq(dev_index, cellId);
    setPhyState(dev_index, cellId, PHY_CFG);

    if (TRUE == gMacCtx[dev_index][cellId].phyStub) {
        L1ConfigResponse_t _rsp;
        sleep(1);
        _rsp.ErrorCode = 0;
        macRevCfgResonse(&_rsp, dev_index, cellId);
    }
}

void macRevErrorIndication(L1ErrorIndication_t* errorInd, uint8_t dev_index, uint8_t cellId)
{
    switch (errorInd->MessageId) {

    case L1_START_REQUEST:
        CMM_DEBUG(LOG_MAC, "PHY START RSP");
        if (errorInd->ErrorCode != 0) {
            setPhyState(dev_index, cellId, PHY_IDLE);
        } else {
            setPhyState(dev_index, cellId, PHY_ACTIVE);

            return;
        }
        break;

    case L1_STOP_REQUEST:
        setPhyState(dev_index, cellId, PHY_IDLE);
        break;
    default:
        break;
    }
    if (errorInd->ErrorCode != 0)
        CMM_WARN(LOG_MAC, "Rev error indication %d", errorInd->ErrorCode);
}

extern uint32_t gcurrentScs[MAC_PC802_INDEX_MAX][MAC_MAX_CELL_NUM];
void macTxStartIndHandle(L1SlotIndication_t* slotInd, uint8_t dev_index, uint8_t cellId)
{
    unsigned long long beginTicks = 0;
    unsigned long long endTicks = 0;
    unsigned long long differVal = 0;
    static L1SlotIndication_t sMacLocalGSfn[MAC_PC802_INDEX_MAX][MAC_MAX_CELL_NUM] = { 0 };
    if (PHY_INIT == getPhyState(dev_index, cellId)) {
        setPhyState(dev_index, cellId, PHY_ACTIVE);
        gMacCtx[dev_index][cellId].schState = SCH_NORMAL;
        gSystemTtiNum[dev_index][cellId] = 0;
        sMacLocalGSfn[dev_index][cellId].SFN = 0;
        sMacLocalGSfn[dev_index][cellId].Slot = 0;
    }
    if (PHY_ACTIVE != getPhyState(dev_index, cellId))
        return;

    sMacLocalGSfn[dev_index][cellId] = *slotInd;

    if (0 != gMacCtx[dev_index][cellId].runttiPerTc) {
        if ((gSystemTtiNum[dev_index][cellId] != 0) && (gSystemTtiNum[dev_index][cellId] % gMacCtx[dev_index][cellId].runttiPerTc == 0)) {
            sendPhyStopReq(dev_index, cellId);
            CMM_DEBUG(LOG_MAC, "PHY STOP REQ CELL[%d]", cellId);
            gMacCtx[dev_index][cellId].schState = SCH_STOP;
            CMM_DEBUG(LOG_MAC, "-------- [%d]slot has been resolved ---------", gSystemTtiNum[dev_index][cellId]);
            gSystemTtiNum[dev_index][cellId]++;
            return;
        }
    }
    gSystemTtiNum[dev_index][cellId]++;
    gL2TxGSfn[dev_index][cellId] = gsfnShift(sMacLocalGSfn[dev_index][cellId], 2, gcurrentScs[dev_index][cellId]);
    gL2RxGSfn[dev_index][cellId] = gsfnShift(sMacLocalGSfn[dev_index][cellId], 2, gcurrentScs[dev_index][cellId]); // sMacLocalGSfn;

    sMacLocalGSfn[dev_index][cellId] = gsfnShift(sMacLocalGSfn[dev_index][cellId], 1, gcurrentScs[dev_index][cellId]);
    gMacTxRfn[dev_index][cellId] = gL2TxGSfn[dev_index][cellId].SFN;
    gMacTxSlot[dev_index][cellId] = gL2TxGSfn[dev_index][cellId].Slot;
    gMacRxRfn[dev_index][cellId] = gL2RxGSfn[dev_index][cellId].SFN;
    gMacRxSlot[dev_index][cellId] = gL2RxGSfn[dev_index][cellId].Slot;
    beginTicks = currentcycles();
    macSchedule(gMacTxRfn[dev_index][cellId], gMacTxSlot[dev_index][cellId], dev_index, cellId);
    endTicks = currentcycles();
    differVal = endTicks - beginTicks;
    if (differVal >= maxTimeOut) {
        CMM_ERROR(LOG_MAC, "!!! Dev:channel- [%d:%d] L2L_MSG RING Run overtime ticks[%llu] ticksLimit[%d]", dev_index, cellId, differVal, maxTimeOut);
    }
}
void macCtrlMgrSend(uint8_t dev_index, uint8_t cellId, char* pBuf, uint32_t bufLen)
{

    gMacCtx[dev_index][cellId].msgqH.pfnQuickMsgRingSendMsg(&gMacCtx[dev_index][cellId].msgqH, pBuf, bufLen);
}
void macAppMgrSend(uint8_t dev_index, uint8_t cellId, char* pBuf, uint32_t bufLen)
{

    gMacCtx[dev_index][cellId].msgqApp.pfnQuickMsgRingSendMsg(&gMacCtx[dev_index][cellId].msgqApp, pBuf, bufLen);
}

void macDlSchBuildPayload(uint32_t size, uint32_t* offset, uint8_t dev_index, uint8_t cellId)
{
    uint32_t length = 0;
    char* dest = NULL;
    uint32_t initSize = size;

    char fileName[256];
    uint32_t cfgOffset = *offset;
    memset(fileName, 0, 256);
#ifndef MAC_MULTI_PC802
    if (0 == pcxxDataAlloc(size, &dest, offset))
#else
    if (0 == pcxxDataAlloc(size, &dest, offset, dev_index, cellId))
#endif
    {
        if (gMacCtx[dev_index][cellId].pdschMode == PDSCHMODE_AUTO_MODE) {
            while (size) {
                length = (size > sizeof(BsMacPayloads_pdsch)) ? sizeof(BsMacPayloads_pdsch) : size;
                memcpy(dest, BsMacPayloads_pdsch, length);
                size -= length;
                dest += length;
            }
        } else if (gMacCtx[dev_index][cellId].pdschMode == PDSCHMODE_LOAD_MODE) {
            char* firstAddr = (char*)Predefine_Data_Buf[(gL2TxGSfn[dev_index][cellId].SFN) & MAX_PRE_SCHEDULE_SFN_WINDOW_MASK][gL2TxGSfn[dev_index][cellId].Slot] + cfgOffset;
            memcpy(dest, firstAddr, initSize);
        }
#ifndef MAC_MULTI_PC802
        pcxxDataSend(*offset, initSize);
#else
        pcxxDataSend(*offset, initSize, dev_index, cellId);
#endif
    } else
        CMM_ERROR(LOG_MAC, "alloc data buffer: error \n");
}

uint32_t TxdataParams(L1TXDATAREQ_t* txdata, uint8_t* OutputStart, uint8_t dev_index, uint8_t cellId)
{
    uint32_t j, i;
    uint32_t offset = 0;
    uint8_t* Output = OutputStart;

    txdata->SFN = gMacTxRfn[dev_index][cellId];
    txdata->Slot = gMacTxSlot[dev_index][cellId];

    L1TxDataPduInfo_t* pTxdataPdu;
    offset += offsetof(L1TXDATAREQ_t, pdus);
    memcpy(Output, txdata, offset);
    Output += offset;
    for (j = 0; j < txdata->numPDUs; j++) {
        uint32_t PduSize = offset;
        pTxdataPdu = (L1TxDataPduInfo_t*)((char*)txdata + offset);
        offset += offsetof(L1TxDataPduInfo_t, TLVs);
        uint8_t* OutputPos = Output;
        Output += offsetof(L1TxDataPduInfo_t, TLVs);

        for (i = 0; i < pTxdataPdu->numTLV; i++) {
            L1DataTLV_t* pDataTLV = &pTxdataPdu->TLVs[i];
            if (pDataTLV->Tag == 2)
                macDlSchBuildPayload(pDataTLV->Length, &pDataTLV->Value, dev_index, cellId);
            offset += sizeof(L1DataTLV_t);
            memcpy(Output, pDataTLV, sizeof(L1DataTLV_t));
            Output += sizeof(L1DataTLV_t);
        }
        memcpy(OutputPos, pTxdataPdu, offsetof(L1TxDataPduInfo_t, TLVs));
        pTxdataPdu->PDULength = offset - PduSize;
    }
    return offset;
}

static int appMgrMsgqHRecv(void* pArg, void* pMsg, uint32_t msgLen)
{
    uint8_t dev_index = 0, cellId = 0;

    static uint8_t cellNum = 1;

    ASSERT(pMsg);
    ASSERT(msgLen > 0);
    if (NULL != pArg) {
        CMM_DEBUG(LOG_MAC, "pArg %p is not NULL", pArg);
    }

    // printf("appMsg recv, dev_index:%d cellId:%d\n",dev_index,cellId);
    CtrlMsg_s* msg = (CtrlMsg_s*)pMsg;
    dev_index = msg->paras[2];
    cellId = msg->paras[3];
    printf("appMsg recv, dev_index:%d cellId:%d\n", dev_index, cellId);
    switch (msg->msgId) {
    case APP_TC_CONFIG_CASE: {
        sendPhyParamReq(dev_index, cellId);
        sendPhyConfigReq(dev_index, cellId);
        setPhyState(dev_index, cellId, PHY_CFG);
    } break;

    case APP_TC_DL_CASE: {
        gL2TxGSfn[dev_index][cellId].SFN = msg->paras[0];
        gL2TxGSfn[dev_index][cellId].Slot = msg->paras[1];
        sendSingleDlTTIReq(gL2TxGSfn[dev_index][cellId].SFN, gL2TxGSfn[dev_index][cellId].Slot, dev_index, cellId);
    } break;

    case APP_TC_UL_CASE: {
        gL2RxGSfn[dev_index][cellId].SFN = msg->paras[0];
        gL2RxGSfn[dev_index][cellId].Slot = msg->paras[1];
        sendSingleUlTTIReq(gL2RxGSfn[dev_index][cellId].SFN, gL2RxGSfn[dev_index][cellId].Slot, dev_index, cellId);
    } break;

    case APP_TC_TX_CASE: {
        gL2TxGSfn[dev_index][cellId].SFN = msg->paras[0];
        gL2TxGSfn[dev_index][cellId].Slot = msg->paras[1];
        sendSingleTxDataReq(gL2TxGSfn[dev_index][cellId].SFN, gL2TxGSfn[dev_index][cellId].Slot, dev_index, cellId);
    } break;

    case APP_TX_CASE: {
        if ((msg->paras[1] != 0) && (msg->paras[1] <= MAC_MAX_CELL_NUM)) {
            cellNum = msg->paras[1];
        }
        gCellNum = cellNum;
        macTcLoad(msg->paras[0], cellId, dev_index);
        gMacCtx[dev_index][cellId].runMode = RUNMODE_FAPI_GENERATOR;
        if (PHY_IDLE != getPhyState(dev_index, cellId)) {
            sendPhyStopReq(dev_index, cellId);
            if (TRUE == gMacCtx[dev_index][cellId].phyStub) {
                L1ErrorIndication_t _ind;
                sleep(1);
                _ind.MessageId = L1_STOP_REQUEST;
                _ind.ErrorCode = 0;
                macRevErrorIndication(&_ind, dev_index, cellId);
            }
        } else {
            // sendPhyParamReq();
            sendPhyConfigReq(dev_index, cellId);
            setPhyState(dev_index, cellId, PHY_CFG);
            sleep(1);

            if (TRUE == gMacCtx[dev_index][cellId].phyStub) {
                L1ConfigResponse_t _rsp;
                sleep(1);
                _rsp.ErrorCode = 0;
                macRevCfgResonse(&_rsp, dev_index, cellId);
            }
        }
        gMacCtx[dev_index][cellId].schState = SCH_NORMAL;

    } break;

    case APP_TC_STOP: {
        for (cellId = 0; cellId < cellNum; cellId++) {
            printf("sendPhyStopReq, cellid: %d\n", cellId);
            sendPhyStopReq(dev_index, cellId);
            gMacCtx[dev_index][cellId].schState = SCH_STOP;
        }
    } break;

    case APP_START_TEST: {
        sTcIdInRunning = msg->paras[0];
        for (cellId = 0; cellId < cellNum; cellId++) {
            if (PHY_IDLE != getPhyState(dev_index, cellId)) {
                sendPhyStopReq(dev_index, cellId);
                if (TRUE == gMacCtx[dev_index][cellId].phyStub) {
                    L1ErrorIndication_t _ind;
                    sleep(1);
                    _ind.MessageId = L1_STOP_REQUEST;
                    _ind.ErrorCode = 0;
                    macRevErrorIndication(&_ind, dev_index, cellId);
                }
            } else {
                sendPhyParamReq(dev_index, cellId);
                sendPhyConfigReq(dev_index, cellId);
                setPhyState(dev_index, cellId, PHY_CFG);
                sleep(1);

                if (TRUE == gMacCtx[dev_index][cellId].phyStub) {
                    L1ConfigResponse_t _rsp;
                    sleep(1);
                    _rsp.ErrorCode = 0;
                    macRevCfgResonse(&_rsp, dev_index, cellId);
                }
            }
            gMacCtx[dev_index][cellId].schState = SCH_NORMAL;
        }
    } break;

    default:
        return -1;
    }

    return 0;
}

void macRevAppCtrlMsg(uint8_t dev_index, uint8_t cellId)
{
    gMacCtx[dev_index][cellId].msgqH.pfnQuickMsgRingRecvMsgWithUnpackFunc(&gMacCtx[dev_index][cellId].msgqApp, appMgrMsgqHRecv, NULL);
}
