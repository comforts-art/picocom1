/*
 * PICOCOM PROPRIETARY INFORMATION
 *
 * This software is supplied under the terms of a license agreement or
 * nondisclosure agreement with PICOCOM and may not be copied
 * or disclosed except in accordance with the terms of that agreement.
 *
 * Copyright PICOCOM.
 */

#ifndef __MAC_TX_H__
#define __MAC_TX_H__
#include "mac.h"
#include "cmmTypes.h"

void setL1CtrlMsgGrpHeader(uint8_t dev_index, uint8_t cellId);

#ifndef MAC_MULTI_PC802
#define START_L1_MSG_PROC(dev_index, cellId)      \
    do {                                          \
        if (0 != pcxxSendStart()) {               \
            printf("pcxx start send fail\n");     \
            return;                               \
        }                                         \
        setL1CtrlMsgGrpHeader(dev_index, cellId); \
    } while (0)

#define END_L1_MSG_PROC(dev_index, cellId)  \
    do {                                    \
        if (0 != pcxxSendEnd()) {           \
            printf("pcxx end send fail\n"); \
            return;                         \
        }                                   \
        traceL1CtrlMsg(dev_index, cellId);  \
    } while (0)

#else
#define START_L1_MSG_PROC(dev_index, cellId)         \
    do {                                             \
        if (0 != pcxxSendStart(dev_index, cellId)) { \
            printf("pcxx start send fail\n");        \
            return;                                  \
        }                                            \
        setL1CtrlMsgGrpHeader(dev_index, cellId);    \
    } while (0)

#define END_L1_MSG_PROC(dev_index, cellId)         \
    do {                                           \
        if (0 != pcxxSendEnd(dev_index, cellId)) { \
            printf("pcxx end send fail\n");        \
            return;                                \
        }                                          \
        traceL1CtrlMsg(dev_index, cellId);         \
    } while (0)
#endif

void macTtiTx(uint16_t sfn, uint8_t slot, uint8_t dev_index, uint8_t cellId);
void sendPhyConfigReq(uint8_t dev_index, uint8_t cellId);
void sendPhyParamReq(uint8_t dev_index, uint8_t cellId);
void sendPhyStartReq(uint8_t dev_index, uint8_t cellId);
void sendPhyStopReq(uint8_t dev_index, uint8_t cellId);
void sendDlTTIReq(uint16_t sfn, uint8_t slot, uint8_t dev_index, uint8_t cellId);
void sendUlTTIReq(uint16_t sfn, uint8_t slot, uint8_t dev_index, uint8_t cellId);
void sendTxdataReq(uint16_t sfn, uint8_t slot, uint8_t dev_index, uint8_t cellId);
void sendSingleDlTTIReq(uint16_t sfn, uint8_t slot, uint8_t dev_index, uint8_t cellId);
void sendSingleUlTTIReq(uint16_t sfn, uint8_t slot, uint8_t dev_index, uint8_t cellId);
void sendSingleTxDataReq(uint16_t sfn, uint8_t slot, uint8_t dev_index, uint8_t cellId);

#endif
