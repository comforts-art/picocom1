/*
 * PICOCOM PROPRIETARY INFORMATION
 *
 * This software is supplied under the terms of a license agreement or
 * nondisclosure agreement with PICOCOM and may not be copied
 * or disclosed except in accordance with the terms of that agreement.
 *
 * Copyright PICOCOM.
 */

#ifndef __UU_MON_MSG_H_
#define __UU_MON_MSG_H_
#include "cmmTypes.h"

typedef struct
    {
    uint32_t msgIdx;
    uint32_t objIdx;
    uint32_t itemCnt;
    uint32_t item[5];
} UuMonMsg_s;

void sendUuMonMsg(const UuMonMsg_s* msg);

typedef enum {
    /**3 item, Ul RTWP. DL Tx Powr, DL Tx Powr No Hsdsch*/
    UU_MON_CELL_MEA_INFO,
    /**4 item, SIR, SIR ERROR, Tx Power, RTT*/
    UU_MON_UE_L1_MEA,
    /**1 item, Downlink CPICH Ec/I0(Or RSCP)*/
    UU_MON_DL_EC_IO,

    /**RRC Setup Indication event, no item include*/
    UU_MON_RL_SETUP_INDI,
    /**RRC Release Indication event, no item include*/
    UU_MON_RL_RELEASE_INDI,
    /**RRC Radio Link failure event, no item include*/
    UU_MON_UL_RL_FAILURE_INDI,
    /**RRC CELL SETUP  event, no item include*/
    UU_MON_CELL_SETUP_INDI,
    /**RRC CELL DELETE  event, no item include*/
    UU_MON_CELL_DEL_INDI
} UuMonType_e;

#endif